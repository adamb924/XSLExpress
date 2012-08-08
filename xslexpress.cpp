#include "xslexpress.h"

#include <QProcess>
#include <QtDebug>
#include <QtGui>

XSLExpress::XSLExpress(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    mSettings = new QSettings("AdamBaker","XSLExpress");

    testForXsltproc();

    populateCombo();

    connect( ui.process, SIGNAL(clicked()), this, SLOT(process()) );
    connect( ui.inputFiles, SIGNAL(drop()), this, SLOT(autoProcess()) );
    connect( ui.saveCurrent, SIGNAL(clicked()), this, SLOT(saveCurrent()) );
    connect( ui.deleteCurrent, SIGNAL(clicked()), this, SLOT(deleteCurrent()) );
    connect( ui.savedSettings, SIGNAL(currentIndexChanged(QString)), this, SLOT(settingsChosen(QString)) );
    connect( ui.getParameters, SIGNAL(clicked()), this, SLOT(loadParameters()) );
    connect( ui.getParametersWithDefaults, SIGNAL(clicked()), this, SLOT(loadParametersWithDefaults()));
    connect( ui.clearValues, SIGNAL(clicked()), this, SLOT(clearValues()) );
}

XSLExpress::~XSLExpress()
{
    delete mSettings;
}

void XSLExpress::autoProcess()
{
    if( ui.processImmediately->isChecked() )
        process();
}

void XSLExpress::process()
{
    QString xslFile = ui.xslFile->text();
    QString replaceThis = ui.replaceThis->text();
    QString replaceWith = ui.replaceWith->text();
    QStringList inputFiles = ui.inputFiles->toPlainText().split("\n", QString::SkipEmptyParts );

    if( xslFile.isEmpty() )
        return;
    if( inputFiles.isEmpty() )
        return;

    QString failures;

    // so for whatever reason this isn't working
    QProgressDialog progress( tr("Processing files..."), tr("Cancel"), 0, inputFiles.count(), 0);
    progress.setWindowModality(Qt::WindowModal);

    for( int i=0; i<inputFiles.count(); i++)
    {
        progress.setValue(i);
        if (progress.wasCanceled())
            break;

        QString outputFile = inputFiles.at(i);
        outputFile.replace(QRegExp(replaceThis), replaceWith);

        if( inputFiles.at(i) == outputFile && !ui.allowOverwrite->isChecked() )
        {
            failures += inputFiles.at(i) + tr(" (overwrite prevented)\n");
            continue;
        }

        qDebug() << outputFile;

        QFileInfo info(inputFiles.at(i));
        QString errorFilename = info.absoluteDir().filePath( tr("Error %1.txt").arg(info.fileName()) );
        qDebug() << errorFilename;

        QProcess *myProcess = new QProcess(this);
        myProcess->setStandardErrorFile(errorFilename);
        QStringList arguments;

        arguments << "-o" << outputFile;

        if( !ui.paramName1->text().isEmpty() )
            arguments << "--param" << ui.paramName1->text() << "'" + ui.paramValue1->text() + "'";
        if( !ui.paramName2->text().isEmpty() )
            arguments << "--param" << ui.paramName2->text() << "'" + ui.paramValue2->text() + "'";
        if( !ui.paramName3->text().isEmpty() )
            arguments << "--param" << ui.paramName3->text() << "'" + ui.paramValue3->text() + "'";
        if( !ui.paramName4->text().isEmpty() )
            arguments << "--param" << ui.paramName4->text() << "'" + ui.paramValue4->text() + "'";
        if( !ui.paramName5->text().isEmpty() )
            arguments << "--param" << ui.paramName5->text() << "'" + ui.paramValue5->text() + "'";

        arguments << xslFile << inputFiles.at(i);

        if( mPath.isEmpty() )
            myProcess->start("xsltproc", arguments);
        else
            myProcess->start(mPath, arguments);

        // wait up to five minutes per file
        if( !myProcess->waitForFinished(300000) )
        {
            failures += inputFiles.at(i) + tr(" (timeout after 5 minutes)\n");
            delete myProcess;
            continue;
        }

        if( myProcess->exitCode() != 0 )
        {
            failures += inputFiles.at(i) + tr(" (see %1)\n").arg(errorFilename);
            delete myProcess;
            continue;
        }
        QFileInfo errorFileInfo(errorFilename);
        if( errorFileInfo.size() == 0 )
            QFile::remove(errorFilename);

        delete myProcess;
    }
    progress.setValue(inputFiles.count());

    if(!failures.isEmpty())
        QMessageBox::information(this,tr("Error Report"),tr("These files quit with an error:\n%1").arg(failures.trimmed()));
}

