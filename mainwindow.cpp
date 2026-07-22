#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , authService(artistRepo, listenerRepo)
    , artistService(artistRepo, albumRepo, songRepo, playlistRepo)
    , listenerService(listenerRepo, playlistRepo, songRepo, artistRepo, albumRepo)
{
    ui->setupUi(this);

    stackedWidget = new QStackedWidget(this);
    loginPage = new LoginPage(authService, this);
    registerPage = new RegisterPage(authService, this);
    stackedWidget->addWidget(loginPage);
    stackedWidget->addWidget(registerPage);
    setCentralWidget(stackedWidget);

    connect(loginPage, &LoginPage::loginSucceeded, this, &MainWindow::handleAuthSucceeded);
    connect(loginPage, &LoginPage::registerRequested, this, &MainWindow::goToRegisterPage);
    connect(registerPage, &RegisterPage::registerSucceeded, this, &MainWindow::handleAuthSucceeded);
    connect(registerPage, &RegisterPage::cancelRequested, this, &MainWindow::goToLoginPage);

    stackedWidget->setCurrentWidget(loginPage);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::goToRegisterPage() { stackedWidget->setCurrentWidget(registerPage); }
void MainWindow::goToLoginPage() { stackedWidget->setCurrentWidget(loginPage); }

void MainWindow::handleAuthSucceeded(Role role, int accountId)
{
    QString roleText = (role == Role::Artist) ? "هنرمند" : "شنونده";
    QMessageBox::information(this, "ورود موفق",
                             "با نقش " + roleText + " و شناسه‌ی " + QString::number(accountId) + " وارد شدید.\n(صفحه‌ی اختصاصی، قدم بعدی است)");
}

