#include "settings.h"
#include <QKeyEvent>
#include <QDebug>

#include <QLabel>
#include <QPushButton>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QFileInfo>
#include <QMessageBox>

Settings::Settings(QIcon appIcon, QWidget *parent)
    :QWidget(parent)
{
    setWindowFlags(Qt::Dialog);
    setWindowIcon(appIcon);
    setWindowTitle(trUtf8("QSRun settings"));
    _initUI();
    _load();
}

Settings::~Settings() {}

bool Settings::event(QEvent *e)
{
    if(e->type() == QEvent::KeyPress) {
        if(((QKeyEvent*)e)->key() == Qt::Key_Escape) {
            hide();
            return true;
        }
    }
    return QWidget::event(e);
}

void Settings::_initUI()
{
    _editorPath = new QLineEdit(this);

    _refreshSeconds = new QSpinBox(this);
    _refreshSeconds->setRange(10, 600);
    _refreshSeconds->setSingleStep(10);
    _refreshSeconds->setSuffix(trUtf8("s"));
    
    QDialogButtonBox *buttons = new QDialogButtonBox(
            QDialogButtonBox::Ok
            | QDialogButtonBox::Apply
            | QDialogButtonBox::Cancel);
    connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
    connect(buttons->button(QDialogButtonBox::Apply), SIGNAL(clicked()),
            this, SLOT(apply()));

    QFormLayout *l = new QFormLayout();
    l->addRow(trUtf8("Editor path"), _editorPath);
    l->addRow(new QLabel(trUtf8("... Pro-tip: use only editors with GUI ;)")));
    l->addRow(trUtf8("AutoRefresh each"), _refreshSeconds);
    l->addRow(buttons);
    setLayout(l);
}

bool Settings::_validate()
{
    QStringList errors;
    if( ! QFileInfo( _editorPath->text() ).isExecutable() ) {
        errors << trUtf8("Invalid editor path!");
    }
   
    if( errors.size() ) {
        QMessageBox::warning(this,
                windowTitle(),
                errors.join("\n"));
        return false;
    }
    return true;
}

bool Settings::_save()
{
    if(! _validate() ) {
        return false;
    }
    _settings.setValue("path/editor", _editorPath->text());
    _settings.setValue("time/refresh", _refreshSeconds->value());
    emit changed();
    return true;
}

void Settings::_load()
{
    _editorPath->setText( editorPath() );
    _refreshSeconds->setValue( refreshSeconds() );
}

void Settings::apply() { _save(); }
void Settings::accept() { if(_save()) hide(); }
void Settings::reject() { _load(); hide(); }



// Individual getters for our options

QString Settings::editorPath()
{
    return _settings.value("path/editor", "/usr/bin/gvim").toString();
}

int Settings::refreshSeconds()
{
    return _settings.value("time/refresh", "20").toInt();
}

