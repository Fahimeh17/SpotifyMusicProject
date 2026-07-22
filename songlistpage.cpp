#include "songlistpage.h"
#include "ui_songlistpage.h"
#include "services/SortFilterUtils.h"
#include "exceptions/Exceptions.h"
#include <QInputDialog>
#include <QMessageBox>
#include <set>

SongListPage::SongListPage(ArtistService &artistService, ListenerService &listenerService, QWidget *parent)
    : QWidget(parent), ui(new Ui::SongListPage), artistService(artistService), listenerService(listenerService)
{
    ui->setupUi(this);
}

SongListPage::~SongListPage() { delete ui; }

// ---------- سه حالت ورود به این صفحه ----------

void SongListPage::openArtistAlbum(int newArtistId, int newAlbumId, const QString &title)
{
    mode = Mode::ArtistAlbum;
    artistId = newArtistId;
    albumId = newAlbumId;
    ui->labelTitle->setText(title);
    configureButtonsForMode();
    loadBaseSongs();
}

void SongListPage::openListenerPlaylist(int newListenerId, int newPlaylistId, const QString &title)
{
    mode = Mode::ListenerPlaylist;
    listenerId = newListenerId;
    playlistId = newPlaylistId;
    ui->labelTitle->setText(title);
    configureButtonsForMode();
    loadBaseSongs();
}

void SongListPage::openBrowseAlbum(int newListenerId, int newArtistId, int newAlbumId, const QString &title)
{
    mode = Mode::ListenerBrowseAlbum;
    listenerId = newListenerId;
    artistId = newArtistId;
    albumId = newAlbumId;
    ui->labelTitle->setText(title);
    configureButtonsForMode();
    loadBaseSongs();
}

// ---------- نمایش/مخفی‌کردن دکمه‌ها بر اساس حالت ----------

void SongListPage::configureButtonsForMode()
{
    switch (mode) {
    case Mode::ArtistAlbum:
        ui->buttonAddSong->setText("Add Song");
        ui->buttonAddSong->setVisible(true);
        ui->buttonEditSong->setVisible(true);
        ui->buttonDeleteSong->setText("Delete Song");
        ui->buttonDeleteSong->setVisible(true);
        ui->buttonLike->setVisible(false);
        break;
    case Mode::ListenerPlaylist:
        ui->buttonAddSong->setVisible(false);
        ui->buttonEditSong->setVisible(false);
        ui->buttonDeleteSong->setText("Remove from Playlist");
        ui->buttonDeleteSong->setVisible(true);
        ui->buttonLike->setVisible(true);
        break;
    case Mode::ListenerBrowseAlbum:
        ui->buttonAddSong->setText("Add to Playlist");
        ui->buttonAddSong->setVisible(true);
        ui->buttonEditSong->setVisible(false);
        ui->buttonDeleteSong->setVisible(false);
        ui->buttonLike->setVisible(true);
        break;
    }
}

// ---------- بارگذاری داده از سرویس (منبع واقعی) ----------

void SongListPage::loadBaseSongs()
{
    switch (mode) {
    case Mode::ArtistAlbum:
        baseSongs = (albumId == 0) ? artistService.getSingles(artistId)
                                   : artistService.getSongsOfAlbum(albumId);
        break;
    case Mode::ListenerPlaylist:
        baseSongs = (playlistId == 0) ? listenerService.getFavoriteSongs(listenerId)
                                      : listenerService.getSongsOfPlaylist(playlistId);
        break;
    case Mode::ListenerBrowseAlbum:
        baseSongs = (albumId == 0) ? listenerService.getArtistSingles(artistId)
                    : listenerService.getArtistAlbums(artistId).empty()
                        ? std::vector<Song>{}
                        : [&] {
                              // آهنگ‌های همین آلبوم خاص را از طریق ArtistService (فقط خواندنی) می‌گیریم
                              return artistService.getSongsOfAlbum(albumId);
                          }();
        break;
    }
    populateGenreFilter();
    applySearchFilterAndRender();
}

