#pragma once
#include "../vault/PasswordManager.h"
#include <QByteArray>
#include <QFile>
#include <QSaveFile>
#include <QString>
#include <optional>
#include <sodium/crypto_pwhash.h>
class LoginGate {
public:
  static const int LOGIN_HASH_BYTES = 32;

  static std::optional<PasswordManager> authenticate();
  struct masterFileData {
    unsigned char loginSalt[crypto_pwhash_SALTBYTES];
    unsigned char loginHash[LOGIN_HASH_BYTES];
    unsigned char vaultSalt[crypto_pwhash_SALTBYTES];
  };

private:
  static bool readMasterFile(masterFileData &out, QFile &file);
  bool        writeMasterFile();
  static bool verifyMasterFile(const QByteArray &password, const unsigned char *storedSalt,
                               const unsigned char *storedHash);

  static std::optional<PasswordManager> firstTimeInstallation(const QString &title, const QString &label);
};
