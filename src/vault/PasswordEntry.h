#pragma once
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <qhashfunctions.h>

struct PasswordEntry {
  QString site;
  QString username;
  QString email;
  QString password;
  //---//
  QJsonObject          toJson() const;
  static PasswordEntry fromJson(const QJsonObject &obj);
};
