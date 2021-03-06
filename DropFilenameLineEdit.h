/*
 * DropFilenameLineEdit.h
 *
 *  Created on: Aug 1, 2012
 *      Author: Adam
 */

#ifndef DROPFILENAMELINEEDIT_H_
#define DROPFILENAMELINEEDIT_H_

#include <QtWidgets/QLineEdit>

class DropFilenameLineEdit: public QLineEdit
{
public:
    enum DropType { UriEncode, NoEncode };

    explicit DropFilenameLineEdit(QWidget * parent = nullptr );
    explicit DropFilenameLineEdit(DropType type = NoEncode, QWidget * parent = nullptr );

private:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent ( QDropEvent * event );
    DropType mType;
};

#endif /* DROPFILENAMELINEEDIT_H_ */
