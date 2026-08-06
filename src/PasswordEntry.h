#pragma once
#include <QJsonObject>
#include <QObject>
#include <QString>

struct PasswordEntry {
  QString site;
  QString username;
  QString password;

  QJsonObject          toJson() const;
  static PasswordEntry fromJson(const QJsonObject &obj);
};
