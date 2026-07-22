#include "loginpage.h"
#include "ui_loginpage.h"
#include "exceptions/Exceptions.h"

LoginPage::LoginPage(AuthService &authService, QWidget *parent)
    : QWidget(parent), ui(new Ui::LoginPage), authService(authService)
{
    ui->setupUi(this);
    ui->labelError->setText("");
    ui->lineEditPassword->setEchoMode(QLineEdit::Password);
}

LoginPage::~LoginPage() { delete ui; }

void LoginPage::on_buttonLogin_clicked()
{
    QString userName = ui->lineEditUserName->text().trimmed();
    QString password = ui->lineEditPassword->text();

    if (userName.isEmpty() || password.isEmpty()) {
        ui->labelError->setText("نام کاربری و رمز عبور را وارد کنید.");
        return;
    }
    try {
        LoginResult result = authService.login(userName.toStdString(), password.toStdString());
        ui->labelError->setText("");
        ui->lineEditPassword->clear();
        emit loginSucceeded(result.role, result.accountId);
    } catch (const AppException &ex) {
        ui->labelError->setText(QString::fromStdString(ex.what()));
    }
}

void LoginPage::on_buttonRegister_clicked() { emit registerRequested(); }