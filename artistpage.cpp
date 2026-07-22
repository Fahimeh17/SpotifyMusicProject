#include "artistpage.h"
#include "ui_artistpage.h"

#include "services/SortFilterUtils.h"
#include "exceptions/Exceptions.h"
#include <QListWidgetItem>
#include <QInputDialog>
#include <QMessageBox>

ArtistPage::ArtistPage(ArtistService &artistService, QWidget *parent)
    : QWidget(parent), ui(new Ui::ArtistPage), artistService(artistService)
{
    ui->setupUi(this);
}

ArtistPage::~ArtistPage() { delete ui; }

void ArtistPage::setArtist(int newArtistId, const QString &fullName)
{
    artistId = newArtistId;
    ui->labelWelcome->setText("Welcome back, " + fullName + "!");
    refresh();
}

void ArtistPage::refresh()
{
    ui->listWidgetAlbums->clear();

    // ردیف اول همیشه Singles است (فقط اگر آهنگ Single‌ای وجود داشته باشد نمایش می‌دهیم)
    auto singles = artistService.getSingles(artistId);
    if (!singles.empty()) {
        auto *singlesItem = new QListWidgetItem(QString::fromStdString(SINGLES_ALBUM_NAME));
        singlesItem->setData(Qt::UserRole, 0); // albumId = 0 یعنی Singles
        ui->listWidgetAlbums->addItem(singlesItem);
    }

    auto albums = SortFilterUtils::sortAlbumsByName(artistService.getAlbums(artistId));
    for (auto &album : albums) {
        auto *item = new QListWidgetItem(QString::fromStdString(album.getName()));
        item->setData(Qt::UserRole, album.getId());
        ui->listWidgetAlbums->addItem(item);
    }
}

void ArtistPage::on_buttonAddAlbum_clicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, "آلبوم جدید", "نام آلبوم:", QLineEdit::Normal, "", &ok);
    if (!ok || name.trimmed().isEmpty()) return;
    try {
        artistService.createAlbum(artistId, name.trimmed().toStdString());
        refresh();
    } catch (const AppException &ex) {
        QMessageBox::warning(this, "خطا", QString::fromStdString(ex.what()));
    }
}

void ArtistPage::on_buttonAddSong_clicked() { openCreateOrEditSongDialog(0); }

void ArtistPage::openCreateOrEditSongDialog(int existingSongId)
{
    bool isEdit = existingSongId != 0;
    Song current;
    if (isEdit) {
        auto found = artistService.getSongsOfAlbum(0); // placeholder, real lookup happens in SongListPage
    }

    bool ok;
    QString name = QInputDialog::getText(this, "آهنگ", "نام آهنگ:", QLineEdit::Normal, "", &ok);
    if (!ok || name.trimmed().isEmpty()) return;

    int year = QInputDialog::getInt(this, "آهنگ", "سال انتشار:", 2024, 1900, 2100, 1, &ok);
    if (!ok) return;

    QString genre = QInputDialog::getText(this, "آهنگ", "سبک:", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    QString audioFile = QInputDialog::getText(this, "آهنگ", "نام فایل صوتی:", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    // انتخاب آلبوم (یا Single)
    QStringList albumChoices;
    QList<int> albumIds;
    albumChoices << "بدون آلبوم (Single)";
    albumIds << 0;
    for (auto &album : artistService.getAlbums(artistId)) {
        albumChoices << QString::fromStdString(album.getName());
        albumIds << album.getId();
    }
    QString chosen = QInputDialog::getItem(this, "آهنگ", "آلبوم:", albumChoices, 0, false, &ok);
    if (!ok) return;
    int albumId = albumIds[albumChoices.indexOf(chosen)];

    try {
        artistService.createSong(artistId, name.trimmed().toStdString(), year,
                                 genre.trimmed().toStdString(), audioFile.trimmed().toStdString(), albumId);
        refresh();
    } catch (const AppException &ex) {
        QMessageBox::warning(this, "خطا", QString::fromStdString(ex.what()));
    }
}

void ArtistPage::on_buttonEditAlbum_clicked()
{
    auto *item = ui->listWidgetAlbums->currentItem();
    if (!item) { QMessageBox::information(this, "توجه", "یک آلبوم انتخاب کنید."); return; }
    int albumId = item->data(Qt::UserRole).toInt();
    if (albumId == 0) { QMessageBox::information(this, "توجه", "Singles یک آلبوم واقعی نیست."); return; }

    bool ok;
    QString newName = QInputDialog::getText(this, "ویرایش آلبوم", "نام جدید:", QLineEdit::Normal, item->text(), &ok);
    if (!ok || newName.trimmed().isEmpty()) return;
    try {
        artistService.editAlbum(albumId, newName.trimmed().toStdString());
        refresh();
    } catch (const AppException &ex) {
        QMessageBox::warning(this, "خطا", QString::fromStdString(ex.what()));
    }
}

void ArtistPage::on_buttonDeleteAlbum_clicked()
{
    auto *item = ui->listWidgetAlbums->currentItem();
    if (!item) { QMessageBox::information(this, "توجه", "یک آلبوم انتخاب کنید."); return; }
    int albumId = item->data(Qt::UserRole).toInt();
    if (albumId == 0) { QMessageBox::information(this, "توجه", "Singles قابل حذف نیست."); return; }

    auto reply = QMessageBox::question(this, "حذف آلبوم",
                                       "آلبوم «" + item->text() + "» و تمام آهنگ‌های آن حذف می‌شود. ادامه می‌دهید؟");
    if (reply != QMessageBox::Yes) return;

    try {
        artistService.deleteAlbum(albumId);
        refresh();
    } catch (const AppException &ex) {
        QMessageBox::warning(this, "خطا", QString::fromStdString(ex.what()));
    }
}

void ArtistPage::on_buttonLogout_clicked() { emit logoutRequested(); }

void ArtistPage::on_listWidgetAlbums_itemDoubleClicked(QListWidgetItem *item)
{
    int albumId = item->data(Qt::UserRole).toInt();
    emit openAlbumRequested(artistId, albumId, item->text());
}