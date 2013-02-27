#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QObject>
#include <QLineEdit>
#include <QScriptEngine>

#include "googleconv.h"

class Calculator : public QObject
{
    Q_OBJECT
public:
    explicit Calculator(QLineEdit *inputField);
    void calculate(QString typeText);

signals:

public slots:

private:
    QLineEdit *_inputField;
    GoogleConv _convertor;
    QScriptEngine scriptEngine;
};

#endif // CALCULATOR_H
