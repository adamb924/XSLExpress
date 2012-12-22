TEMPLATE = app
TARGET = XSLExpress
QT += core \
    gui
HEADERS += DropFilenameLineEdit.h \
    xslexpress.h \
    dropfilenametextedit.h \
    xsltproc.h
SOURCES += DropFilenameLineEdit.cpp \
    main.cpp \
    xslexpress.cpp \
    dropfilenametextedit.cpp \
    xsltproc.cpp
FORMS += xslexpress.ui
RESOURCES +=
LIBS += -L./ \
    -lexslt \
    -llibxslt \
    -llibxml2 \
    -liconv
