#include "core/LoginGate.h"
#include "platform/WindowManager.h"
#include "ui/MainWindow.h"
#include "vault/PasswordEntry.h"
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
#include <iostream>
#include <print>
#include <qdebug.h>
#include <qtmetamacros.h>
#include <sodium.h>
#include <sodium/core.h>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setApplicationName("PasswordManager");

  QString masterPassword;
  if (!LoginGate::authenticate(masterPassword)) {
    return 0;
  }

  MainWindow window;
  QFile      file(":/styles.qss");

  if (file.open(QFile::ReadOnly))
    app.setStyleSheet(file.readAll());
  else
    qWarning() << file.errorString();

  Platform::attemptFloating(&window);

  window.resize(800, 600);
  window.show();
  Platform::WindowUtils::centerWindow(&window);

  if (sodium_init() < 0) {
    qDebug() << "libsodium failed to init\n";
  } else {
    qDebug() << "libsodium is alive\n";
  }

  return app.exec();
}