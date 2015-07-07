#include "settingsnamedialog.h"
#include "ui_settingsnamedialog.h"

SettingsNameDialog::SettingsNameDialog(const QStringList &settings, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsNameDialog)
{
    ui->setupUi(this);
    ui->comboBox->addItem("");
    ui->comboBox->addItems(settings);
}

SettingsNameDialog::~SettingsNameDialog()
{
    delete ui;
}

QString SettingsNameDialog::name() const
{
    return ui->comboBox->currentText();
}
