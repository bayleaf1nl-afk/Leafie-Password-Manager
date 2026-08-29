#include "PasswordUtils.h"
#include "../ui/DialogUtils.h"
#include "zxcvbn/zxcvbn.hpp"
#include <QString>
#include <cstdint>
#include <qobject.h>
#include <sodium/randombytes.h>
#include <string>
#include <vector>

QString PasswordUtils::generatePassword(const GenerationSettings &settings) {
  QString charset = R"(abcdefghijklmnopqrstuvwxyz)";
  if (settings.useSymbols) {
    charset += R"(!@#$%^&*()_+{}|:"<>?[]\;',./)";
  }
  if (settings.useDigits) {
    charset += R"(0123456789)";
  }
  if (settings.useUppercase) {
    charset += R"(ABCDEFGHIJKLMNOPQRSTUVWXYZ)";
  }
  QString result;
  result.reserve(settings.length);

  for (int i = 0; i < settings.length; ++i) {
    uint32_t character = randombytes_uniform(static_cast<uint32_t>(charset.size()));
    result.append(charset.at(character));
  }
  return result;
}

PasswordUtils::GenerationSettings DialogUtils::PasswordGenerationWindow::currentSettings() const {
  return {lengthSpin->value(), symbolsCheck->isChecked(), digitsCheck->isChecked(), uppercaseCheck->isChecked()};
}

double PasswordUtils::estimateStrength(const QString &password, const QStringList &context) {
  std::vector<std::string> stdContext;
  for (const auto &s : context)
    stdContext.push_back(s.toStdString());

  std::vector<const char *> userInputs;
  for (const auto &s : stdContext)
    userInputs.push_back(s.c_str());
  userInputs.push_back(nullptr);
  zxcvbn_guesses_t guesses = 0;

  int rc = zxcvbn_password_strength(password.toStdString().c_str(), userInputs.data(), &guesses, nullptr);
  if (rc != 0) return 0.0; // failed

  return std::log10(guesses);
}

QString PasswordUtils::strengthLabel(double guessesLog10) {
  // read on zxcvbn docs for more info; these are kind of arbitrary and magic numbers
  if (guessesLog10 < 3) return "Very Weak";
  if (guessesLog10 < 6) return "Weak";
  if (guessesLog10 < 8) return "Okay";
  if (guessesLog10 < 10) return "Strong";
  return "Very Strong";
}
