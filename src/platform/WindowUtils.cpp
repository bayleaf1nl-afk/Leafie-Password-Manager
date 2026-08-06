#include "../vault/PasswordEntry.h"
#include "WindowManager.h"
#include <QApplication>
#include <QClipboard>
#include <QPointer>
#include <QScreen>
#include <QTimer>
namespace Platform::WindowUtils {
void centerWindow(QWidget *window) {
  QScreen *screen = window->screen();
  if (!screen) screen = QApplication::primaryScreen();
  if (!screen) return;

  QRect geometry = screen->availableGeometry();
  window->move(geometry.center() - window->rect().center());
}

void copyToClipboard(const PasswordEntry &entry) { QApplication::clipboard()->setText(entry.password); }
} // namespace Platform::WindowUtils

namespace Platform {
void attemptFloating(QWidget *window) {
  QPointer<QWidget> safeWindow = window;

  QTimer::singleShot(10, [safeWindow]() {
    if (!safeWindow) return; // now this check is actually meaningful

    if (Platform::TilingWM::available()) {
      Platform::TilingWM::requestFloating(safeWindow);
    } else {
      Platform::StandardWM::requestFloating(safeWindow);
    }
  });
}
} // namespace Platform