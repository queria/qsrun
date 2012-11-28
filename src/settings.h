#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QIcon>
#include <QEvent>
#include <QSettings>
#include <QLineEdit>
#include <QSpinBox>

class Settings : public QWidget
{
    Q_OBJECT

public:
    Settings(QIcon appIcon, QWidget *parent=0);
    ~Settings();
    virtual bool event(QEvent *e);
    QString editorPath();
    int refreshSeconds();

signals:
    void changed();

private slots:
    void apply();
    void accept();
    void reject();

private:
    void _initUI();
    void _load();
    bool _save();
    bool _validate();

    QSettings _settings;
    QLineEdit *_editorPath;
    QSpinBox *_refreshSeconds;
};
#endif // SETTINGS_H
