#include <QtGui/QApplication>
#include "runbar.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RunBar w;
    w.show();

    return a.exec();
}
