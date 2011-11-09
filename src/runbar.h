#ifndef RUNBAR_H
#define RUNBAR_H

#include <QtGui/QLineEdit>
#include <QCompleter>

#include "apphinter.h"

class RunBar : public QLineEdit
{
    Q_OBJECT

public:
    RunBar(QWidget *parent = 0);
    ~RunBar();

private slots:
    void _typed(QString text);

private:
    AppHinter *_hinter;
};

#endif // RUNBAR_H
