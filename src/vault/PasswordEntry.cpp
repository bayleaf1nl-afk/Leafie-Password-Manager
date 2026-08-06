#include "PasswordEntry.h"
#include <QJsonObject>

QJsonObject PasswordEntry::toJson() const {
  QJsonObject obj;
  obj["site"]     = site;
  obj["username"] = username;
  obj["password"] = password;
  return obj;
}

PasswordEntry PasswordEntry::fromJson(const QJsonObject &obj) {
  PasswordEntry entry;
  entry.site     = obj["site"].toString();
  entry.username = obj["username"].toString();
  entry.password = obj["password"].toString();
  return entry;
}