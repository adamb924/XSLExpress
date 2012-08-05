/*
 * DropFilenameLineEdit.cpp
 *
 *  Created on: Aug 1, 2012
 *      Author: Adam
 */

#include <QtGui>

#include "DropFilenameLineEdit.h"

DropFilenameLineEdit::DropFilenameLineEdit( QWidget * parent ) : QLineEdit(parent)
{
    setAcceptDrops(true);
}

DropFilenameLineEdit::DropFilenameLineEdit()
{
    setAcceptDrops(true);
}

DropFilenameLineEdit::~DropFilenameLineEdit()
{
}

void DropFilenameLineEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void DropFilenameLineEdit::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls())
        this->setText(event->mimeData()->urls().at(0).toLocalFile());
}