void SongListPage::populateGenreFilter()
{
    std::set<std::string> genres;
    for (auto &s : baseSongs) genres.insert(s.getGenre());

    ui->comboBoxGenreFilter->blockSignals(true);
    ui->comboBoxGenreFilter->clear();
    ui->comboBoxGenreFilter->addItem("All Genres");
    for (auto &g : genres) ui->comboBoxGenreFilter->addItem(QString::fromStdString(g));
    ui->comboBoxGenreFilter->blockSignals(false);
}


    void SongListPage::applySearchFilterAndRender()
{
    std::vector<Song> result = baseSongs;

    QString keyword = ui->lineEditSearch->text().trimmed();
    if (!keyword.isEmpty())
        result = SortFilterUtils::searchSongsByName(result, keyword.toStdString());

    QString genre = ui->comboBoxGenreFilter->currentText();
    if (!genre.isEmpty() && genre != "All Genres")
        result = SortFilterUtils::filterSongsByGenre(result, genre.toStdString());

    render(result);
}

void SongListPage::render(const std::vector<Song> &songs)
{
    displayedSongs = songs;
    ui->listWidgetSongs->clear();
    for (auto &s : displayedSongs) {
        QString text = QString::fromStdString(s.getName()) + "  —  " +
                       QString::number(s.getReleaseYear()) + "  —  " +
                       QString::fromStdString(s.getGenre());
        auto *item = new QListWidgetItem(text);
        item->setData(Qt::UserRole, s.getId());
        ui->listWidgetSongs->addItem(item);
    }
}

int SongListPage::currentSelectedSongId()
{
    auto *item = ui->listWidgetSongs->currentItem();
    return item ? item->data(Qt::UserRole).toInt() : 0;
}

// ---------- اسلات‌های نوار ابزار (جستجو/فیلتر/مرتب‌سازی) ----------

void SongListPage::on_lineEditSearch_textChanged(const QString &) { applySearchFilterAndRender(); }
void SongListPage::on_comboBoxGenreFilter_currentIndexChanged(int) { applySearchFilterAndRender(); }
void SongListPage::on_buttonSortByName_clicked() { render(SortFilterUtils::sortSongsByName(displayedSongs)); }
void SongListPage::on_buttonSortByYear_clicked() { render(SortFilterUtils::sortSongsByYear(displayedSongs)); }

void SongListPage::on_listWidgetSongs_itemSelectionChanged()
{
    if (mode != Mode::ListenerPlaylist && mode != Mode::ListenerBrowseAlbum) return;
    int songId = currentSelectedSongId();
    if (songId == 0) return;
    bool liked = listenerService.isLiked(listenerId, songId);
    ui->buttonLike->setText(liked ? "Unlike" : "Like");
}

// ---------- دکمه‌های اصلی ----------

void SongListPage::on_buttonAddSong_clicked()
{
    if (mode == Mode::ArtistAlbum) {
        bool ok;
        QString name = QInputDialog::getText(this, "آهنگ جدید", "نام آهنگ:", QLineEdit::Normal, "", &ok);
        if (!ok || name.trimmed().isEmpty()) return;
        int year = QInputDialog::getInt(this, "آهنگ جدید", "سال انتشار:", 2024, 1900, 2100, 1, &ok);
        if (!ok) return;
        QString genre = QInputDialog::getText(this, "آهنگ جدید", "سبک:", QLineEdit::Normal, "", &ok);
        if (!ok) return;
        QString audioFile = QInputDialog::getText(this, "آهنگ جدید", "نام فایل صوتی:", QLineEdit::Normal, "", &ok);
        if (!ok) return;
        try {
            artistService.createSong(artistId, name.trimmed().toStdString(), year,
                                     genre.trimmed().toStdString(), audioFile.trimmed().toStdString(), albumId);
            loadBaseSongs();
        } catch (const AppException &ex) {
            QMessageBox::warning(this, "خطا", QString::fromStdString(ex.what()));
        }
    } else if (mode == Mode::ListenerBrowseAlbum) {
        int songId = currentSelectedSongId();
        if (songId == 0) { QMessageBox::information(this, "توجه", "یک آهنگ انتخاب کنید."); return; }

        auto playlists = listenerService.getPlaylists(listenerId);
        if (playlists.empty()) { QMessageBox::information(this, "توجه", "اول یک لیست پخش بساز."); return; }

        QStringList names;
        QList<int> ids;
        for (auto &p : playlists) { names << QString::fromStdString(p.getName()); ids << p.getId(); }

        bool ok;
        QString chosen = QInputDialog::getItem(this, "افزودن به لیست پخش", "لیست پخش:", names, 0, false, &ok);
        if (!ok) return;
        int targetPlaylistId = ids[names.indexOf(chosen)];
        try {
            listenerService.addSongToPlaylist(targetPlaylistId, listenerId, songId);
            QMessageBox::information(this, "انجام شد", "آهنگ به لیست پخش اضافه شد.");
        } catch (const AppException &ex) {

            QMessageBox::warning(this, "خطا", QString::fromStdString(ex.what()));
        }
    }
}

