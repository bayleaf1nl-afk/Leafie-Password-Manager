#include "LoginGate.h"
#include "../platform/WindowManager.h"
#include "../ui/GenericDialog.h"
#include "../vault/PasswordManager.h"
#include "FileUtils.h"
#include <QDebug>
#include <QDialog>
#include <QFile>
#include <QFileDevice>
#include <QLineEdit>
#include <QMessageBox>
#include <QSaveFile>
#include <QTimer>
#include <optional>
#include <qcborvalue.h>
#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qmessagebox.h>
#include <qobject.h>
#include <qwindowdefs.h>
#include <sodium/crypto_pwhash.h>
#include <sodium/randombytes.h>
#include <sodium/utils.h>
std::optional<PasswordManager> LoginGate::authenticate() {
  QFile   file(FileUtils::masterFilePath());
  bool    isFirstLaunch = !file.exists();
  QString title         = isFirstLaunch ? "Set Master Password" : "Login";
  QString label         = isFirstLaunch ? "Please write your new master password" : "Please enter your master password";

  if (isFirstLaunch) {
    return firstTimeInstallation(title, label);
  }

  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(nullptr, "Error", "Could not read from master password file");
    return std::nullopt;
  }
  masterFileData data;
  qDebug() << "master file size:" << file.size();
  qDebug() << "expected:" << sizeof(data.loginSalt) << sizeof(data.loginHash) << sizeof(data.vaultSalt);
  if (!readMasterFile(data, file)) {
    QMessageBox::critical(nullptr, "Error", "Master password file is corrupted.");
    return std::nullopt;
  }

  const int maxAttempts = 3;

  for (int attempt = 1; attempt <= maxAttempts; ++attempt) {
    QString label = QString("Master Password (%1/%2 attempts): ").arg(attempt).arg(maxAttempts);

    DialogUtils::GenericDialog dlg("Login", {{label, QLineEdit::Password}}, false);

    if (dlg.exec() != QDialog::Accepted) return std::nullopt;
    QByteArray password = dlg.inputText(0).toUtf8();

    if (!verifyMasterFile(password, data.loginSalt, data.loginHash)) {
      continue;
    }

    PasswordManager candidate(password);
    if (!candidate.deriveKey(data.vaultSalt)) {
      QMessageBox::critical(nullptr, "Login Failed", "Could not derive the vault encryption key.");
      return std::nullopt;
    }
    if (!candidate.LoadVault()) {
      QMessageBox::critical(nullptr, "Login failed", "Vault file is corrupted.");
      return std::nullopt;
    }
    sodium_memzero(password.data(), password.size());
    password.clear();
    return candidate;
  }

  QMessageBox::critical(nullptr, "Login Failed", "Too many incorrect attempts.");
  return std::nullopt;
}

bool LoginGate::verifyMasterFile(const QString &passwd, const unsigned char *storedSalt,
                                 const unsigned char *storedHash) {
  unsigned char     computedHash[LOGIN_HASH_BYTES];
  const std::string stdPass = passwd.toStdString();

  if (crypto_pwhash(computedHash, sizeof computedHash, stdPass.c_str(), stdPass.size(), storedSalt,
                    crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
                    crypto_pwhash_ALG_DEFAULT) != 0) {
    return false; //! something fucked up here somehow
  }
  return sodium_memcmp(computedHash, storedHash, sizeof computedHash) == 0;
}

bool LoginGate::writeMasterFile() { return true; }

bool LoginGate::readMasterFile(masterFileData &out, QFile &file) {
  qint64 loginSaltRead = file.read(reinterpret_cast<char *>(out.loginSalt), sizeof out.loginSalt);
  qDebug() << "loginSalt:" << loginSaltRead << "/" << sizeof out.loginSalt;
  if (loginSaltRead != sizeof out.loginSalt) return false;
  qint64 loginHashRead = file.read(reinterpret_cast<char *>(out.loginHash), sizeof out.loginHash);
  qDebug() << "loginHash:" << loginHashRead << "/" << sizeof out.loginHash;
  if (loginHashRead != sizeof out.loginHash) return false;
  qint64 vaultSaltRead = file.read(reinterpret_cast<char *>(out.vaultSalt), sizeof out.vaultSalt);
  qDebug() << "vaultSalt:" << vaultSaltRead << "/" << sizeof out.vaultSalt;
  if (vaultSaltRead != sizeof out.vaultSalt) return false;

  return true;
}

std::optional<PasswordManager> LoginGate::firstTimeInstallation(const QString &title, const QString &label) {

  QSaveFile     file(FileUtils::masterFilePath());
  unsigned char loginSalt[crypto_pwhash_SALTBYTES];
  unsigned char vaultSalt[crypto_pwhash_SALTBYTES];
  randombytes_buf(loginSalt, sizeof loginSalt);
  randombytes_buf(vaultSalt, sizeof vaultSalt);

  DialogUtils::GenericDialog dialog(title, {{label, QLineEdit::Password}}, false, nullptr);
  if (dialog.exec() != QDialog::Accepted) return std::nullopt;
  Platform::attemptFloating(&dialog);
  QString outMasterPassword = dialog.inputText(0);

  unsigned char computedHash[LOGIN_HASH_BYTES];
  auto          stdPassword = outMasterPassword.toStdString();
  const char   *cPassword   = stdPassword.c_str();
  if (crypto_pwhash(computedHash, sizeof computedHash, cPassword, stdPassword.size(), loginSalt,
                    crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
                    crypto_pwhash_ALG_DEFAULT) != 0) {
    QMessageBox::critical(nullptr, "Error", "Could not hash the password properly!!");
    return std::nullopt;
  }

  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(nullptr, "Error", "Could not write to master password file");
    return std::nullopt;
  }
  file.write(reinterpret_cast<const char *>(loginSalt), sizeof loginSalt);
  file.write(reinterpret_cast<const char *>(computedHash), sizeof computedHash);
  file.write(reinterpret_cast<const char *>(vaultSalt), sizeof vaultSalt);

  if (!file.commit()) {
    QMessageBox::critical(nullptr, "Error", "Could not finalize master password file write");
    return std::nullopt;
  }
  PasswordManager candidate(outMasterPassword);
  if (!candidate.deriveKey(vaultSalt)) {
    QMessageBox::critical(nullptr, "Error", "Could not derive the key to encrypt the vault!");
    return std::nullopt;
  }
  if (!candidate.SaveVault()) {
    QMessageBox::critical(nullptr, "Error", "Could not write the vault file.");
    return std::nullopt;
  }
  return candidate;
}
