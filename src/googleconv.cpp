#include "googleconv.h"

GoogleConv::GoogleConv() :
    _pattern("\\W*([0-9\\.]+)\\W*([a-zA-Z]{3})\\W*in\\W*([a-zA-Z]{3})\\W*"),
    _responsePattern("rhs:\\W*\"([0-9\\.]+) ([0-9A-Za-z\\.\\ ]+)\"")
{
}

bool GoogleConv::isPattern(QString expression)
{
    return _pattern.exactMatch(expression);
}

QString GoogleConv::convert(QString expression)
{
    if( ! _pattern.exactMatch(expression) ) {
        return trUtf8("-invalid input-");
    }

    return convert(
            _pattern.cap(1),
            _pattern.cap(2),
            _pattern.cap(3));
}

QString GoogleConv::convert(QString value, QString from, QString to)
{
    // Convert value using Google's JSON API for Finance Convertor
    // see: http://www.google.com/finance/converter
    //
    QEventLoop loop;

    QNetworkReply *reply = _qnam.get(
            QNetworkRequest(
                QUrl(
                    QString("http://www.google.com/ig/calculator?hl=en&q=%1%2=?%3") \
                    .arg(value).arg(from).arg(to)
                    )));
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    if(reply->error() != QNetworkReply::NoError) {
        return reply->errorString();
    }

    QString response = reply->readAll();

    if( _responsePattern.indexIn(response) != -1 ) {
        QString converted = _responsePattern.cap(1);
        QString currency = _responsePattern.cap(2);
        return QString("%1 %2").arg( converted.toDouble(), 0, 'f', 2 ).arg(currency);
    }

    //return response; // to debug errors
    return "";
}
