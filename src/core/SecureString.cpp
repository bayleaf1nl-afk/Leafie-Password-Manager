#include "SecureString.h"
#include <QByteArray>
#include <cstring>
#include <sodium.h>
#include <utility>

SecureString::SecureString(const QString &text) {
  QByteArray utf8 = text.toUtf8();
  assign(utf8.constData(), static_cast<std::size_t>(utf8.size()));
  if (!utf8.isEmpty()) sodium_memzero(utf8.data(), utf8.size());
}

SecureString::SecureString(const SecureString &other) { assign(other.m_data, other.m_size); }

SecureString::SecureString(SecureString &&other) noexcept : m_data(other.m_data), m_size(other.m_size) {
  other.m_data = nullptr;
  other.m_size = 0;
}

SecureString &SecureString::operator=(const SecureString &other) {
  if (this == &other) return *this;
  clear();
  assign(other.m_data, other.m_size);
  return *this;
}

SecureString &SecureString::operator=(SecureString &&other) noexcept {
  if (this == &other) return *this;
  clear();
  m_data       = other.m_data;
  m_size       = other.m_size;
  other.m_data = nullptr;
  other.m_size = 0;
  return *this;
}

SecureString::~SecureString() { clear(); }

void SecureString::clear() {
  sodium_free(m_data); // zeroes the buffer before releasing it
  m_data = nullptr;
  m_size = 0;
}

void SecureString::assign(const char *data, std::size_t size) {
  if (size == 0) return;

  m_data = static_cast<char *>(sodium_malloc(size + 1));
  if (!m_data) qFatal("could not allocate guarded memory for a secret");
  memcpy(m_data, data, size);
  m_data[size] = '\0';
  m_size       = size;
}

qsizetype SecureString::charCount() const {
  qsizetype count = 0;
  for (std::size_t i = 0; i < m_size; ++i) {
    if ((static_cast<unsigned char>(m_data[i]) & 0xC0) != 0x80) ++count; // skip UTF-8 continuation bytes
  }
  return count;
}

QString SecureString::toQString() const {
  if (m_size == 0) return QString();
  return QString::fromUtf8(m_data, static_cast<qsizetype>(m_size));
}

bool SecureString::operator==(const SecureString &other) const {
  if (m_size != other.m_size) return false;
  if (m_size == 0) return true;
  return sodium_memcmp(m_data, other.m_data, m_size) == 0;
}
