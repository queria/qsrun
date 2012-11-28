#ifndef FOLDERWATCH_H
#define FOLDERWATCH_H

#include <QObject>
#include <QHash>
#include <QStringList>

class FolderWatch : public QObject
{
    Q_OBJECT
public:
    explicit FolderWatch(QObject *parent = 0);
    bool available();
    bool observe(QString path);
    bool forget(QString path);
    QStringList observed();
    bool reportEach(); // default is to NOT report each path
    void setReportEach(bool shouldReport);

signals:
    void folderChanged(QString path); // only when reportEach
    void somethingChanged(); // always (and after all individual folderChanged)

public slots:
    void processEvents();

private:
    QHash<QString,int> _observed;
    int _inot;
    bool _available;
    bool _report_each;

};

#endif // FOLDERWATCH_H
