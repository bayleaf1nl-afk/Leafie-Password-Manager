#pragma once
#include <qobject.h>
namespace PasswordUtils {
struct GenerationSettings {
  int  length       = 16;
  bool useSymbols   = false;
  bool useDigits    = true;
  bool useUppercase = true;
};

GenerationSettings currentSettings();
double             estimateStrength(const QString &password, const QStringList &context);
QString            generatePassword(const GenerationSettings &settings);
QString            strengthLabel(double guessesLog10);
} // namespace PasswordUtils
