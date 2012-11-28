#include "folderwatch.h"

#include <QDebug>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/inotify.h>
#define BUF_LEN 1024

FolderWatch::FolderWatch(QObject *parent)
    :QObject(parent),
    _available(false),
    _report_each(false)
{
    _inot = inotify_init();
    if(_inot != -1) {
        if( fcntl(_inot, F_SETFL, O_NONBLOCK) != -1 ) {
            _available = true;
        } // TODO: we should report ...
    } // report ...
}

bool FolderWatch::available() { return _available; }
bool FolderWatch::reportEach() { return _report_each; }
void FolderWatch::setReportEach(bool shouldReport) { _report_each = shouldReport; }

bool FolderWatch::observe(QString path)
{
    if( ! _available ) { return false; }
    if( _observed.contains(path) ) { return true; }

    int wd = inotify_add_watch(
            _inot,
            path.toLocal8Bit().constData(),
            IN_MODIFY | IN_ATTRIB | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE | IN_MOVE_SELF
            );
    if(wd != -1) {
        qDebug() << "path" << path << "is now observed";
        _observed.insert(path, wd);
        return true;
    }
    qDebug() << "unable to observe path" << path;
    return false;
}

bool FolderWatch::forget(QString path)
{
    if( ! _available ) { return false; }
    if( ! _observed.contains(path) ) { return false; }

    if( 0 == inotify_rm_watch(_inot,
                _observed.value(path)) ) {
        _observed.remove(path);
        qDebug() << "path" << path << "was forgotten";
        return true;
    }
    return false;
}

QStringList FolderWatch::observed()
{
    return QStringList( _observed.keys() );
}

/* // for debug purposes
QString mask2str(int mask)
{
    QStringList flags;
    if(mask & IN_MODIFY) { flags << "MODIFY"; }
    if(mask & IN_ATTRIB) { flags << "ATTRIB"; }
    if(mask & IN_CREATE) { flags << "CREATE"; }
    if(mask & IN_DELETE) { flags << "DELETE"; }
    if(mask & IN_DELETE_SELF) { flags << "DELETE_SELF"; }
    if(mask & IN_MOVE) { flags << "MOVE"; }
    if(mask & IN_MOVE_SELF) { flags << "MOVE_SELF"; }
    return flags.join(" ");
}
void eventDebug(struct inotify_event *event) {
    // print out event info
    qDebug() << "wd:" << event->wd
        << "mask:" << mask2str(event->mask)
        << "cookie:" << event->cookie
        << "len:" << event->len
        << "dir:" << ((event->mask & IN_ISDIR) ? "yes" : "no");

    if(event->len) {
        qDebug() << " filename =" << event->name;
    }

    qDebug() << " size of this message:" << (sizeof(struct inotify_event) + event->len);
}
*/

void FolderWatch::processEvents()
{
    if( ! _available ) { return; }

    char buf[BUF_LEN] __attribute__((aligned(4)));
    ssize_t len, i = 0;
    len = read(_inot, buf, BUF_LEN);

    if( len < 1) {
        qDebug("No events to process.");
        return;
    }

    QStringList changed;

    QString dir;
    while( i < len ) {
        struct inotify_event *event =
            (struct inotify_event *) &buf[i];

        if(_report_each) {
            dir = _observed.key(event->wd);
            if( !dir.isEmpty() && !changed.contains(dir) ) {
                changed << dir;
            }
        }

        // eventDebug(event); // uncomment two methods above

        i += sizeof(struct inotify_event) + event->len;
    }

    if(_report_each) {
        for(int idx=0; idx < changed.size(); ++idx) {
            emit folderChanged(changed[idx]);
        }
    }

    emit somethingChanged();
}

