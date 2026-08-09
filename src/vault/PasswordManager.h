#pragma once

#include "PasswordEntry.h"
#include <qcontainerfwd.h>
#include <qobject.h>
#include <sodium.h>
// REFACTOR: migrate all of the functions that constitute vault.json or
//
class PasswordManager {

public:
  explicit PasswordManager(const QString &masterPassword);
  ~PasswordManager();
  // read-only access to the m_entries vector by reference; promise to not modify the manager's own vector
  // why cant i just declare ts readonly :(
  const QVector<PasswordEntry> &entries() const;

  bool       addEntry(const PasswordEntry &entry);
  bool       removeEntry(int index);
  bool       updateEntry(int index, const PasswordEntry &entry);
  bool       isDuplicateEntry(const QString &site, const QString &username) const;
  bool       SaveVault();
  bool       LoadVault();
  bool       exportVault(const QString &path) const;
  bool       importVault(const QString &path);
  bool       deriveKey(const unsigned char *existingSalt);
  QByteArray encrypt(const QByteArray &plaintext);
  QByteArray decrypt(const QByteArray &data);
  bool       isValidIndex(int index);
  QByteArray stringify() const;

private:
  QVector<PasswordEntry> m_entries;
  QString                m_masterPassword;
  unsigned char          salt[crypto_pwhash_SALTBYTES];
  unsigned char          m_key[crypto_secretbox_KEYBYTES];
};