#ifndef ARTISTPAGE_H
#define ARTISTPAGE_H

#include <QWidget>
#include<QListWidgetItem>
#include "services/artistservice.h"

namespace Ui { class ArtistPage; }

class ArtistPage : public QWidget
{
    Q_OBJECT
public:
    explicit ArtistPage(ArtistService &artistService, QWidget *parent = nullptr);
    ~ArtistPage();

    void setArtist(int artistId, const QString &fullName);
    void refresh();

signals:
    void openAlbumRequested(int artistId, int albumId, const QString &albumName);
    void logoutRequested();

private slots:
    void on_buttonAddAlbum_clicked();
    void on_buttonAddSong_clicked();
    void on_buttonEditAlbum_clicked();
    void on_buttonDeleteAlbum_clicked();
    void on_buttonLogout_clicked();
    void on_listWidgetAlbums_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::ArtistPage *ui;
    ArtistService &artistService;
    int artistId = 0;

    void openCreateOrEditSongDialog(int existingSongId = 0);
};

#endif // ARTISTPAGE_H
