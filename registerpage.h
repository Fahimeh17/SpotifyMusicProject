#ifndef REGISTERPAGE_H
#define REGISTERPAGE_H

#include <QWidget>
#include "entities/Account.h"
#include "services/AuthService.h"

namespace Ui { class RegisterPage; }

class RegisterPage : public QWidget
{
    Q_OBJECT
public:
    explicit RegisterPage(AuthService &authService, QWidget *parent = nullptr);
    ~RegisterPage();

signals:
    void registerSucceeded(Role role, int accountId);
    void cancelRequested();

private slots:
    void on_buttonOK_clicked();
    void on_buttonCancel_clicked();

private:
    Ui::RegisterPage *ui;
    AuthService &authService;
    void clearFields();
};

#endif // REGISTERPAGE_H
