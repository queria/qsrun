#include "dbussingleinstance.h"

DBusSingleInstance::DBusSingleInstance()
{
}

bool DBusSingleInstance::isFirst()
{
    QDBusInterface *ifc = new QDBusInterface("net.satas.QSRun", "/");
    QDBusMessage msg = ifc->call("triggerSingleInstance");
    return ( msg.type() != QDBusMessage::ReplyMessage );
}

void DBusSingleInstance::markFirst(QObject *targetObj, const char *targetSlot)
{
    _adaptor = new DBusSingleInstanceAdaptor(targetObj);
    connect(_adaptor, SIGNAL(triggered()), targetObj, targetSlot);

    QDBusConnection con = QDBusConnection::sessionBus();
    con.registerService("net.satas.QSRun");
    con.registerObject("/", targetObj);
}


DBusSingleInstanceAdaptor::DBusSingleInstanceAdaptor(QObject *obj) :
    QDBusAbstractAdaptor(obj)
{
}

bool DBusSingleInstanceAdaptor::triggerSingleInstance()
{
    emit triggered();
    return true;
}

