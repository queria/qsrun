#include "runbar.h"
#include <QAbstractItemView>
#include <QApplication>
#include <QProcess>
#include <QDir>
#include <QKeyEvent>

#include <QDebug>
#include <QMessageBox>

RunBar::RunBar(QWidget *parent):
    QLineEdit(parent),
    _icon(":/icons/qsrun.png")
{
    setMinimumSize(300,20);
    setWindowIcon(_icon);
    setWindowTitle(trUtf8("QSRun"));

    _settings = new Settings(_icon, this);

    _hinter = new AppHinter();
    _completer = NULL;

    _calculator = new Calculator(this);

    _initActions();
    _initTray();
    _initConnections();

    reload();
    _settingsChanged();
    _autoRefresh.start();
}

RunBar::~RunBar()
{

}

bool RunBar::event(QEvent *e)
{
    if(e->type() == QEvent::KeyPress) {
        if(((QKeyEvent*)e)->key() == Qt::Key_Escape) {
            _hideMe();
            return true;
        }
    }
    return QLineEdit::event(e);
}

void RunBar::closeEvent(QCloseEvent *e)
{
    // we have to quit when window is closed for ourself
    // because we have quitOnLastWindowClose=false (headless most of the time)
    qApp->quit();
    e->accept();
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
    if( _launchApp(
            _settings->editorPath(),
            QStringList(_hinter->historyPath())) ) {
        qApp->quit();
    }
    QMessageBox::warning(this,
            trUtf8("QSRun - sorry"),
            trUtf8("Unable to launch specified editor:\n %1\n\nYou can change the path in settings.").arg(
                _settings->editorPath()));
}

void RunBar::reload()
{
    _hinter->reload();
}

void RunBar::_hinterChanged()
{
    this->_renewCompleter();
    _lastInput.clear();
    this->clear();
}

void RunBar::_renewCompleter()
{
    if(_completer) {
        _completer->disconnect(this);
        _completer->deleteLater();
    }

    _completer = new QCompleter(_hinter->availableCommands());
    _completer->setWidget(this);
    connect(_completer, SIGNAL(highlighted(QString)), this, SLOT(_completer_highlighted(QString)));
}

void RunBar::_initActions()
{
    _toggleAction = new QAction(QIcon(":/icons/toggle.png"),trUtf8("Toggle"), this);
    _editHistoryAction = new QAction(QIcon(":/icons/history.png"), trUtf8("Edit history"), this);
    _reloadAction = new QAction(QIcon(":/icons/reload.png"), trUtf8("Reload App DB"), this);
    _showSettingsAction = new QAction(QIcon(":/icons/settings.png"), trUtf8("Settings..."), this);
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
    _trayMenu->addAction(_showSettingsAction);
    _trayMenu->addAction(QIcon(":/icons/quit.png"),"Quit", qApp, SLOT(quit()));
    _tray->setContextMenu(_trayMenu);
    _tray->show();
}

void RunBar::_initConnections()
{
    connect(this, SIGNAL(textEdited(QString)), this, SLOT(_typed(QString)));
    connect(this, SIGNAL(returnPressed()), this, SLOT(confirmed()));
    connect(_hinter, SIGNAL(changed()), this, SLOT(_hinterChanged()));
    connect(_settings, SIGNAL(changed()), this, SLOT(_settingsChanged()));
    connect(_toggleAction, SIGNAL(triggered()), this, SLOT(toggle()));
    connect(_editHistoryAction, SIGNAL(triggered()), this, SLOT(editHistory()));
    connect(_reloadAction, SIGNAL(triggered()), this, SLOT(reload()));
    connect(_showSettingsAction, SIGNAL(triggered()), _settings, SLOT(show()));
    connect(_tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(_toggleForTray(QSystemTrayIcon::ActivationReason)));
    connect(&_autoRefresh, SIGNAL(timeout()), _hinter, SLOT(reloadIfNeeded()));
}

void RunBar::_typed(QString input, bool testLastInput)
{
    if(input[0] == '=') {
        _completer->popup()->hide();
        _keepFocus();
        _calculator->calculate(input);
        return;
    }

    if(input.isEmpty()) {
        _completer->popup()->hide();
        _lastInput.clear();
        _keepFocus();
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
    if(_completer->completionCount() < 2) {
        _completer->popup()->hide();
        _keepFocus();
    }
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

    if(appName == "!refresh") {
        clear();
        _hinter->reloadIfNeeded();
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

    if(appName == "!settings") {
        _showSettingsAction->trigger();
        return;
    }

    QString appFullName = appName;
    QString path = _hinter->pathForApp(appName);
    if(!path.isEmpty()) {
        appFullName = QDir::toNativeSeparators(QDir(path).filePath(appName));
    }

    if( ! _launchApp(appFullName, _emptyArgs) ) {
        return;
    }
    if(_hinter->addToHistory(appName)) {
        this->_renewCompleter();
    }
    _hideMe();
}

void RunBar::_hideMe()
{
    _completer->popup()->hide();
    hide();

    _lastInput.clear();
    clear();
}

void RunBar::_keepFocus()
{
    this->activateWindow();
}

bool RunBar::_launchApp(QString path, QStringList args)
{
    qDebug() << "Should run " << path;
    return QProcess::startDetached(path, args);
}

void RunBar::_settingsChanged()
{
    _autoRefresh.setInterval( _settings->refreshSeconds() * 1000 );
    qDebug() << "AutoRefresh interval is (msec):" << _autoRefresh.interval();
}