void SongListPage::on_buttonEditSong_clicked()
{
    if (mode != Mode::ArtistAlbum) return;
    int songId = currentSelectedSongId();
    if (songId == 0) { QMessageBox::information(this, "توجه", "یک آهنگ انتخاب کنید."); return; }

    Song current;
    bool found = false;
    for (auto &s : baseSongs) if (s.getId() == songId) { current = s; found = true; break; }
    if (!found) return;

    bool ok;
    QString name = QInputDialog::getText(this, "ویرایش آهنگ", "نام آهنگ:", QLineEdit::Normal,
                                         QString::fromStdString(current.getName()), &ok);
    if (!ok || name.trimmed().isEmpty()) return;
    int year = QInputDialog::getInt(this, "ویرایش آهنگ", "سال انتشار:", current.getReleaseYear(), 1900, 2100, 1, &ok);
    if (!ok) return;
    QString genre = QInputDialog::getText(this, "ویرایش آهنگ", "سبک:", QLineEdit::Normal,
                                          QString::fromStdString(current.getGenre()), &ok);
    if (!ok) return;
    QString audioFile = QInputDialog::getText(this, "ویرایش آهنگ", "نام فایل صوتی:", QLineEdit::Normal,
                                              QString::fromStdString(current.getAudioFileName()), &ok);
    if (!ok) return;

    try {
        artistService.editSong(songId, name.trimmed().toStdString(), year, genre.trimmed().toStdString(),
                               audioFile.trimmed().toStdString(), current.getAlbumId());
        loadBaseSongs();
    } catch (const AppException &ex) {
        QMessageBox::warning(this, "خطا", QString::fromStdString(ex.what()));
    }
}

void SongListPage::on_buttonDeleteSong_clicked()
{
    int songId = currentSelectedSongId();
    if (songId == 0) { QMessageBox::information(this, "توجه", "یک آهنگ انتخاب کنید."); return; }

    try {
        if (mode == Mode::ArtistAlbum) {
            auto reply = QMessageBox::question(this, "حذف آهنگ", "این آهنگ کاملاً از سیستم حذف می‌شود. ادامه می‌دهید؟");
            if (reply != QMessageBox::Yes) return;
            artistService.deleteSong(songId);
        } else if (mode == Mode::ListenerPlaylist) {
            if (playlistId == 0) { QMessageBox::information(this, "توجه", "از Favorite Songs با دکمه‌ی Unlike حذف کن."); return; }
            listenerService.removeSongFromPlaylist(playlistId, listenerId, songId);
        }
        loadBaseSongs();
    } catch (const AppException &ex) {
        QMessageBox::warning(this, "خطا", QString::fromStdString(ex.what()));
    }
}

void SongListPage::on_buttonLike_clicked()
{
    int songId = currentSelectedSongId();
    if (songId == 0) { QMessageBox::information(this, "توجه", "یک آهنگ انتخاب کنید."); return; }

    bool liked = listenerService.isLiked(listenerId, songId);
    if (liked) listenerService.unlikeSong(listenerId, songId);
    else listenerService.likeSong(listenerId, songId);

    if (mode == Mode::ListenerPlaylist && playlistId == 0) loadBaseSongs(); // خودِ Favorites تغییر کرد
    else on_listWidgetSongs_itemSelectionChanged(); // فقط متن دکمه را عوض کن
}

void SongListPage::on_buttonBack_clicked() { emit backRequested(); }