#ifndef XSLEXPRESS_H
#define XSLEXPRESS_H

#include <QWidget>
#include <QList>
#include <QLineEdit>

#include "ui_xslexpress.h"
#include "settings.h"

class QSettings;

class XSLExpress : public QWidget
{
    Q_OBJECT

public:
    XSLExpress(QWidget *parent = nullptr);
    ~XSLExpress();

private:
    Ui::XSLExpressClass *ui;

    void populateCombo();

    bool loadParametersFromXsl(bool withDefaults = false);

    QString mPath;

    QList<QLineEdit*> aParameterNames;
    QList<DropFilenameLineEdit*> aParameterValues;

    QHash<QString,Settings> mSavedSettings;

    QStringList mXslPaths;

    void removeParameterLineEdits();
    void setParameterBoxVisibility();

    void closeEvent(QCloseEvent *event);
    void readSettings();
    void writeSettings();

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
    void xslPaths();

    void showMoreOptions(bool showMore);

    void openXslFile();
    void openXslFileContainingFolder();
    void openFirstInputFile();
    void openFirstInputFileContainingFolder();
};

#endif // XSLEXPRESS_H
