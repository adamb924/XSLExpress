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

    setParameterBoxVisibility();

    populateCombo();

    connect( ui.process, SIGNAL(clicked()), this, SLOT(process()) );
    connect( ui.inputFiles, SIGNAL(drop()), this, SLOT(autoProcess()) );
    connect( ui.saveCurrent, SIGNAL(clicked()), this, SLOT(saveCurrent()) );
    connect( ui.deleteCurrent, SIGNAL(clicked()), this, SLOT(deleteCurrent()) );
    connect( ui.savedSettings, SIGNAL(currentIndexChanged(QString)), this, SLOT(settingsChosen(QString)) );
    connect( ui.getParametersWithDefaults, SIGNAL(clicked()), this, SLOT(loadParametersWithDefaults()));
    connect( ui.clearValues, SIGNAL(clicked()), this, SLOT(clearValues()) );
    connect( ui.xslFile, SIGNAL(textChanged(QString)), this, SLOT(loadParametersWithDefaults()) );
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

        QFileInfo info(inputFiles.at(i));
        QString errorFilename = info.absoluteDir().filePath( tr("Error %1.txt").arg(info.fileName()) );

        QProcess *myProcess = new QProcess(this);
        myProcess->setStandardErrorFile(errorFilename);
        QStringList arguments;

        arguments << "-o" << outputFile;

        for(int j=0; j<aParameterValues.count(); j++)
        {
            if( !aParameterNames.at(j)->text().isEmpty() && !aParameterValues.at(j)->text().isEmpty() )
            {
                arguments << "--param" << aParameterNames.at(j)->text() << "'" + aParameterValues.at(j)->text() + "'";
            }
        }

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
    for(int i=0; i<aParameterValues.count(); i++)
    {
        settingsString += aParameterNames.at(i)->text() + "\n";
        settingsString += aParameterValues.at(i)->text() + "\n";
    }

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
    if( settings.count() < 3 )
        return;

    QString xslFilename = settings.at(2);
    if( !QFile::exists(xslFilename) )
    {
        if( QMessageBox::question(this,tr("XSLExpress"),tr("The XSL file specified in these settings cannot be found. Do you wish to delete these settings?"), QMessageBox::Yes|QMessageBox::No, QMessageBox::No) == QMessageBox::Yes )
            deleteCurrent();
        return;
    }

    ui.replaceThis->setText( settings.at(0) );
    ui.replaceWith->setText( settings.at(1) );
    ui.xslFile->setText( xslFilename );

    for(int i=0; i < qMin(aParameterValues.count(),(settings.count()-3)/2); i++)
    {
        aParameterNames.at(i)->setText( settings.at(3 + 2*i) );
        aParameterValues.at(i)->setText( settings.at(3 + 2*i + 1) );
    }

    if( aParameterValues.count() != (settings.count()-3)/2 )
    {
        QMessageBox::information(this,tr("XSLExpress"),tr("The number of parameters in the XSL file and in the saved settings does not match. You should recheck the values to see if they are correct."));
    }
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

bool XSLExpress::loadParametersFromXsl(bool withDefaults)
{    
    QString xslFile = ui.xslFile->text();
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
                ui.parameterGridLayout->addWidget(aParameterNames.last(),count+1,0);

                aParameterValues << new DropFilenameLineEdit(this);
                if( withDefaults )
                    aParameterValues.last()->setText(value);
                ui.parameterGridLayout->addWidget(aParameterValues.last(),count+1,1);

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
    ui.parameterName->setVisible(false);
    ui.parameterValue->setVisible(false);

    qDeleteAll(aParameterNames);
    aParameterNames.clear();
    qDeleteAll(aParameterValues);
    aParameterValues.clear();
}

void XSLExpress::setParameterBoxVisibility()
{
    if( aParameterNames.count() > 0 )
        ui.parameterBox->setVisible(true);
    else
        ui.parameterBox->setVisible(false);
}

void XSLExpress::clearValues()
{
    for(int i=0; i<aParameterValues.count(); i++)
        aParameterValues.at(i)->setText("");
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
