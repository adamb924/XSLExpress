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

RC_ICONS = icons/XSLExpress.ico

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../xlstproc64/lib/ -lexslt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../xlstproc64/lib/ -lexslt
else:unix: LIBS += -L$$PWD/../xlstproc64/lib/ -lexslt

INCLUDEPATH += $$PWD/../xlstproc64/include
DEPENDPATH += $$PWD/../xlstproc64/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../xlstproc64/lib/ -lxml2
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../xlstproc64/lib/ -lxml2
else:unix: LIBS += -L$$PWD/../xlstproc64/lib/ -lxml2

INCLUDEPATH += $$PWD/../xlstproc64/include
DEPENDPATH += $$PWD/../xlstproc64/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../xlstproc64/lib/ -lxslt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../xlstproc64/lib/ -lxslt
else:unix: LIBS += -L$$PWD/../xlstproc64/lib/ -lxslt

INCLUDEPATH += $$PWD/../xlstproc64/include
DEPENDPATH += $$PWD/../xlstproc64/include
