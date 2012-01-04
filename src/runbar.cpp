#include "runbar.h"
#include <QAbstractItemView>
#include <QApplication>
#include <QProcess>
#include <QDir>

#include <QDebug>
#include <QMessageBox>

RunBar::RunBar(QWidget *parent):
    QLineEdit(parent),
    _icon(":/icons/qsrun.png")
{
    setMinimumSize(300,20);
    setWindowIcon(_icon);
    setWindowTitle(trUtf8("QSRun"));

    _hinter = new AppHinter();
    _completer = NULL;

    _initActions();
    _initTray();
    _initConnections();

    reload();

}

RunBar::~RunBar()
{

}

void RunBar::_methodNA()
{
    QMessageBox::information(this, trUtf8("QSRun - unfinished"), trUtf8("This methods is not implemented yet"));
}

void RunBar::toggle()
{
    setVisible( ! isVisible() );
}

void RunBar::editHistory()
{
    _methodNA();
}

void RunBar::reload()
{
    _hinter->reload();

    if(_completer) {
        _completer->disconnect(this);
        _completer->deleteLater();
    }

    _completer = new QCompleter(_hinter->availableCommands());
    _completer->setWidget(this);
    connect(_completer, SIGNAL(highlighted(QString)), this, SLOT(_completer_highlighted(QString)));
    this->clear();
}

void RunBar::_initActions()
{
    _toggleAction = new QAction(QIcon(":/icons/toggle.png"),trUtf8("Toggle"), this);
    _editHistoryAction = new QAction(QIcon(":/icons/history.png"), trUtf8("Edit history"), this);
    _reloadAction = new QAction(QIcon(":/icons/reload.png"), trUtf8("Reload App DB"), this);

    _editHistoryAction->setEnabled(false);
}

void RunBar::_initTray()
{
    _tray = new QSystemTrayIcon(this);
    _tray->setIcon(_icon);
    _tray->setToolTip("QSRun2");
    _trayMenu = new QMenu("QSRun");
    _trayMenu->addAction(_toggleAction);
    _trayMenu->addAction(_reloadAction);
    _trayMenu->addAction(_editHistoryAction);
    _trayMenu->addAction(QIcon(":/icons/quit.png"),"Quit", qApp, SLOT(quit()));
    _tray->setContextMenu(_trayMenu);
    _tray->show();
}

void RunBar::_initConnections()
{

    connect(this, SIGNAL(textEdited(QString)), this, SLOT(_typed(QString)));
    connect(this, SIGNAL(returnPressed()), this, SLOT(confirmed()));
    connect(_toggleAction, SIGNAL(triggered()), this, SLOT(toggle()));
    connect(_editHistoryAction, SIGNAL(triggered()), this, SLOT(editHistory()));
    connect(_reloadAction, SIGNAL(triggered()), this, SLOT(reload()));
    connect(_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(_toggleForTray(QSystemTrayIcon::ActivationReason)));
}

void RunBar::_typed(QString input, bool testLastInput)
{
    if(input[0] == '=') {
        _completer->popup()->hide();
        _calculator->calculate(input);
        return;
    }

    if(input.isEmpty()) {
        _completer->popup()->hide();
        _lastInput.clear();
        return;
    }

    QString last = _lastInput;
    _lastInput = input;

    if(testLastInput) {
        if(input == last) {
            return;
        }
        if(last.startsWith(input)) {
            // after backspace etc
            return;
        }
    }

    QString suggestion = _hinter->hint(input);
    qDebug() << "Hinter replied with:" << suggestion;
    if(suggestion.isEmpty()) { return; }
    _setInputString(input, suggestion);
    qDebug() << "Completion prefix:" << input;
    _completer->setCompletionPrefix(input);
    _completer->complete();
}

void RunBar::_completer_highlighted(QString selectedRow)
{
    _setInputString(_lastInput, selectedRow);
}

void RunBar::_setInputString(QString input, QString suggestion)
{
    setText(suggestion);
    setSelection(input.length(), suggestion.length() - input.length());
}

void RunBar::_toggleForTray(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::Context) {
        return;
    }
    toggle();
}

void RunBar::setVisible(bool visible)
{
    QLineEdit::setVisible(visible);
    if(visible) {
        qApp->setActiveWindow(this);
    } else {
        setText("");
    }
}

void RunBar::confirmed()
{
    QString appName = this->text();

    if(appName.isEmpty()) {
        return;
    }

    if(appName == "!reload") {
        _reloadAction->trigger();
        return;
    }
    if(appName == "!history") {
        _editHistoryAction->trigger();
        return;
    }

    QString appFullName = appName;
    QString path = _hinter->pathForApp(appName);
    if(!path.isEmpty()) {
        appFullName = QDir::toNativeSeparators(QDir(path).filePath(appName));
    }

    qDebug() << "Should run " << appFullName;
    if( ! QProcess::startDetached(appFullName, QStringList()) ) {
        return;
    }
    _hinter->addToHistory(appName);
    this->clear();
    // FIXME: until we support dbus/single-instance-app we have to quit after success
    qApp->quit();
}
