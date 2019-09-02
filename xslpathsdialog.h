#ifndef XSLPATHSDIALOG_H
#define XSLPATHSDIALOG_H

#include <QDialog>

class QStringListModel;

namespace Ui {
class XslPathsDialog;
}

class XslPathsDialog : public QDialog
{
    Q_OBJECT

public:
    XslPathsDialog(QStringList paths, QWidget *parent = nullptr);
    ~XslPathsDialog();

    QStringList paths() const;

private:
    Ui::XslPathsDialog *ui;

    QStringListModel * mModel;

private slots:
    void add();
    void remove();
};

#endif // XSLPATHSDIALOG_H
