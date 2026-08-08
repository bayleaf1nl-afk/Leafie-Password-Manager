#include "PasswordManager.h"
#include "PasswordEntry.h"
#include <iostream>
#include <qdir.h>
#include <qevent.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qobject.h>
#include <qstringview.h>
#include <string>

PasswordManager::PasswordManager(const QString &masterPassword) { m_masterPassword = masterPassword; }

const QVector<PasswordEntry> &PasswordManager::entries() const { return m_entries; }

bool PasswordManager::isValidIndex(int index) { return index > 0 && index < m_entries.size(); }

bool PasswordManager::isDuplicateEntry(const QString &site, const QString &username) const {
  for (const PasswordEntry &entry : m_entries) {
    if (entry.site == site && entry.username == username) return true;
  }
  return false;
}

bool PasswordManager::removeEntry(int index) {
  if (!isValidIndex(index)) {
    return false;
  }
  m_entries.remove(index);
  return SaveVault();
}

bool PasswordManager::updateEntry(int index, const PasswordEntry &entry) {
  if (!isValidIndex(index)) return false;

  m_entries[index] = entry;
  return SaveVault();
}

bool PasswordManager::addEntry(const PasswordEntry &entry) {
  m_entries.push_back(entry);
  return SaveVault();
}

bool PasswordManager::SaveVault() {
  QJsonArray array;

  for (const auto &entry : m_entries) {
    array.append(entry.toJson());
  }

  QJsonDocument doc(array);

  QFile file("vault.json");
  if (!file.open(QIODevice::WriteOnly)) return false;
  return file.write(doc.toJson()) != -1;
}

bool PasswordManager::LoadVault() {
  QFile file("vault.json");
  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "Could not open file: ";
    return false;
  };
  QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
  if (doc.isNull()) {
    qWarning() << "Vault file is corrupted or not valid JSON";
    return false;
  }
  QJsonArray array = doc.array();

  for (const auto &value : array) {
    m_entries.push_back(PasswordEntry::fromJson(value.toObject()));
  }
  return true;
}