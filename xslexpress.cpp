#include "xslexpress.h"

#include <QtDebug>
#include <QtWidgets>
#include <QProgressDialog>
#include <QHash>
#include <QErrorMessage>

#include "xsltproc.h"
#include "settingsnamedialog.h"
#include "xslpathsdialog.h"

XSLExpress::XSLExpress(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::XSLExpressClass)
{
    ui->setupUi(this);

    setStatus(XSLExpress::Ready);

    readSettings();

    setParameterBoxVisibility();

    populateCombo();

    ui->extensionWidget->hide();

    connect( ui->process, SIGNAL(clicked()), this, SLOT(process()) );
    connect( ui->inputFiles, SIGNAL(drop()), this, SLOT(autoProcess()) );
    connect( ui->saveCurrent, SIGNAL(clicked()), this, SLOT(saveCurrent()) );
    connect( ui->deleteCurrent, SIGNAL(clicked()), this, SLOT(deleteCurrent()) );
    connect( ui->savedSettings, SIGNAL(currentIndexChanged(QString)), this, SLOT(settingsChosen(QString)) );
    connect( ui->getParametersWithDefaults, SIGNAL(clicked()), this, SLOT(loadParametersWithDefaults()));
    connect( ui->clearValues, SIGNAL(clicked()), this, SLOT(clearValues()) );
    connect( ui->xslFile, SIGNAL(textChanged(QString)), this, SLOT(loadParametersWithDefaults()) );
    connect( ui->copyButton, SIGNAL(clicked(bool)), this, SLOT(copyCall()) );
    connect( ui->moreButton, SIGNAL(toggled(bool)), this, SLOT(showMoreOptions(bool)) );
    connect( ui->openXslFileButton, SIGNAL(clicked(bool)), this, SLOT(openXslFile()) );
    connect( ui->openXslFileFolderButton, SIGNAL(clicked(bool)), this, SLOT(openXslFileContainingFolder()) );
    connect( ui->openFirstInputFileButton, SIGNAL(clicked(bool)), this, SLOT(openFirstInputFile()));
    connect( ui->openFirstInputFileFolderButton, SIGNAL(clicked(bool)), this, SLOT(openFirstInputFileContainingFolder()));
    connect( ui->xslPathsButton, SIGNAL(clicked(bool)), this, SLOT(xslPaths() ));
}

XSLExpress::~XSLExpress()
{
}

void XSLExpress::autoProcess()
{
    if( ui->processImmediately->isChecked() )
        process();
}

