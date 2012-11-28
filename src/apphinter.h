#ifndef APPHINTER_H
#define APPHINTER_H

#include <QObject>
#include <QStringList>
#include <QHash>
#include "folderwatch.h"

class AppHinter : public QObject
{
    Q_OBJECT
public:
    explicit AppHinter(QObject *parent = 0);
    QStringList availableCommands();
    QString hint(QString pattern);
    QString historyPath();
    QString pathForApp(QString appName);
    QStringList envPath();

signals:
    void changed();

public slots:
    void reload();
    void reloadIfNeeded();
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
    FolderWatch *_watch;
};

#endif // APPHINTER_H
