#include "core/LoginGate.h"
#include "platform/WindowManager.h"
#include "ui/MainWindow.h"
#include "vault/PasswordManager.h"
//---------------------------//
#include <QApplication>
#include <QDialog>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenuBar>
#include <QProcess>
#include <QPushButton>
#include <QScreen>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <optional>
#include <qdebug.h>
#include <qlogging.h>
#include <qtmetamacros.h>
#include <sodium.h>
#include <sodium/core.h>

int main(int argc, char *argv[]) {
  if (sodium_init() < 0) {
    qFatal() << "libsodium failed to init\n";
  } else {
    qDebug() << "libsodium is alive\n";
  }

  QApplication app(argc, argv);
  app.setApplicationName("PasswordManager");

  std::optional<PasswordManager> manager = LoginGate::authenticate();
  if (!manager) {
    return 0;
  } // auth and get candidate PM

  MainWindow window(std::move(*manager)); // move candidate PasswordManager's ownership to MainWindow
  QFile      file(":/styles.qss");

  if (file.open(QFile::ReadOnly))
    app.setStyleSheet(file.readAll());
  else
    qWarning() << file.errorString();

  Platform::attemptFloating(&window);

  window.resize(800, 600);
  window.show();
  Platform::WindowUtils::centerWindow(&window);

  return app.exec();
}
