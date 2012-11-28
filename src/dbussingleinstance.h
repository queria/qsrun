#ifndef DBUSSINGLEAPP_H
#define DBUSSINGLEAPP_H



#include <QObject>
#include <QDebug>
#include <QtDBus>

class DBusSingleInstanceAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "net.satas.QSRun")
public:
    DBusSingleInstanceAdaptor(QObject *obj);
public slots:
    bool triggerSingleInstance();
signals:
    bool triggered();
};

class DBusSingleInstance : public QObject
{
    Q_OBJECT
public:
    explicit DBusSingleInstance();
    bool isFirst();
    void markFirst(QObject *targetObj, const char *targetSlot);
private:
    DBusSingleInstanceAdaptor *_adaptor;
};

#endif // DBUSSINGLEAPP_H
