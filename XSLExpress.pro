TEMPLATE = app
TARGET = XSLExpress
QT += core gui widgets
HEADERS += DropFilenameLineEdit.h \
    xslexpress.h \
    dropfilenametextedit.h \
    xsltproc.h \
    settingsnamedialog.h
SOURCES += DropFilenameLineEdit.cpp \
    main.cpp \
    xslexpress.cpp \
    dropfilenametextedit.cpp \
    xsltproc.cpp \
    settingsnamedialog.cpp
FORMS += xslexpress.ui \
    settingsnamedialog.ui
RESOURCES +=
LIBS += -L./ \
    -llibexslt \
    -lxslt \
    -lxml2
    -liconv
