#pragma once
#include "../PasswordEntry.h"
#include <QWidget>
namespace Platform {
void attemptFloating(QWidget *window);

namespace StandardWM {
void requestFloating(QWidget *);
}

namespace TilingWM {
void requestFloating(QWidget *);
bool available();
} // namespace TilingWM
namespace WindowUtils {
void centerWindow(QWidget *window);
void copyToClipboard(const PasswordEntry &entry);
} // namespace WindowUtils

} // namespace Platform
