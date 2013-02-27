#include "calculator.h"

#include <QString>
#include <QStringList>
#include <QDebug>

Calculator::Calculator(QLineEdit *inputField) :
    QObject(inputField),
    _inputField(inputField)
{
}

void Calculator::calculate(QString typeText)
{
    int cursor = _inputField->cursorPosition();

    QString delimiter(" => ");
    QString expression;
    QString result;

    QStringList parts = typeText.split(delimiter);

    if(parts.length() > 1) {
        expression = parts[0].mid(1);
        if(_convertor.isPattern(expression)) {
            result = _convertor.convert(expression);
        } else {
            QScriptValue res = scriptEngine.evaluate(expression);
            if(res.isValid() && !res.isError() && !res.isUndefined() ) {
                result = res.toString();
            }
        }
    } else {
        expression = " ";
        cursor = 2;
        result = "Type in JavaScript expression to evaluate ...";
    }

    _inputField->setText("=" + expression + delimiter + result);
    _inputField->setCursorPosition(cursor);
}

