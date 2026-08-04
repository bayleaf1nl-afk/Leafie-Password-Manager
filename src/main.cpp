#include "LoginGate.h"
#include "MainWindow.h"
#include "PasswordEntry.h"
#include "PasswordManager.h"
#include "platform/WindowManager.h"
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
#include <qtmetamacros.h>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setApplicationName("PasswordManager");

  QString masterPassword;
  if (!LoginGate::authenticate(masterPassword)) {
    return 0;
  }

  MainWindow window;
  QFile file(":/styles.qss");

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