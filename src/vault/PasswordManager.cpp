#include "PasswordManager.h"
#include "../core/FileUtils.h"
#include "PasswordEntry.h"
#include <QSaveFile>
#include <cstring>
#include <qassert.h>
#include <qdir.h>
#include <qevent.h>
#include <qfiledevice.h>
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
#include <utility>

namespace {
void wipe(QByteArray &buffer) {
  if (!buffer.isEmpty()) sodium_memzero(buffer.data(), buffer.size());
  buffer.clear();
}
} // namespace

PasswordManager::PasswordManager() {
  m_key = static_cast<unsigned char *>(sodium_malloc(KEY_BYTES));
  if (!m_key) qFatal("could not allocate guarded memory for the vault key");
  sodium_memzero(salt, sizeof salt);
  sodium_mlock(salt, sizeof salt);
}

PasswordManager::PasswordManager(PasswordManager &&other) noexcept : m_entries(std::move(other.m_entries)) {
  sodium_mlock(salt, sizeof salt);
  memcpy(salt, other.salt, sizeof salt);
  m_key       = other.m_key;
  other.m_key = nullptr;
  sodium_memzero(other.salt, sizeof other.salt);
}

PasswordManager &PasswordManager::operator=(PasswordManager &&other) noexcept {
  if (this == &other) return *this;

  m_entries = std::move(other.m_entries);
  sodium_free(m_key);
  m_key       = other.m_key;
  other.m_key = nullptr;
  memcpy(salt, other.salt, sizeof salt);
  sodium_memzero(other.salt, sizeof other.salt);
  return *this;
}

PasswordManager::~PasswordManager() {
  sodium_free(m_key); // zeroes the key before unlocking and releasing the guarded pages
  m_key = nullptr;
  sodium_munlock(salt, sizeof salt); // zeroes as well
}

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
  wipe(plaintext);
  if (encrypted.isEmpty()) return false; // encrypt() failed, dont even try anything further

  QSaveFile file(FileUtils::vaultPath());
  if (!file.open(QIODevice::WriteOnly)) return false;
  if (file.write(encrypted) == -1) return false;

  if (!file.commit()) return false;
  QFile(file.fileName()).setPermissions(QFileDevice::WriteOwner | QFileDevice::ReadOwner);
  return true;
}

bool PasswordManager::LoadVault() {
  QFile file(FileUtils::vaultPath());
  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "Could not open file: ";
    return false;
  };

  QByteArray encrypted = file.readAll();
  QByteArray plaintext = decrypt(encrypted);
  if (plaintext.isEmpty()) return false; // decrypt() failed, don't even try anything

  QJsonDocument doc = QJsonDocument::fromJson(plaintext);
  wipe(plaintext);
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
  wipe(plaintext);
  if (encrypted.isEmpty()) return false;
  QSaveFile file(path);

  if (!file.open(QIODevice::WriteOnly)) return false;
  if (file.write(encrypted) != -1) return false;

  return file.commit();
}

bool PasswordManager::importVault(const QString &path) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly)) return false;

  QByteArray encrypted = file.readAll();
  QByteArray plaintext = decrypt(encrypted);
  if (plaintext.isEmpty()) return false;

  QJsonDocument doc = QJsonDocument::fromJson(plaintext);
  wipe(plaintext);

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

bool PasswordManager::deriveKey(QByteArray &masterPassword, const unsigned char *existingSalt) {
  if (existingSalt) {
    memcpy(salt, existingSalt, sizeof salt); // copy it from pre-existing salt
  } else {
    randombytes_buf(salt, sizeof salt); // generate one otherwise; first login only
  }

  const std::size_t passLength = static_cast<std::size_t>(masterPassword.size());

  char *passInput = static_cast<char *>(sodium_malloc(passLength + 1));
  if (!passInput) {
    wipe(masterPassword);
    return false;
  }
  memcpy(passInput, masterPassword.constData(), passLength);
  passInput[passLength] = '\0';
  wipe(masterPassword); // the caller's copy is gone from here on

  auto result = crypto_pwhash(m_key, KEY_BYTES, passInput, passLength, salt, crypto_pwhash_OPSLIMIT_INTERACTIVE,
                              crypto_pwhash_MEMLIMIT_INTERACTIVE, crypto_pwhash_ALG_DEFAULT);
  sodium_free(passInput);

  if (result == 0) {
    return true;
  }
  qDebug() << "crypto_pwhash result:" << result;
  qDebug() << "key size:" << KEY_BYTES;
  qDebug() << "salt size:" << sizeof(salt);
  qDebug() << "password length:" << passLength;
  return false; //! oom error. handle it some other way but for now just return false
}
