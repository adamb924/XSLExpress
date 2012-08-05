/*
 * DropFilenameLineEdit.h
 *
 *  Created on: Aug 1, 2012
 *      Author: Adam
 */

#ifndef DROPFILENAMELINEEDIT_H_
#define DROPFILENAMELINEEDIT_H_

#include <qlineedit.h>

class DropFilenameLineEdit: public QLineEdit
{
public:
	DropFilenameLineEdit();
        DropFilenameLineEdit( QWidget * parent = 0 );
	virtual ~DropFilenameLineEdit();
private:
        void dragEnterEvent(QDragEnterEvent *event);
        void dropEvent ( QDropEvent * event );
};

#endif /* DROPFILENAMELINEEDIT_H_ */
