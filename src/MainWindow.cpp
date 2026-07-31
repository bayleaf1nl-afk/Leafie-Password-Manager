#include "MainWindow.h"
#include "LoginGate.h"
#include "PasswordEntry.h"
#include "PasswordManager.h"
#include "GenericDialog.h"
#include "platform/WindowManager.h"
#include "PasswordEntry.h"
//---------------------------//
#include <QApplication>
#include <QFile>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QMenuBar>
#include <QLabel>
#include <QDialog>
#include <QProcess>
#include <QTimer>
#include <QScreen>
#include <qlineedit.h>
#include <qwindowdefs.h>

MainWindow::MainWindow() {
  setWindowTitle("Leaf's Password Manager");
  auto *central = new QWidget;
  setCentralWidget(central);
  auto *layout = new QVBoxLayout(central);

  topMenu = new QMenuBar(this);
  topMenu->setSizePolicy(QSizePolicy::Expanding,  // size policy for the menu,
                         QSizePolicy::Expanding); // horizontally and vertically

  infoLabel = new QLabel(tr("Use this!!"));
  infoLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
  infoLabel->setAlignment(Qt::AlignCenter);
  infoLabel->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);

  auto *bottomFiller = new QWidget;
  bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  addButton = new QPushButton("Add", this);
  delButton = new QPushButton("Delete", this);
  passwordList = new QListWidget(this);
  searchBox = new QLineEdit(this);

  layout->setContentsMargins(5, 5, 5, 5);
  layout->addWidget(addButton);
  layout->addWidget(delButton);
  layout->addWidget(passwordList);
  layout->addWidget(searchBox);
  layout->addWidget(topMenu);
  layout->addWidget(infoLabel);
  layout->addWidget(bottomFiller);

  connect(addButton, &QPushButton::clicked, this, &MainWindow::NewPassword);

  connect(delButton, &QPushButton::clicked, this, &MainWindow::RemovePassword);
}



void MainWindow::NewPassword(){
    QVector<DialogField> fields = {
        {"Site: ", QLineEdit::Normal},
        {"Username: ", QLineEdit::Normal},
        {"Password: ", QLineEdit::Password},
    };

    GenericDialog dlg("New Password", fields, false, this);
    if (dlg.exec() == QDialog::Accepted){
        PasswordEntry entry;
        entry.site = dlg.inputText(0);
        entry.username = dlg.inputText(1);
        entry.password = dlg.inputText(2);

        passwordList->addItem(entry.site);
    }
}
void MainWindow::RemovePassword(){
    const int row = passwordList->currentRow();
    if (row < 0) {
        return;
    }
    delete passwordList->takeItem(row);
}
void MainWindow::ExportDialog(){
    return;
}
void MainWindow::ImportDialog(){
    return;
}
