#ifndef DROPFILENAMETEXTEDIT_H
#define DROPFILENAMETEXTEDIT_H

#include <QPlainTextEdit>

class DropFilenameTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit DropFilenameTextEdit(QWidget *parent = 0);

private:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent ( QDragMoveEvent * event );
    void dropEvent ( QDropEvent * event );

signals:
    void drop();

public slots:

};

#endif // DROPFILENAMETEXTEDIT_H
