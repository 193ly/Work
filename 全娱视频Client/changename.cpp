#include "changename.h"
#include "ui_changename.h"
#include<QMessageBox>
ChangeName::ChangeName(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ChangeName)
{
    ui->setupUi(this);
}

ChangeName::~ChangeName()
{
    delete ui;
}

void ChangeName::on_pb_submit_clicked()
{
    QString name =ui->le_name->text();
    if(!name.isEmpty())
    {
        this->hide();
        ui->le_name->clear();
        emit SIG_submit(name);
    }
    else
    {
        QMessageBox::about(this, "提示", "名字不能为空");
    }
}

