#include "listenerpage.h"
#include "ui_listenerpage.h"
#include "services/SortFilterUtils.h"
#include "exceptions/Exceptions.h"
#include <QInputDialog>
#include <QMessageBox>

ListenerPage::ListenerPage(ListenerService &listenerService, QWidget *parent)
    : QWidget(parent), ui(new Ui::ListenerPage), listenerService(listenerService)
{
    ui->setupUi(this);
}

ListenerPage::~ListenerPage() { delete ui; }

void ListenerPage::setListener(int newListenerId, const QString &fullName)
{
    listenerId = newListenerId;
    ui->labelWelcome->setText("Welcome back, " + fullName + "!");
    refresh();
}

void ListenerPage::refresh()
{
    ui->listWidgetPlaylists->clear();

    // ردیف اول همیشه Favorite Songs است (ثابت، حتی اگر خالی باشد)
    auto *favItem = new QListWidgetItem(QString::fromStdString(FAVORITE_SONGS_NAME));
    favItem->setData(Qt::UserRole, 0); // playlistId = 0 یعنی Favorite Songs
    ui->listWidgetPlaylists->addItem(favItem);

    auto playlists = SortFilterUtils::sortPlaylistsByName(listenerService.getPlaylists(listenerId));
    for (auto &p : playlists) {
        auto *item = new QListWidgetItem(QString::fromStdString(p.getName()));
        item->setData(Qt::UserRole, p.getId());
        ui->listWidgetPlaylists->addItem(item);
    }
}

void ListenerPage::on_buttonAddPlaylist_clicked()
{
    bool ok;
    QString name = QInputDialog::getText(this, "پلی‌لیست جدید", "نام:", QLineEdit::Normal, "", &ok);
    if (!ok || name.trimmed().isEmpty()) return;
    try {
        listenerService.createPlaylist(listenerId, name.trimmed().toStdString());
        refresh();
    } catch (const AppException &ex) {
        QMessageBox::warning(this, "خطا", QString::fromStdString(ex.what()));
    }
}

void ListenerPage::on_buttonEditPlaylist_clicked()
{
    auto *item = ui->listWidgetPlaylists->currentItem();
    if (!item) { QMessageBox::information(this, "توجه", "یک لیست پخش انتخاب کنید."); return; }
    int playlistId = item->data(Qt::UserRole).toInt();
    if (playlistId == 0) { QMessageBox::information(this, "توجه", "Favorite Songs قابل ویرایش نیست."); return; }

    bool ok;
    QString newName = QInputDialog::getText(this, "ویرایش لیست پخش", "نام جدید:", QLineEdit::Normal, item->text(), &ok);
    if (!ok || newName.trimmed().isEmpty()) return;
    try {
        listenerService.editPlaylist(playlistId, listenerId, newName.trimmed().toStdString());
        refresh();
    } catch (const AppException &ex) {
        QMessageBox::warning(this, "خطا", QString::fromStdString(ex.what()));
    }
}

void ListenerPage::on_buttonDeletePlaylist_clicked()
{
    auto *item = ui->listWidgetPlaylists->currentItem();
    if (!item) { QMessageBox::information(this, "توجه", "یک لیست پخش انتخاب کنید."); return; }
    int playlistId = item->data(Qt::UserRole).toInt();
    if (playlistId == 0) { QMessageBox::information(this, "توجه", "Favorite Songs قابل حذف نیست."); return; }

    auto reply = QMessageBox::question(this, "حذف لیست پخش",
                                       "لیست «" + item->text() + "» حذف می‌شود (خود آهنگ‌ها از سیستم حذف نمی‌شوند). ادامه می‌دهید؟");
    if (reply != QMessageBox::Yes) return;

    try {
        listenerService.deletePlaylist(playlistId, listenerId);
        refresh();
    } catch (const AppException &ex) {
        QMessageBox::warning(this, "خطا", QString::fromStdString(ex.what()));
    }
}

void ListenerPage::on_buttonExplore_clicked() { emit exploreRequested(listenerId); }
void ListenerPage::on_buttonLogout_clicked() { emit logoutRequested(); }

void ListenerPage::on_listWidgetPlaylists_itemDoubleClicked(QListWidgetItem *item)
{
    int playlistId = item->data(Qt::UserRole).toInt();
    emit openPlaylistRequested(listenerId, playlistId, item->text());
}