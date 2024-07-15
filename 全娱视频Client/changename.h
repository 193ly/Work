#ifndef CHANGENAME_H
#define CHANGENAME_H

#include <QWidget>

namespace Ui {
class ChangeName;
}

class ChangeName : public QWidget
{
    Q_OBJECT

public:
    explicit ChangeName(QWidget *parent = nullptr);
    ~ChangeName();

private slots:
    void on_pb_submit_clicked();
signals:
    SIG_submit(QString);
private:
    Ui::ChangeName *ui;
};

#endif // CHANGENAME_H
