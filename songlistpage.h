#ifndef SONGLISTPAGE_H
#define SONGLISTPAGE_H

#include <QWidget>
#include <QListWidgetItem>
#include "services/ArtistService.h"
#include "services/ListenerService.h"

namespace Ui { class SongListPage; }

class SongListPage : public QWidget
{
    Q_OBJECT
public:
    enum class Mode { ArtistAlbum, ListenerPlaylist, ListenerBrowseAlbum };

    explicit SongListPage(ArtistService &artistService, ListenerService &listenerService,
                          QWidget *parent = nullptr);
    ~SongListPage();

    void openArtistAlbum(int artistId, int albumId, const QString &title);
    void openListenerPlaylist(int listenerId, int playlistId, const QString &title); // playlistId==0 => Favorites
    void openBrowseAlbum(int listenerId, int artistId, int albumId, const QString &title);

signals:
    void backRequested();

private slots:
    void on_buttonAddSong_clicked();
    void on_buttonEditSong_clicked();
    void on_buttonDeleteSong_clicked();
    void on_buttonLike_clicked();
    void on_buttonBack_clicked();
    void on_lineEditSearch_textChanged(const QString &text);
    void on_comboBoxGenreFilter_currentIndexChanged(int index);
    void on_buttonSortByName_clicked();
    void on_buttonSortByYear_clicked();
    void on_listWidgetSongs_itemSelectionChanged();

private:
    Ui::SongListPage *ui;
    ArtistService &artistService;
    ListenerService &listenerService;

    Mode mode = Mode::ArtistAlbum;
    int artistId = 0;
    int albumId = 0;
    int listenerId = 0;
    int playlistId = 0;

    std::vector<Song> baseSongs;    // آخرین لیست خام گرفته‌شده از سرویس (قبل از جستجو/فیلتر)
    std::vector<Song> displayedSongs; // نسخه‌ی نمایشی فعلی (بعد از جستجو/فیلتر/مرتب‌سازی)

    void configureButtonsForMode();
    void loadBaseSongs();
    void populateGenreFilter();
    void applySearchFilterAndRender();
    void render(const std::vector<Song> &songs);
    int currentSelectedSongId();
};
#endif // SONGLISTPAGE_H
