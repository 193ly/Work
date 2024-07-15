#include"ckernel.h"
#include <QApplication>
#undef main
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //MainDialog w;
    //w.show();
    CKernel* ckernel = CKernel::GetInstance();
    return a.exec();
}
