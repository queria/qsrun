#ifndef RUNBAR_H
#define RUNBAR_H

#include <QtGui/QLineEdit>
#include <QCompleter>
#include <QIcon>
#include <QMenu>
#include <QSystemTrayIcon>

#include "apphinter.h"
#include "calculator.h"
class RunBar : public QLineEdit
{
    Q_OBJECT

public:
    RunBar(QWidget *parent = 0);
    ~RunBar();

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

private:
    void _initActions();
    void _initTray();
    void _initConnections();
    void _setInputString(QString input, QString suggestion);
    void _methodNA();

private:
    QIcon _icon;
    QSystemTrayIcon *_tray;
    QMenu *_trayMenu;

    QAction *_toggleAction;
    QAction *_editHistoryAction;
    QAction *_reloadAction;

    QCompleter *_completer;
    AppHinter *_hinter;
    Calculator *_calculator;

    QString _lastInput;
};

#endif // RUNBAR_H
