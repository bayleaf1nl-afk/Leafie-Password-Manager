#pragma once
#include <QString>
#include <qjsonobject.h>
#include <qobject.h>
#include <QJsonObject>

struct PasswordEntry{
    QString site;
    QString username;
    QString password;

    QJsonObject toJson() const;
    static PasswordEntry fromJson(const QJsonObject &obj);
};

