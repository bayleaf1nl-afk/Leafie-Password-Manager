#pragma once
#include "../core/SecureString.h"
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <qhashfunctions.h>

struct PasswordEntry {
  QString site;
  QString username;
  QString email;
  SecureString password;
  //---//
  QJsonObject          toJson() const;
  static PasswordEntry fromJson(const QJsonObject &obj);
};
