#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QObject>

class Calculator : public QObject
{
    Q_OBJECT
public:
    explicit Calculator(QObject *parent = 0);
    void calculate(QString typeText);

signals:

public slots:

};

#endif // CALCULATOR_H
