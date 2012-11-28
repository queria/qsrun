#include "apphinter.h"
#include <QDir>
#include <QFile>
#include <QProcessEnvironment>

#include <QDebug>

AppHinter::AppHinter(QObject *parent) :
    QObject(parent)
{
    QDir home = QDir::home();
    home.mkpath(".cache/qsrun/");
    _historyPath = home.filePath(".cache/qsrun/historyQt.json");

    _watch = new FolderWatch(this);
    connect(_watch, SIGNAL(somethingChanged()), this, SLOT(reload()));
}

QStringList AppHinter::availableCommands()
{
    QStringList all(_history);
    all << _apps;
    all.sort();
    all.removeDuplicates();
    return all;
}

QString AppHinter::hint(QString pattern)
{
    QRegExp rx("^" + QRegExp::escape(pattern) + ".*");
    int matching = _history.indexOf(rx);
    if(matching >= 0) {
        return _history[matching];
    }
    matching = _apps.indexOf(rx);
    if(matching >= 0) {
        return _apps[matching];
    }
    return "";
}

QString AppHinter::pathForApp(QString appName)
{
    return _apps_with_path.value(appName);
}

QString AppHinter::historyPath()
{
    return _historyPath;
}

void AppHinter::reload()
{
    _loadApplications();
    _loadHistory();
    emit changed();
}

bool AppHinter::addToHistory(QString executed)
{
    if(_history.contains(executed)) {
        return false;
    }
    _history << executed;
    _history.sort();
    QFile historyFile(historyPath());
    if(!historyFile.open(QFile::WriteOnly)) {
        qWarning() << "unable to write into History File" << historyPath();
        return false;
    }
    historyFile.write(_stringListToJson(_history, true).toUtf8());
    historyFile.close();
    qDebug() << "command " << executed << "added to History";
    return true;
}

QStringList AppHinter::envPath()
{
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QStringList appDirectories = env.value("PATH", "").split(":");

    for(int idx=0; idx < appDirectories.length(); ++idx) {
        QString path = _expandHome(appDirectories[idx]);
        if(path.endsWith('/')) {
            path = path.left(path.size()-1);
        }
        appDirectories[idx] = path;
    }
    return appDirectories;
}

void AppHinter::_loadApplications()
{
    _apps.clear();
    _apps_with_path.clear();

    QStringList appDirectories = envPath();
    QStringList alreadyDone;
    if(appDirectories.empty()) {
        qDebug() << "no directories to walk through";
        return;
    }
    for(int idx=0; idx < appDirectories.length(); ++idx) {
        QString path = appDirectories[idx];
        if(alreadyDone.contains(path)) {
            qDebug() << "directory" << path << "already walked before";
            continue;
        }
        qDebug() << "walking through directory" << path;
        alreadyDone << path;
        QStringList apps = QDir(path).entryList(
                    QDir::Files | QDir::Executable
                    );
        for(int appIdx=0; appIdx < apps.length(); ++appIdx) {
            if( !_apps_with_path.contains(apps[appIdx]) ) {
                _apps_with_path.insert(apps[appIdx], path);
                _apps << apps[appIdx];
            } // else: it was already present in some dir at beggining of PATH
        }
        _watch->observe(path);
    }
    _apps.sort();
}

void AppHinter::reloadIfNeeded()
{
    _watch->processEvents();
}

QString AppHinter::_expandHome(QString path)
{
    if(path[0] == '~' && path[1] == '/') {
        return QDir::home().filePath( path.mid(2) );
    }

    return path;
}

void AppHinter::_loadHistory()
{
    _history.clear();

    QFile historyFile(historyPath());
    if( ! historyFile.open(QFile::ReadOnly) ) {
        return;
    }
    _history = _jsonToStringList(historyFile.readAll());
    historyFile.close();

    addToHistory("!refresh");
    addToHistory("!reload");
    addToHistory("!history");
    addToHistory("!settings");
    _history.sort();
}

QStringList AppHinter::_jsonToStringList(QString json)
{
    QString buffer;
    QStringList list;

    json = json.remove("\n");
    json = json.remove("\r");

    bool started = false;
    bool in_string = false;

    int length = json.length();
    for(int idx=0; idx < length; ++idx) {
        if(!started) {
            if(json[idx] == '[') {
                started = true;
            }
            continue;
        }
        if(!in_string) {
            if(json[idx] == '"') {
                in_string = true;
            } else if( json[idx] == ']' ) {
                started = false;
                break; // FINISH
            }
            continue;
        }
        if(json[idx] == '"') {
            if(!buffer.isEmpty()) {
                if(buffer[buffer.length() -1] == '\\') {
                    buffer[buffer.length() - 1] = '"'; // replace escaped doublequote
                    continue;
                }
                list << buffer;
                buffer.clear();
            } // here we skip empty strings (useless in qsrun)
            in_string = false;
            continue;
        }
        buffer = buffer.append(json[idx]);
    }
    if(started) {
        qDebug() << "ERROR: we missed end of json!";
    }
    if(in_string) {
        qDebug() << "ERROR: we missed end of string!";
    }
    return list;
}

QString AppHinter::_stringListToJson(QStringList list, bool formatted)
{
    if(list.empty()) { return "[]"; }

    QString newLine = formatted ? "\n" : " ";
    QString sepNewLine = "\"," + newLine + "\"";

    return "[" + newLine + "\"" + list.join(sepNewLine) + "\"" + newLine + "]";
}
