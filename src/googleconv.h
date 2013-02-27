#ifndef GOOGLECONV_H
#define GOOGLECONV_H

#include <QObject>
#include <QRegExp>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QScriptEngine>
#include <QEventLoop>

class GoogleConv : public QObject
{
    Q_OBJECT
public:
    explicit GoogleConv();
    bool isPattern(QString expression);
    QString convert(QString expression);
    QString convert(QString value, QString from, QString to);

signals:

public slots:

private:
    QRegExp _pattern;
    QRegExp _responsePattern;
    QNetworkAccessManager _qnam;
    //QScriptEngine scriptEngine;

    //QString _convert(QString value, QString from, QString to);
};

#endif // GOOGLECONV_H
