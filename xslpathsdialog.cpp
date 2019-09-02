#include "xslpathsdialog.h"
#include "ui_xslpathsdialog.h"

#include <QStringListModel>
#include <QInputDialog>

XslPathsDialog::XslPathsDialog(QStringList paths, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::XslPathsDialog)
{
    ui->setupUi(this);
    mModel = new QStringListModel(paths, this);
    ui->listView->setModel(mModel);

    connect( ui->addButton, SIGNAL(clicked(bool)), this, SLOT(add()) );
    connect( ui->removeButton, SIGNAL(clicked(bool)), this, SLOT(remove()) );
}

XslPathsDialog::~XslPathsDialog()
{
    delete ui;
}

QStringList XslPathsDialog::paths() const
{
    return mModel->stringList();
}

void XslPathsDialog::add()
{
    bool ok;
    QString newPath = QInputDialog::getText(this, tr("Add path"), tr("XSL Path (to a directory)"), QLineEdit::Normal, QString(), &ok);
    if(ok) {
        QStringList paths = mModel->stringList();
        paths.append(newPath);
        mModel->setStringList(paths);
    }
}

void XslPathsDialog::remove()
{
    QStringList paths = mModel->stringList();

    int row = ui->listView->currentIndex().row();
    if( row > -1 && row < paths.count() ) {
        paths.removeAt(row);
    }
    mModel->setStringList(paths);
}
