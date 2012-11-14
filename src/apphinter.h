#ifndef APPHINTER_H
#define APPHINTER_H

#include <QObject>
#include <QStringList>
#include <QHash>

class AppHinter : public QObject
{
    Q_OBJECT
public:
    explicit AppHinter(QObject *parent = 0);
    QStringList availableCommands();
    QString hint(QString pattern);
    QString historyPath();
    QString pathForApp(QString appName);

signals:

public slots:
    void reload();
    bool addToHistory(QString executed);

private:
    void _loadApplications();
    void _loadHistory();
    QString _expandHome(QString path);
    QStringList _jsonToStringList(QString json);
    QString _stringListToJson(QStringList list, bool formatted=false);

private:
    QStringList _apps;
    QHash<QString,QString> _apps_with_path;
    QStringList _history;
    QString _historyPath;
};

#endif // APPHINTER_H
