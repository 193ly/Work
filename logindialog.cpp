#include "logindialog.h"
#include "ui_logindialog.h"
#include<QMessageBox>
#include<QDebug>
LoginDialog::LoginDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginDialog)
{
    //防止connect不认识自己定义的类型
    qRegisterMetaType<HOBBY>("HOBBY");
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::setHobby(HOBBY& hobby)
{
    hobby.dance = ui->cb_dance->isChecked() ? 1 : 0;
    hobby.edu = ui->cb_educate->isChecked()? 1 : 0;
    hobby.ennegy = ui->cb_ennegy->isChecked()? 1 : 0;
    hobby.food = ui->cb_food->isChecked()? 1 : 0;
    hobby.funny = ui->cb_funny->isChecked()? 1 : 0;
    hobby.music = ui->cb_music->isChecked()? 1 : 0;
    hobby.outside = ui->cb_outdoor->isChecked()? 1 : 0;
    hobby.video = ui->cb_video->isChecked()? 1 : 0;
}

void LoginDialog::on_pb_login_clicked()
{
    QString tel = ui->le_tel->text();
    QString password = ui->le_password->text();

    if(tel.isEmpty())
    {
        QMessageBox::about(this, "错误", "手机号不能为空");
        return;
    }
    if(tel.size() != 11)
    {
        QMessageBox::about(this, "错误","手机号长度错误");
        return;
    }
    if(password.isEmpty())
    {
        QMessageBox::about(this, "错误", "密码不能为空");
        return;
    }
    for(char c:tel.toStdString())
    {
        if(!(c >= '0' && c <= '9'))
        {
            QMessageBox::about(this, "错误","手机号错误");
            return;
        }
    }


    //_sleep(2000);
    Q_EMIT SIG_login(tel, password);
}


void LoginDialog::on_cb_all_stateChanged(int arg1)
{
    if(arg1 == 0)
    {
        ui->cb_dance->setChecked(false);
        ui->cb_educate->setChecked(false);
        ui->cb_ennegy->setChecked(false);
        ui->cb_food->setChecked(false);
        ui->cb_funny->setChecked(false);
        ui->cb_music->setChecked(false);
        ui->cb_outdoor->setChecked(false);
        ui->cb_video->setChecked(false);
    }
    else
    {
        ui->cb_dance->setChecked(true);
        ui->cb_educate->setChecked(true);
        ui->cb_ennegy->setChecked(true);
        ui->cb_food->setChecked(true);
        ui->cb_funny->setChecked(true);
        ui->cb_music->setChecked(true);
        ui->cb_outdoor->setChecked(true);
        ui->cb_video->setChecked(true);
    }
}

void LoginDialog::on_pb_regsiter_clicked()
{

    QString name = ui->le_resiger_name->text();
    QString tel = ui->le_resiger_tel->text();
    QString password = ui->le_resiger_password->text();
    QString confrimPassword = ui->le_resiger_confirm->text();
    if(name.isEmpty())
    {
        QMessageBox::about(this, "错误", "用户名不能为空");
        return;
    }
    if(tel.isEmpty())
    {
        QMessageBox::about(this, "错误", "手机号不能为空");
        return;
    }
    if(tel.size() != 11)
    {
        QMessageBox::about(this, "错误","手机号长度错误");
        return;
    }
    if(password.isEmpty())
    {
        QMessageBox::about(this, "错误", "密码不能为空");
        return;
    }
    if(confrimPassword != password)
        QMessageBox::about(this, "错误", "两次的密码不同");
    for(char c:tel.toStdString())
    {
        if(!(c >= '0' && c <= '9'))
        {
            QMessageBox::about(this, "错误","手机号错误");
            return;
        }
    }


   // _sleep(2000);
    HOBBY hobby;
    setHobby(hobby);
    Q_EMIT SIG_register(name, tel, password, hobby);
}

void LoginDialog::slot_show()
{
    this->show();
}
