#include "registerpage.h"
#include "ui_registerpage.h"
#include "exceptions/Exceptions.h"

RegisterPage::RegisterPage(AuthService &authService, QWidget *parent)
    : QWidget(parent), ui(new Ui::RegisterPage), authService(authService)
{
    ui->setupUi(this);
    ui->labelError->setText("");
    ui->radioButtonListener->setChecked(true);
}

RegisterPage::~RegisterPage() { delete ui; }

void RegisterPage::clearFields()
{
    ui->lineEditFullName->clear();
    ui->lineEditUserName->clear();
    ui->lineEditPassword->clear();
    ui->textEditBiography->clear();
    ui->radioButtonListener->setChecked(true);
    ui->labelError->setText("");
}

void RegisterPage::on_buttonOK_clicked()
{
    QString fullName = ui->lineEditFullName->text().trimmed();
    QString userName = ui->lineEditUserName->text().trimmed();
    QString password = ui->lineEditPassword->text();
    QString biography = ui->textEditBiography->toPlainText();

    if (fullName.isEmpty() || userName.isEmpty() || password.isEmpty()) {
        ui->labelError->setText("نام، نام‌کاربری و رمز عبور نباید خالی باشند.");
        return;
    }
    try {
        if (ui->radioButtonArtist->isChecked()) {
            auto artist = authService.registerArtist(fullName.toStdString(), userName.toStdString(),
                                                     password.toStdString(), biography.toStdString());
            clearFields();
            emit registerSucceeded(Role::Artist, artist.getId());
        } else {
            auto listener = authService.registerListener(fullName.toStdString(), userName.toStdString(),
                                                         password.toStdString(), biography.toStdString());
            clearFields();
            emit registerSucceeded(Role::Listener, listener.getId());
        }
    } catch (const AppException &ex) {
        ui->labelError->setText(QString::fromStdString(ex.what()));
    }
}

void RegisterPage::on_buttonCancel_clicked()
{
    clearFields();
    emit cancelRequested();
}