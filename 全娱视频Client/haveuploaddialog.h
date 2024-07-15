#ifndef HAVEUPLOADDIALOG_H
#define HAVEUPLOADDIALOG_H

#include <QWidget>
#include<QVBoxLayout>
#include<vector>
#include"videoinfoitem.h"
using namespace std;
namespace Ui {
class HaveUploadDialog;
}

class HaveUploadDialog : public QWidget
{
    Q_OBJECT

public:
    explicit HaveUploadDialog(QWidget *parent = nullptr);
    ~HaveUploadDialog();

    addItem(QString name, QMovie* movie, int size, QString rmpt);
private slots:
    //void on_pushButton_clicked();

private:
    Ui::HaveUploadDialog *ui;
    QVBoxLayout* m_pLayOut;
    vector<VideoInfoItem*> m_videoInfoItem;
};

#endif // HAVEUPLOADDIALOG_H
