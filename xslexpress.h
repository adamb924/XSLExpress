#ifndef XSLEXPRESS_H
#define XSLEXPRESS_H

#include <QWidget>
#include <QList>
#include <QLineEdit>
#include "ui_xslexpress.h"

class QSettings;

class XSLExpress : public QWidget
{
    Q_OBJECT

public:
    XSLExpress(QWidget *parent = 0);
    ~XSLExpress();

private:
    Ui::XSLExpressClass *ui;
    QSettings *mSettings;

    void populateCombo();

    bool loadParametersFromXsl(bool withDefaults = false);

    QString mPath;

    QList<QLineEdit*> aParameterNames;
    QList<DropFilenameLineEdit*> aParameterValues;

    void removeParameterLineEdits();
    void setParameterBoxVisibility();

private slots:
    void autoProcess();
    void process();
    void saveCurrent();
    void deleteCurrent();
    void settingsChosen( const QString & text );
    void loadParameters();
    void loadParametersWithDefaults();
    void clearValues();
    void copyCall();
};

#endif // XSLEXPRESS_H
