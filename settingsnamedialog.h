#ifndef SETTINGSNAMEDIALOG_H
#define SETTINGSNAMEDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsNameDialog;
}

class SettingsNameDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsNameDialog(const QStringList & settings, QWidget *parent = 0);
    ~SettingsNameDialog();

    QString name() const;

private:
    Ui::SettingsNameDialog *ui;
};

#endif // SETTINGSNAMEDIALOG_H