void XSLExpress::process()
{


    QString xslFile = ui->xslFile->text();
    QString replaceThis = ui->replaceThis->text();
    QString replaceWith = ui->replaceWith->text();
    QStringList inputFiles = ui->inputFiles->toPlainText().split("\n", QString::SkipEmptyParts );

    if( xslFile.isEmpty() )
        return;
    if( inputFiles.isEmpty() )
        return;

    QString failures;

    setStatus(XSLExpress::Processing);

    // so for whatever reason this isn't working
    QProgressDialog progress( tr("Processing files..."), tr("Cancel"), 0, inputFiles.count(), nullptr);
    progress.setWindowModality(Qt::WindowModal);

    Xsltproc transform;
    QString lastXmlErrorMessage;

    transform.setXslPaths(mXslPaths);

    QErrorMessage *errDialog = new QErrorMessage(this);
    QFont font("Courier");
    font.setStyleHint(QFont::TypeWriter);
    errDialog->setFont(font);
    errDialog->resize(600,300);

    for( int i=0; i<inputFiles.count(); i++)
    {
        progress.setValue(i);
        if (progress.wasCanceled())
            break;

        QString outputFile = inputFiles.at(i);
        outputFile.replace(QRegExp(replaceThis), replaceWith);

        QDir::setCurrent( QFileInfo(outputFile).dir().absolutePath() );

        if( inputFiles.at(i) == outputFile && !ui->allowOverwrite->isChecked() )
        {
            failures += inputFiles.at(i) + tr(" (overwrite prevented)\n");
            continue;
        }

        QFileInfo info(inputFiles.at(i));
        QString errorFilename = QDir::temp().filePath( tr(".~err-%1!").arg( info.fileName() ) );
        QFileInfo errorFileInfo(errorFilename);

        QHash<QString,QString> parameters;
        for(int j=0; j<aParameterValues.count(); j++)
            if( !aParameterNames.at(j)->text().isEmpty() )
                parameters.insert( aParameterNames.at(j)->text() , aParameterValues.at(j)->text() );

        transform.setStyleSheet(xslFile);
        transform.setXmlFilename( inputFiles.at(i) );
        transform.setOutputFilename(outputFile);
        transform.setErrorFilename(errorFilename);
        transform.setParameters(parameters);
        Xsltproc::ReturnValue retval = transform.execute();

        QString stylesheetOutput;
        QFile data(errorFileInfo.absoluteFilePath());
        if (data.open(QFile::ReadOnly)) {
            QTextStream in(&data);
            stylesheetOutput = in.readAll();
            stylesheetOutput.replace("\n","<p>").replace(" ","&nbsp;");
        }

        switch(retval)
        {
        case Xsltproc::ApplyStylesheetFailure: // for an invalid stylesheet, show the message and exit
        case Xsltproc::InvalidStylesheet: // for an invalid stylesheet, show the message and exit
            progress.cancel();
            errDialog->setWindowTitle(tr("XSL Stylesheet Error"));
            errDialog->showMessage(stylesheetOutput);
            setStatus(XSLExpress::Ready);
            return;
        case Xsltproc::InvalidXmlFile: // for an individual failure, make a note and keep going
            lastXmlErrorMessage = stylesheetOutput;
            failures += inputFiles.at(i) + tr(" (invalid input file)\n");
            break;
        case Xsltproc::CouldNotOpenOutput:
            failures += inputFiles.at(i) + tr(" (could not open output file)\n");
            break;
        case Xsltproc::GenericFailure:
            failures += inputFiles.at(i) + tr(" (unknown error)\n");
            break;
        case Xsltproc::Success:
            if( !stylesheetOutput.isEmpty() )
            {
                errDialog->setWindowTitle(tr("XSL Stylesheet Output (xsl:message)"));
                errDialog->showMessage(stylesheetOutput);
            }
            break;
        }

        QFile::remove(errorFilename);
    }
    progress.setValue(inputFiles.count());

    if(!failures.isEmpty())
    {
        QMessageBox::information(this,tr("Error Report"),tr("These file(s) quit with an error:\n%1").arg(failures.trimmed()));
        errDialog->setWindowTitle(tr("Errors"));
        errDialog->showMessage(lastXmlErrorMessage);
    }
    setStatus(XSLExpress::Ready);
}

