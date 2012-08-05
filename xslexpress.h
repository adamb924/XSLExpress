#ifndef XSLEXPRESS_H
#define XSLEXPRESS_H

#include <QtGui/QWidget>
#include "ui_xslexpress.h"

class QSettings;

class XSLExpress : public QWidget
{
    Q_OBJECT

public:
    XSLExpress(QWidget *parent = 0);
    ~XSLExpress();

private:
    Ui::XSLExpressClass ui;
    QSettings *mSettings;

    void populateCombo();

    void loadParametersFromXsl(bool withDefaults = false);
    void testForXsltproc();

    QString mPath;

private slots:
    void autoProcess();
    void process();
    void saveCurrent();
    void deleteCurrent();
    void settingsChosen( const QString & text );
    void loadParameters();
    void loadParametersWithDefaults();
    void clearValues();
};

#endif // XSLEXPRESS_H
