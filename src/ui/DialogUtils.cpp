#include "DialogUtils.h"
#include <QMessageBox>

int confirmationWindow(const QString &title, const QString &text) {
  auto msgBox = QMessageBox();
  msgBox.setWindowTitle(title);
  msgBox.setText(text);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);
  msgBox.setIcon(QMessageBox::NoIcon);
  int choice = msgBox.exec();
  return choice;
}