void XSLExpress::saveCurrent()
{
    SettingsNameDialog dlg( mSavedSettings.keys(), this );
    if( dlg.exec() )
    {
        QString name = dlg.name();

        if( mSavedSettings.contains(name) )
            if( QMessageBox::question (this, tr("XSLExpress"), tr("Do you wish to replace the existing settings with that name?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No ) == QMessageBox::No )
                return;

        Settings s;
        s.mReplaceThis = ui->replaceThis->text();
        s.mReplaceWith = ui->replaceWith->text();
        s.mXslFile = ui->xslFile->text();
        for(int i=0; i<aParameterValues.count(); i++)
        {
            s.mParameters[ aParameterNames.at(i)->text() ] = aParameterValues.at(i)->text();
        }

        mSavedSettings[name] = s;

        populateCombo();
        ui->savedSettings->setCurrentText(name);
    }
}

void XSLExpress::deleteCurrent()
{
    mSavedSettings.remove( ui->savedSettings->currentText() );
    populateCombo();
}

void XSLExpress::settingsChosen( const QString & text )
{
    if( text.isEmpty() || !mSavedSettings.contains(text) )
        return;

    Settings s = mSavedSettings.value(text);

    if( !QFile::exists( s.mXslFile ) )
    {
        if( QMessageBox::question(this,tr("XSLExpress"),tr("The XSL file specified in these settings cannot be found. Do you wish to delete these settings?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes )
            deleteCurrent();
        return;
    }

    ui->replaceThis->setText( s.mReplaceThis );
    ui->replaceWith->setText( s.mReplaceWith );
    ui->xslFile->setText( s.mXslFile );

    int i=0;
    QHashIterator<QString, QString> iter(s.mParameters);
    while (iter.hasNext()) {
        iter.next();
        aParameterNames.at(i)->setText( iter.key() );
        aParameterValues.at(i)->setText( iter.value() );
        i++;
    }

    if( aParameterValues.count() != s.mParameters.count() )
    {
        QMessageBox::information(this,tr("XSLExpress"),tr("The number of parameters in the XSL file and in the saved settings does not match. You should recheck the values to see if they are correct."));
    }
}

void XSLExpress::populateCombo()
{
    ui->savedSettings->clear();
    ui->savedSettings->addItem( "" );
    ui->savedSettings->addItems( mSavedSettings.keys() );

}

void XSLExpress::loadParametersWithDefaults()
{
    loadParametersFromXsl(true);
}

void XSLExpress::loadParameters()
{
    loadParametersFromXsl(false);
}

bool XSLExpress::loadParametersFromXsl(bool withDefaults)
{
    QString xslFile = ui->xslFile->text();
    if( xslFile.isEmpty() )
        return false;

    removeParameterLineEdits();

    // Find the parameters that the XSL file is requesting
    QFile file(xslFile);
    if(! file.open(QFile::ReadOnly | QFile::Text) ) { return false; }
    QXmlStreamReader xml(&file);
    int count = 0;
    while (!xml.atEnd())
    {
        if(xml.readNext() == QXmlStreamReader::StartElement)
        {
            if( xml.name().toString() == "param" && xml.namespaceUri() == "http://www.w3.org/1999/XSL/Transform" )
            {
                QString name = xml.attributes().value("name").toString();
                QString value = xml.readElementText();

                aParameterNames << new QLineEdit();
                aParameterNames.last()->setText(name);
                ui->parameterGridLayout->addWidget(aParameterNames.last(),count+1,0);

                aParameterValues << new DropFilenameLineEdit(DropFilenameLineEdit::UriEncode, this);
                if( withDefaults )
                    aParameterValues.last()->setText(value);
                ui->parameterGridLayout->addWidget(aParameterValues.last(),count+1,1);

                count++;
            }
            if( xml.name().toString() == "template" && xml.namespaceUri() == "http://www.w3.org/1999/XSL/Transform" ) // we've moved past the initial parameters
                break;
        }
    }
    file.close();

    setParameterBoxVisibility();

    return true;
}

void XSLExpress::removeParameterLineEdits()
{
    ui->parameterName->setVisible(false);
    ui->parameterValue->setVisible(false);

    qDeleteAll(aParameterNames);
    aParameterNames.clear();
    qDeleteAll(aParameterValues);
    aParameterValues.clear();
}

void XSLExpress::setParameterBoxVisibility()
{
    if( aParameterNames.count() > 0 )
        ui->parameterBox->setVisible(true);
    else
        ui->parameterBox->setVisible(false);
}

void XSLExpress::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    writeSettings();
}

void XSLExpress::readSettings()
{
    QSettings settings("AdamBaker","XSLExpress");
    restoreGeometry(settings.value("geometry").toByteArray());

    int size = settings.beginReadArray("savedSettings");
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);

        Settings s;
        s.mKey = settings.value("key").toString();
        s.mReplaceThis = settings.value("replaceThis").toString();
        s.mReplaceWith = settings.value("replaceWith").toString();
        s.mXslFile = settings.value("xslFile").toString();

        int pSize = settings.beginReadArray("parameters");
        for(int j=0; j<pSize; j++) {
            settings.setArrayIndex(j);
            s.mParameters[ settings.value("parameter").toString() ] = settings.value("value").toString();
        }
        settings.endArray();

        mSavedSettings[s.mKey] = s;
    }
    settings.endArray();

    size = settings.beginReadArray("xslPaths");
    for(int i=0; i<size; i++) {
        settings.setArrayIndex(i);
        mXslPaths.append( settings.value("path").toString() );
    }
    settings.endArray();
}

void XSLExpress::writeSettings()
{
    QSettings settings("AdamBaker","XSLExpress");
    settings.setValue("geometry", saveGeometry());

    settings.beginWriteArray("savedSettings");
    int i=0;
    QHashIterator<QString,Settings> iter(mSavedSettings);
    while (iter.hasNext()) {
        iter.next();

        settings.setArrayIndex(i);
        settings.setValue("key", iter.key() );
        settings.setValue("replaceThis", iter.value().mReplaceThis );
        settings.setValue("replaceWith", iter.value().mReplaceWith );
        settings.setValue("xslFile", iter.value().mXslFile );

        int j=0;
        QHashIterator<QString,QString> paramIter(iter.value().mParameters);
        settings.beginWriteArray("parameters");
        while (paramIter.hasNext()) {
            paramIter.next();
            settings.setArrayIndex(j);
            settings.setValue("parameter", paramIter.key() );
            settings.setValue("value", paramIter.value() );
            j++;
        }
        settings.endArray();

        i++;
    }
    settings.endArray();

    settings.beginWriteArray("xslPaths");
    for(int i=0; i<mXslPaths.count(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("path", mXslPaths.at(i));
    }
    settings.endArray();
}

void XSLExpress::setStatus(const XSLExpress::Status &status)
{
    switch(status) {
    case XSLExpress::Ready:
        ui->statusLabel->setText(tr("Ready"));
        break;
    case XSLExpress::Processing:
        ui->statusLabel->setText(tr("Processing..."));
        break;
    }
}

void XSLExpress::clearValues()
{
    for(int i=0; i<aParameterValues.count(); i++)
        aParameterValues.at(i)->setText("");
}

void XSLExpress::copyCall()
{
    QString clip = "";
    QString parameters = "";
    for(int i=0; i<aParameterNames.count(); i++)
    {
        parameters += "--param " + aParameterNames.at(i)->text() + " \"'" + aParameterValues.at(i)->text() + "'\" ";
    }

    QStringList inputFiles = ui->inputFiles->toPlainText().split("\n", QString::SkipEmptyParts );
    foreach(QString input, inputFiles)
    {
        QString output = input;
        output.replace(QRegExp(ui->replaceThis->text()), ui->replaceWith->text());
        clip += QString("xsltproc %1 -o \"%2\" \"%3\" \"%4\"\n").arg(parameters).arg(output).arg(ui->xslFile->text()).arg(input);
    }

    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText( clip );
}

void XSLExpress::xslPaths()
{
    XslPathsDialog dlg(mXslPaths, this);
    if( dlg.exec() == QDialog::Accepted ) {
        mXslPaths = dlg.paths();
    }
}

void XSLExpress::showMoreOptions(bool showMore)
{
    if( showMore )
    {
        ui->moreButton->setText(tr("Fewer options"));
        ui->extensionWidget->show();
    }
    else
    {
        ui->moreButton->setText(tr("More options"));
        ui->extensionWidget->hide();
    }
}

void XSLExpress::openXslFile()
{
    QString xslPath = ui->xslFile->text();
    if( !xslPath.isEmpty() )
    {
        QDesktopServices::openUrl( QUrl::fromLocalFile(xslPath) );
    }
}

void XSLExpress::openXslFileContainingFolder()
{
    QFileInfo xslInfo(ui->xslFile->text());
    if( xslInfo.exists() )
    {
        QDesktopServices::openUrl( QUrl::fromLocalFile( xslInfo.absoluteDir().absolutePath() ) );
    }
}

void XSLExpress::openFirstInputFile()
{
    QStringList inputFiles = ui->inputFiles->toPlainText().split("\n", QString::SkipEmptyParts );
    if( inputFiles.count() > 0 )
    {
        QDesktopServices::openUrl( QUrl::fromLocalFile( inputFiles.first() ) );
    }
}

void XSLExpress::openFirstInputFileContainingFolder()
{
    QStringList inputFiles = ui->inputFiles->toPlainText().split("\n", QString::SkipEmptyParts );
    if( inputFiles.count() > 0 )
    {
        QFileInfo xslInfo( inputFiles.first() );
        if( xslInfo.exists() )
        {
            QDesktopServices::openUrl( QUrl::fromLocalFile( xslInfo.absoluteDir().absolutePath() ) );
        }
    }
}
