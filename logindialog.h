#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QWidget>
#include"packdef.h"
#include<TcpClientMediator.h>
namespace Ui {
class LoginDialog;
}

class LoginDialog : public QWidget
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    void setHobby(HOBBY& hobby);
    void setTcp(TcpClientMediator* tcp){
        m_tcp = tcp;
    }
private slots:
    void on_pb_login_clicked();

    void on_cb_all_stateChanged(int arg1);

    void on_pb_regsiter_clicked();

    void slot_show();
signals:
    SIG_login(QString tel, QString password);
    SIG_register(QString name, QString tel, QString password, HOBBY hobby);
private:
    Ui::LoginDialog *ui;
    TcpClientMediator* m_tcp;
};

#endif // LOGINDIALOG_H
