#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include<QCloseEvent>
QT_BEGIN_NAMESPACE
namespace Ui { class MainDialog; }
QT_END_NAMESPACE

class MainDialog : public QDialog
{
    Q_OBJECT
signals:
    SIG_close();
public:
    MainDialog(QWidget *parent = nullptr);
    ~MainDialog();
    void closeEvent(QCloseEvent *);
private:
    Ui::MainDialog *ui;
};
#endif // MAINDIALOG_H
