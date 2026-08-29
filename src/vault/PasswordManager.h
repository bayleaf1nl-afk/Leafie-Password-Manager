#pragma once

#include "PasswordEntry.h"
#include <cstddef>
#include <qcontainerfwd.h>
#include <qobject.h>
#include <sodium.h>
// REFACTOR: migrate all of the functions that constitute vault.json or
//
class PasswordManager {

public:
  PasswordManager();
  ~PasswordManager();

  // the key lives in a single guarded allocation, so the manager is move-only
  PasswordManager(const PasswordManager &)            = delete;
  PasswordManager &operator=(const PasswordManager &) = delete;
  PasswordManager(PasswordManager &&other) noexcept;
  PasswordManager &operator=(PasswordManager &&other) noexcept;

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
  // masterPassword is wiped and cleared before returning; it is never retained
  bool       deriveKey(QByteArray &masterPassword, const unsigned char *existingSalt = nullptr);
  QByteArray encrypt(const QByteArray &plaintext) const;
  QByteArray decrypt(const QByteArray &data) const;
  bool       isValidIndex(int index);
  QByteArray stringify() const;

private:
  static constexpr std::size_t KEY_BYTES = crypto_secretbox_KEYBYTES;

  QVector<PasswordEntry> m_entries;
  unsigned char         *m_key = nullptr; // sodium_malloc: locked, guard-paged
  unsigned char          salt[crypto_pwhash_SALTBYTES];
};
