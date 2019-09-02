#ifndef SETTINGS_H
#define SETTINGS_H

#include <QHash>
#include <QString>

class Settings
{
public:
    Settings();

private:
    QString mKey;
    QString mReplaceThis;
    QString mReplaceWith;
    QString mXslFile;
    QHash<QString,QString> mParameters;

    friend class XSLExpress;
};

#endif // SETTINGS_H
