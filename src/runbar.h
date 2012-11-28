#ifndef RUNBAR_H
#define RUNBAR_H

#include <QtGui/QLineEdit>
#include <QCompleter>
#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QTimer>

#include "apphinter.h"
#include "calculator.h"
#include "settings.h"

class RunBar : public QLineEdit
{
    Q_OBJECT

public:
    RunBar(QWidget *parent = 0);
    ~RunBar();
    virtual bool event(QEvent *e);
    virtual void closeEvent(QCloseEvent *e);

public slots:
    void toggle();
    void editHistory();
    void reload();
    virtual void setVisible(bool visible);
    void confirmed();

private slots:
    void _typed(QString input, bool testLastInput=true);
    void _toggleForTray(QSystemTrayIcon::ActivationReason reason);
    void _completer_highlighted(QString selectedRow);
    void _hinterChanged();
    void _settingsChanged();

private:
    void _initActions();
    void _initTray();
    void _initConnections();
    void _setInputString(QString input, QString suggestion);
    void _methodNA();
    void _renewCompleter();
    bool _launchApp(QString path, QStringList args);
    void _hideMe();

private:
    QIcon _icon;
    QSystemTrayIcon *_tray;
    QMenu *_trayMenu;
    Settings *_settings;

    QAction *_toggleAction;
    QAction *_editHistoryAction;
    QAction *_reloadAction;
    QAction *_showSettingsAction;

    QCompleter *_completer;
    AppHinter *_hinter;
    Calculator *_calculator;

    QTimer _autoRefresh;

    QString _lastInput;
    QStringList _emptyArgs;
};

#endif // RUNBAR_H
