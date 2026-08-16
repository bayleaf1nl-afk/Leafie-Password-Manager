#include "PasswordManager.h"
#include "PasswordEntry.h"
#include <cstring>
#include <qassert.h>
#include <qdir.h>
#include <qevent.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qlogging.h>
#include <qobject.h>
#include <qstringview.h>
#include <qtextdocument.h>
#include <sodium/crypto_box.h>
#include <sodium/crypto_pwhash.h>
#include <sodium/crypto_secretbox.h>
#include <sodium/randombytes.h>
#include <sodium/utils.h>
#include <string>

PasswordManager::PasswordManager(const QString &masterPassword) { m_masterPassword = masterPassword; }
PasswordManager::~PasswordManager() { sodium_memzero(m_key, sizeof m_key); }
const QVector<PasswordEntry> &PasswordManager::entries() const { return m_entries; }

bool PasswordManager::isValidIndex(int index) { return index >= 0 && index < m_entries.size(); }

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
  QByteArray plaintext = stringify();
  QByteArray encrypted = encrypt(plaintext);
  if (encrypted.isEmpty()) return false; // encrypt() failed, dont even try anything further

  QFile file("vault.json");
  if (!file.open(QIODevice::WriteOnly)) return false;
  return file.write(encrypted) != -1;
}

bool PasswordManager::LoadVault() {
  QFile file("vault.json");
  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "Could not open file: ";
    return false;
  };

  QByteArray encrypted = file.readAll();
  QByteArray plaintext = decrypt(encrypted);
  if (plaintext.isEmpty()) return false; // decrypt() failed, don't even try anything

  QJsonDocument doc = QJsonDocument::fromJson(plaintext);
  if (doc.isNull() || !doc.isArray()) {
    qWarning() << "Vault file is corrupted or not valid JSON";
    return false;
  }
  m_entries.clear();

  for (const auto &value : doc.array()) {
    if (!value.isObject()) continue;
    m_entries.push_back(PasswordEntry::fromJson(value.toObject()));
  }
  return true;
}

bool PasswordManager::exportVault(const QString &path) const {
  QByteArray plaintext = stringify();
  QByteArray encrypted = encrypt(plaintext);
  if (encrypted.isEmpty()) return false;
  QFile file(path);

  if (!file.open(QIODevice::WriteOnly)) return false;

  return file.write(encrypted) != -1;
}

bool PasswordManager::importVault(const QString &path) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) return false;

  QByteArray encrypted = file.readAll();
  QByteArray plaintext = decrypt(encrypted);
  if (plaintext.isEmpty()) return false;

  QJsonDocument doc = QJsonDocument::fromJson(plaintext);

  if (doc.isNull() || !doc.isArray()) return false;

  const QJsonArray array = doc.array();

  for (const auto &value : array) {
    if (!value.isObject()) continue;
    m_entries.push_back(PasswordEntry::fromJson(value.toObject()));
  }

  return SaveVault();
}

//------------------------------------------------------------------------------//

QByteArray PasswordManager::stringify() const { // returns qbytearray of vault.json
  QJsonArray array;

  for (const auto &entry : m_entries) {
    array.append(entry.toJson());
  }

  QJsonDocument doc(array);
  return doc.toJson();
}

QByteArray PasswordManager::encrypt(const QByteArray &plaintext) const {
  unsigned char nonce[crypto_secretbox_NONCEBYTES];
  randombytes_buf(nonce, sizeof nonce);

  QByteArray ciphertext(crypto_secretbox_MACBYTES + plaintext.size(), 0);

  if (crypto_secretbox_easy(reinterpret_cast<unsigned char *>(ciphertext.data()),
                            reinterpret_cast<const unsigned char *>(plaintext.constData()), plaintext.size(), nonce,
                            m_key) != 0) {
    return QByteArray();
  }
  QByteArray result(reinterpret_cast<const char *>(nonce), sizeof nonce);
  result.append(ciphertext); // the text will look something like [nonce{24}bytes][MACtag{16}bytes][ciphertext{x}bytes]
  return result;
}

QByteArray PasswordManager::decrypt(const QByteArray &data) const {
  if (data.size() < crypto_secretbox_NONCEBYTES + crypto_secretbox_MACBYTES) {
    return QByteArray(); // too short to possibly be valid.
  }
  QByteArray nonce      = data.mid(0, crypto_secretbox_NONCEBYTES);
  QByteArray ciphertext = data.mid(crypto_secretbox_NONCEBYTES);
  QByteArray plaintext(ciphertext.size() - crypto_secretbox_MACBYTES, 0);

  if (crypto_secretbox_open_easy(reinterpret_cast<unsigned char *>(plaintext.data()),
                                 reinterpret_cast<const unsigned char *>(ciphertext.constData()), ciphertext.size(),
                                 reinterpret_cast<const unsigned char *>(nonce.constData()), m_key) != 0) {
    return QByteArray();
  }

  return plaintext;
}

bool PasswordManager::deriveKey(const unsigned char *existingSalt = nullptr) {
  if (existingSalt) {
    memcpy(salt, existingSalt, sizeof salt); // copy it from pre-existing salt
  } else {
    randombytes_buf(salt, sizeof salt); // generate one otherwise; first login only
  }

  const std::string stdPass   = m_masterPassword.toStdString();
  const char *const passInput = stdPass.c_str();

  auto result = crypto_pwhash(m_key, sizeof(m_key), passInput, stdPass.size(), salt, crypto_pwhash_OPSLIMIT_INTERACTIVE,
                              crypto_pwhash_MEMLIMIT_INTERACTIVE, crypto_pwhash_ALG_DEFAULT);
  if (result == 0) {
    return true;
  }
  qDebug() << "crypto_pwhash result:" << result;
  qDebug() << "key size:" << sizeof(m_key);
  qDebug() << "salt size:" << sizeof(salt);
  qDebug() << "password length:" << stdPass.size();
  return false; //! oom error. handle it some other way but for now just return false
}
