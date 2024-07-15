#include "haveuploaddialog.h"
#include "ui_haveuploaddialog.h"

HaveUploadDialog::HaveUploadDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HaveUploadDialog)
{
    ui->setupUi(this);
    m_pLayOut = new QVBoxLayout;
    m_pLayOut->setContentsMargins(0,0,0,0);
    m_pLayOut->setSpacing(3);
    ui->widget->setLayout(m_pLayOut);

}

HaveUploadDialog::~HaveUploadDialog()
{
    delete ui;
    for(int i = 0; i < m_videoInfoItem.size(); i++)
    {
        VideoInfoItem* p = m_videoInfoItem[i];
        delete p;
        p = nullptr;
    }
    if(m_pLayOut)
        delete m_pLayOut;
}

HaveUploadDialog::addItem(QString name, QMovie* movie, int size, QString rmpt)
{
    VideoInfoItem* item = new VideoInfoItem;
    item->setInfo(name, movie, size, rmpt);
    m_pLayOut->addWidget(item);
    m_videoInfoItem.push_back(item);
}
//#include<QDebug>
//#include"videoinfoitem.h"
//void HaveUploadDialog::on_pushButton_clicked()
//{
//    qDebug() << "按下添加";
//    VideoInfoItem* item = new VideoInfoItem;
//    m_pLayOut->addWidget(item);
//}

