#include "hobbylinedialog.h"
#include "ui_hobbylinedialog.h"
#include<QDebug>
HobbyLineDialog::HobbyLineDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HobbyLineDialog)
{
    ui->setupUi(this);




    ui->lineEdit->installEventFilter(this);
    m_menu = new QMenu;
    m_menu->addAction("delete");

    connect(m_menu,SIGNAL(triggered(QAction*)),this,SLOT(slot_clickedMenu(QAction*)));
}

HobbyLineDialog::~HobbyLineDialog()
{
    delete ui;
}
#include<QKeyEvent>
bool HobbyLineDialog::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->lineEdit && event->type() == QEvent::MouseButtonPress){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::RightButton) {
            QPoint pos = QCursor::pos();

            QSize size = m_menu->sizeHint();
            m_menu->exec(QPoint(pos.x(),  pos.y() - size.height()));
            qDebug() << "right press";
        }
//        else if(mouseEvent->button() == Qt::LeftButton)
//        {
//            ui->lineEdit->setFocusPolicy(Qt::StrongFocus);
//            qDebug() << "设置焦点";
//            if (ui->lineEdit->hasFocus()) {
//                qDebug() << "has focus";
//            } else {
//                ui->lineEdit->setFocus(); // 设置焦点到按钮
//            }
//        }
    }
    else if(watched == ui->lineEdit && event->type() == QEvent::KeyPress)
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if(keyEvent->key() == Qt::Key_Enter)
        {
            Q_EMIT SIG_addItem();
            qDebug() << "ender press";
        }
    }
    //event->accept();
    return QWidget::eventFilter(watched, event);
}

QString HobbyLineDialog::GetText()
{
    return ui->lineEdit->text();
}

void HobbyLineDialog::slot_clickedMenu(QAction *action)
{
    if(action->text() == "delete")
    {
        qDebug() << "点击到了删除命令" ;
        Q_EMIT SIG_deleteItem(this);
        //delete this;
    }
}