void XSLExpress::saveCurrent()
{
    QString name = QInputDialog::getText( this, tr("XSLExpress"), tr("Enter a name for this settings") );
    if( mSettings->contains(name) )
        if( QMessageBox::question (this, tr("XSLExpress"), tr("Do you wish to replace the existing settings with that name?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No ) == QMessageBox::No )
            return;

    QString settingsString;
    settingsString += ui.replaceThis->text() + "\n";
    settingsString += ui.replaceWith->text() + "\n";
    settingsString += ui.xslFile->text() + "\n";
    settingsString += ui.paramName1->text() + "\n";
    settingsString += ui.paramValue1->text() + "\n";
    settingsString += ui.paramName2->text() + "\n";
    settingsString += ui.paramValue2->text() + "\n";
    settingsString += ui.paramName3->text() + "\n";
    settingsString += ui.paramValue3->text() + "\n";
    settingsString += ui.paramName4->text() + "\n";
    settingsString += ui.paramValue4->text() + "\n";
    settingsString += ui.paramName5->text() + "\n";
    settingsString += ui.paramValue5->text();

    mSettings->setValue(name , settingsString );

    populateCombo();
}

void XSLExpress::deleteCurrent()
{
    mSettings->remove( ui.savedSettings->currentText() );
    populateCombo();
}

void XSLExpress::settingsChosen( const QString & text )
{
    if( text.isEmpty() || !mSettings->contains(text) )
        return;

    QStringList settings =  mSettings->value(text,"").toString().split("\n", QString::KeepEmptyParts );
    if( settings.count() != 13 )
        return;

    ui.replaceThis->setText( settings.at(0) );
    ui.replaceWith->setText( settings.at(1) );
    ui.xslFile->setText( settings.at(2) );
    ui.paramName1->setText( settings.at(3) );
    ui.paramValue1->setText( settings.at(4) );
    ui.paramName2->setText( settings.at(5) );
    ui.paramValue2->setText( settings.at(6) );
    ui.paramName3->setText( settings.at(7) );
    ui.paramValue3->setText( settings.at(8) );
    ui.paramName4->setText( settings.at(9) );
    ui.paramValue4->setText( settings.at(10) );
    ui.paramName5->setText( settings.at(11) );
    ui.paramValue5->setText( settings.at(12) );
}

void XSLExpress::populateCombo()
{
    ui.savedSettings->clear();
    ui.savedSettings->addItem( "" );
    ui.savedSettings->addItems( mSettings->allKeys() );
}

void XSLExpress::loadParametersWithDefaults()
{
    loadParametersFromXsl(true);
}

void XSLExpress::loadParameters()
{
    loadParametersFromXsl(false);
}

void XSLExpress::loadParametersFromXsl(bool withDefaults)
{
    QString xslFile = ui.xslFile->text();
    if( xslFile.isEmpty() )
        return;

    // Find the parameters that the XSL file is requesting
    QStringList parameters, values;
    QFile file(xslFile);
    if(! file.open(QFile::ReadOnly | QFile::Text) ) { return; }
    QXmlStreamReader xml(&file);
    while (!xml.atEnd())
    {
        if(xml.readNext() == QXmlStreamReader::StartElement)
        {
            if( xml.name().toString() == "param" && xml.namespaceUri() == "http://www.w3.org/1999/XSL/Transform" )
            {
                parameters << xml.attributes().value("name").toString();
                values << xml.readElementText();
            }
            if( xml.name().toString() == "template" && xml.namespaceUri() == "http://www.w3.org/1999/XSL/Transform" ) // we've moved past the initial parameters
                break;
        }
    }
    file.close();

    int len = parameters.length();
    if(len > 5)
        QMessageBox::information(this,tr("XSLExpress"),tr("The .xsl file is requesting %1 parameters, which is too many. The first five will be loaded.").arg(len));

    if( len > 0 )
        ui.paramName1->setText( parameters.at(0) );
    if( len > 1 )
        ui.paramName2->setText( parameters.at(1) );
    if( len > 2 )
        ui.paramName3->setText( parameters.at(2) );
    if( len > 3 )
        ui.paramName4->setText( parameters.at(3) );
    if( len > 4 )
        ui.paramName5->setText( parameters.at(4) );

    if( withDefaults )
    {
        if( len > 0 )
            ui.paramValue1->setText( values.at(0) );
        if( len > 1 )
            ui.paramValue2->setText( values.at(1) );
        if( len > 2 )
            ui.paramValue3->setText( values.at(2) );
        if( len > 3 )
            ui.paramValue4->setText( values.at(3) );
        if( len > 4 )
            ui.paramValue5->setText( values.at(4) );
    }

}

void XSLExpress::clearValues()
{
    ui.paramValue1->setText( "" );
    ui.paramValue2->setText( "" );
    ui.paramValue3->setText( "" );
    ui.paramValue4->setText( "" );
    ui.paramValue5->setText( "" );
}

void XSLExpress::testForXsltproc()
{
    QProcess *myProcess = new QProcess(this);
    myProcess->start("xsltproc");
    if( !myProcess->waitForFinished(300000) )
    {
        if( QMessageBox::Yes == QMessageBox::critical( this, tr("XSLExpress"), tr("XSLExpress couldn't find xsltproc because it is not in the path. Would you like to try to locate it yourself? (If you click no the program will close.)"), QMessageBox::Yes | QMessageBox::No ) )
        {
            mPath = QFileDialog::getOpenFileName( this, tr("Please locate xsltproc") );
            if(mPath.isEmpty())
                QTimer::singleShot(0, this, SLOT(close()));
        }
        else
        {
            QTimer::singleShot(0, this, SLOT(close()));
        }
    }
}
