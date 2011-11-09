#include "runbar.h"
#include <QDebug>

RunBar::RunBar(QWidget *parent)
    : QLineEdit(parent)
{
    _hinter = new AppHinter();

    this->setCompleter(new QCompleter(_hinter->availableCommands(), this));
    connect(this, SIGNAL(textEdited(QString)), this, SLOT(_typed(QString)));
}

RunBar::~RunBar()
{

}

void RunBar::_typed(QString text)
{
    qDebug() << _hinter->hint(text);
}
