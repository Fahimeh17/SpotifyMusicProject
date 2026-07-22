#ifndef LISTENERPAGE_H
#define LISTENERPAGE_H

#include <QWidget>
#include <QListWidgetItem>
#include "services/ListenerService.h"

namespace Ui { class ListenerPage; }

class ListenerPage : public QWidget
{
    Q_OBJECT
public:
    explicit ListenerPage(ListenerService &listenerService, QWidget *parent = nullptr);
    ~ListenerPage();

    void setListener(int listenerId, const QString &fullName);
    void refresh();

signals:
    // playlistId == 0 يعني همون پلی‌لیست ثابت Favorite Songs
    void openPlaylistRequested(int listenerId, int playlistId, const QString &playlistName);
    void exploreRequested(int listenerId);
    void logoutRequested();

private slots:
    void on_buttonAddPlaylist_clicked();
    void on_buttonEditPlaylist_clicked();
    void on_buttonDeletePlaylist_clicked();
    void on_buttonExplore_clicked();
    void on_buttonLogout_clicked();
    void on_listWidgetPlaylists_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::ListenerPage *ui;
    ListenerService &listenerService;
    int listenerId = 0;
};
#endif // LISTENERPAGE_H
