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

    reload();
    if(!_apps.contains("!reload")) {
        addToHistory("!reload");
    }
    if(!_apps.contains("!history")) {
        addToHistory("!history");
    }
}

QStringList AppHinter::availableCommands()
{
    QStringList all(_apps);
    all << _history;
    all.sort();
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

QString AppHinter::historyPath()
{
    return _historyPath;
}

void AppHinter::reload()
{
    _loadApplications();
    _loadHistory();
}

void AppHinter::addToHistory(QString executed)
{
    if(_history.contains(executed)) {
        return;
    }
    _history << executed;
    _history.sort();
    QFile historyFile(historyPath());
    if(!historyFile.open(QFile::WriteOnly)) {
        return;
    }
    historyFile.write(_stringListToJson(_history, true).toUtf8());
    historyFile.close();
}

void AppHinter::_loadApplications()
{
    _apps.clear();

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QStringList appDirectories = env.value("PATH", "").split(":");
    if(appDirectories.empty()) {
        return;
    }
    for(int idx=0; idx < appDirectories.length(); ++idx) {
        _apps << QDir(appDirectories[idx]).entryList(QDir::Files | QDir::Executable);
    }
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
