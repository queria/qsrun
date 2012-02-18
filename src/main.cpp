#include <QtGui/QApplication>
#include "runbar.h"
#include "dbussingleinstance.h"

#define EXIT_ALREADY_RUNNING 1

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DBusSingleInstance *singleInstance = new DBusSingleInstance();
    if( ! singleInstance->isFirst() ) {
        return EXIT_ALREADY_RUNNING;
    }

    RunBar w;
    singleInstance->markFirst(&w, SLOT(show()));

    w.show();

    return a.exec();
}
