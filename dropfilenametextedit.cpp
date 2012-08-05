#include "dropfilenametextedit.h"

#include <QUrl>
#include <QtDebug>

DropFilenameTextEdit::DropFilenameTextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    setAcceptDrops(true);
}

void DropFilenameTextEdit::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void DropFilenameTextEdit::dragMoveEvent ( QDragMoveEvent * event )
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void DropFilenameTextEdit::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasUrls())
    {
        QList<QUrl> list = event->mimeData()->urls();
        QString text;
        text = list.at(0).toLocalFile();
        for(int i=1; i<list.count(); i++)
        {
            text += "\n" + list.at(i).toLocalFile();
        }
        this->setPlainText(text);
        emit drop();
    }
}
