TEMPLATE = app
TARGET = XSLExpress
QT += core gui widgets
HEADERS += DropFilenameLineEdit.h \
    settings.h \
    xslexpress.h \
    dropfilenametextedit.h \
    xslpathsdialog.h \
    xsltproc.h \
    settingsnamedialog.h
SOURCES += DropFilenameLineEdit.cpp \
    main.cpp \
    settings.cpp \
    xslexpress.cpp \
    dropfilenametextedit.cpp \
    xslpathsdialog.cpp \
    xsltproc.cpp \
    settingsnamedialog.cpp
FORMS += xslexpress.ui \
    settingsnamedialog.ui \
    xslpathsdialog.ui
RESOURCES +=
LIBS += -L./ \
    -llibexslt \
    -lxslt \
    -lxml2
    -liconv
RC_ICONS = icons/XSLExpress.ico
