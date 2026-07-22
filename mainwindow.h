#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

#include "repositories/ArtistRepository.h"
#include "repositories/ListenerRepository.h"
#include "repositories/SongRepository.h"
#include "repositories/AlbumRepository.h"
#include "repositories/PlaylistRepository.h"
#include "services/AuthService.h"
#include "services/ArtistService.h"
#include "services/ListenerService.h"

#include "loginpage.h"
#include "registerpage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void handleAuthSucceeded(Role role, int accountId);
    void goToRegisterPage();
    void goToLoginPage();

private:
    Ui::MainWindow *ui;

    // ترتیب اعلان مهم است: باید قبل از سرویس‌ها ساخته شوند
    ArtistRepository artistRepo;
    ListenerRepository listenerRepo;
    SongRepository songRepo;
    AlbumRepository albumRepo;
    PlaylistRepository playlistRepo;

    AuthService authService;
    ArtistService artistService;
    ListenerService listenerService;

    QStackedWidget *stackedWidget;
    LoginPage *loginPage;
    RegisterPage *registerPage;
};
#endif // MAINWINDOW_H
