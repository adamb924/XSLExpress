/*
 * DropFilenameLineEdit.cpp
 *
 *  Created on: Aug 1, 2012
 *      Author: Adam
 */

#include <QtGui>

#include "DropFilenameLineEdit.h"

DropFilenameLineEdit::DropFilenameLineEdit(QWidget *parent) : QLineEdit(parent), mType(NoEncode)
{
    setAcceptDrops(true);
}

DropFilenameLineEdit::DropFilenameLineEdit( DropType type, QWidget * parent ) : QLineEdit(parent), mType(type)
{
    setAcceptDrops(true);
}

void DropFilenameLineEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void DropFilenameLineEdit::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        if( mType == DropFilenameLineEdit::UriEncode )
        {
            this->setText(event->mimeData()->urls().at(0).toString(QUrl::FullyEncoded));
        }
        else
        {
            this->setText(event->mimeData()->urls().at(0).toLocalFile());
        }
    }
}
