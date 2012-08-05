#include "xslexpress.h"

#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    XSLExpress w;
    w.show();
    return a.exec();
}
