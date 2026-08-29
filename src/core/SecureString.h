#pragma once

#include <QString>
#include <cstddef>

// UTF-8 secret held in a sodium_malloc'd buffer: locked (never swapped), guard-paged, and zeroed on destruction.
// Unlike QString, its storage is never implicitly shared, so a copy is always a copy this class can wipe.
class SecureString {
public:
  SecureString() = default;
  explicit SecureString(const QString &text);
  SecureString(const SecureString &other);
  SecureString(SecureString &&other) noexcept;
  SecureString &operator=(const SecureString &other);
  SecureString &operator=(SecureString &&other) noexcept;
  ~SecureString();

  bool        isEmpty() const { return m_size == 0; }
  std::size_t size() const { return m_size; } // bytes
  // number of characters, for masking; computed without materializing the plaintext
  qsizetype charCount() const;
  void      clear();

  // materializes an unprotected plaintext copy; keep it as short-lived as possible
  QString toQString() const;

  bool operator==(const SecureString &other) const;
  bool operator!=(const SecureString &other) const { return !(*this == other); }

private:
  void assign(const char *data, std::size_t size);

  char       *m_data = nullptr; // sodium_malloc: locked, guard-paged, NUL-terminated
  std::size_t m_size = 0;
};
