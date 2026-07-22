#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>
#include "entities/Account.h"
#include "services/AuthService.h"

namespace Ui { class LoginPage; }

class LoginPage : public QWidget
{
    Q_OBJECT
public:
    explicit LoginPage(AuthService &authService, QWidget *parent = nullptr);
    ~LoginPage();

signals:
    void loginSucceeded(Role role, int accountId);
    void registerRequested();

private slots:
    void on_buttonLogin_clicked();
    void on_buttonRegister_clicked();

private:
    Ui::LoginPage *ui;
    AuthService &authService;
};

#endif // LOGINPAGE_H
