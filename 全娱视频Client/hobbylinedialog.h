#ifndef HOBBYLINEDIALOG_H
#define HOBBYLINEDIALOG_H

#include <QWidget>
#include<QEvent>
#include<QEvent>
#include<QMenu>
#include<QAction>
#include<QMouseEvent>
#include<QVBoxLayout>
namespace Ui {
class HobbyLineDialog;
}

class HobbyLineDialog : public QWidget
{
    Q_OBJECT

public:
    explicit HobbyLineDialog(QWidget *parent = nullptr);

    ~HobbyLineDialog();
    bool eventFilter(QObject *watched, QEvent *event);

    QString GetText();
private slots:
    void slot_clickedMenu(QAction*);
signals:
    void SIG_addItem();
    void SIG_deleteItem(HobbyLineDialog*);
private:
    Ui::HobbyLineDialog *ui;
    QVBoxLayout* m_pLayOut;
    QMenu* m_menu;
};

#endif // HOBBYLINEDIALOG_H
