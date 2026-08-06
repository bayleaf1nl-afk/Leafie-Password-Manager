#include "MainWindow.h"
#include "../core/LoginGate.h"
#include "../platform/WindowManager.h"
#include "../vault/PasswordEntry.h"
#include "../vault/PasswordManager.h"
#include "DialogUtils.h"
#include "GenericDialog.h"
//---------------------------//
#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QClipboard>
#include <QCoreApplication>
#include <QDebug>
#include <QDialog>
#include <QFile>
#include <QFileDevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QKeySequence>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QScreen>
#include <QSize>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <Qt>
#include <qcontainerfwd.h>
#include <qhashfunctions.h>
#include <qlabel.h>
#include <qlogging.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qnamespace.h>
#include <qpicture.h>
#include <qwindowdefs.h>

MainWindow::MainWindow() {
  setWindowTitle("Leaf's Password Manager");
  topMenu = new QMenuBar(this);
  createLayout();
  createActions();
  createMenus();
  LoadVault();
}

void MainWindow::createActions() {
  newPasswordAct = new QAction("Add Password", this);
  newPasswordAct->setShortcut(QKeySequence("Ctrl + N"));
  connect(newPasswordAct, &QAction::triggered, this, &MainWindow::NewPassword);

  deletePasswordAct = new QAction("Delete Password", this);
  deletePasswordAct->setShortcut(QKeySequence::Delete);
  connect(deletePasswordAct, &QAction::triggered, this, &MainWindow::RemovePassword);

  deletePasswordNoConfirmAct = new QAction(this);
  deletePasswordNoConfirmAct->setShortcut(QKeySequence(Qt::SHIFT | Qt::Key_Delete));
  addAction(deletePasswordNoConfirmAct);
  connect(deletePasswordNoConfirmAct, &QAction::triggered, this, [this] { RemovePassword(true); });

  editPasswordAct = new QAction("Edit Password", this);
  editPasswordAct->setShortcut(QKeySequence("Ctrl + E"));
  connect(editPasswordAct, &QAction::triggered, this, [this]() {
    const int row = passwordList->currentRow();
    if (row < 0) return; // fuck you

    editPassword(entries[row]);
  });

  exportAct = new QAction("Export", this);
  connect(exportAct, &QAction::triggered, this, &MainWindow::ExportVault);

  importAct = new QAction("Import", this);
  connect(importAct, &QAction::triggered, this, &MainWindow::ImportVault);
}

void MainWindow::createMenus() {
  createEditMenu();
  createFileMenu();
  createSettingsMenu();
  createHelpMenu();
}

void MainWindow::createFileMenu() {
  fileMenu = menuBar()->addMenu("File");

  fileMenu->addAction(exportAct);
  fileMenu->addAction(importAct);
  fileMenu->addSeparator();
  fileMenu->addAction("Quit", this, &QCoreApplication::quit);
}

void MainWindow::createEditMenu() {
  editMenu = menuBar()->addMenu("Edit");

  editMenu->addAction(newPasswordAct);
  editMenu->addAction(deletePasswordAct);
  editMenu->addAction(editPasswordAct);
}

void MainWindow::createSettingsMenu() { settingsMenu = menuBar()->addMenu("Settings"); }

void MainWindow::createHelpMenu() {
  helpMenu = menuBar()->addMenu("Help");

  helpMenu->addAction("About", this, &MainWindow::showAboutDialog);
}

void MainWindow::showAboutDialog() {
  QMessageBox::information(this, "About", "gng u cant be serious its a PASSWORD MANAGER");
}

void MainWindow::createLayout() {
  auto *central = new QWidget;
  setCentralWidget(central);
  auto *mainLayout = new QHBoxLayout(central);
  mainLayout->addWidget(infoLabel);

  mainLayout->addWidget(createLeftPanel());
  mainLayout->addWidget(createRightPanel());
}

QWidget *MainWindow::createLeftPanel() {

  auto *leftWidget   = new QWidget;
  auto *layout       = new QVBoxLayout(leftWidget);
  auto *bottomFiller = new QWidget;
  auto *header       = new QWidget(this);
  auto *headerLayout = new QHBoxLayout(header);

  headerLayout->addWidget(new QLabel("<b>Site</b>"), 2);
  headerLayout->addWidget(new QLabel("<b>Username</b>"), 2);
  headerLayout->addWidget(new QLabel("<b>Email</b>"), 2);
  headerLayout->addWidget(new QLabel("<b>Password</b>"), 2);
  headerLayout->addWidget(new QLabel(""), 1); // spacer matching the Copy button column

  headerLayout->setContentsMargins(4, 2, 4, 2);
  layout->addWidget(header);
  bottomFiller->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  addButton    = new QPushButton("Add", leftWidget);
  delButton    = new QPushButton("Delete", leftWidget);
  passwordList = new QListWidget(leftWidget);
  searchBox    = new QLineEdit(leftWidget);

  layout->setContentsMargins(5, 5, 5, 5);
  layout->addWidget(passwordList);
  layout->addWidget(searchBox);
  layout->addWidget(bottomFiller);
  layout->addWidget(addButton);
  layout->addWidget(delButton);

  connect(addButton, &QPushButton::clicked, this, &MainWindow::NewPassword);
  connect(delButton, &QPushButton::clicked, this, &MainWindow::RemovePassword);
  connect(searchBox, &QLineEdit::textChanged, this, &MainWindow::FilterPasswords);

  return leftWidget;
}

QWidget *MainWindow::createRightPanel() {
  auto *rightWidget = new QWidget;
  auto  leftLayout  = new QVBoxLayout;

  // todo; edit panel will live here most likely
  return rightWidget;
}

void MainWindow::NewPassword() {
  QVector<DialogField> fields = {
      {"Site: ", QLineEdit::Normal},
      {"Username: ", QLineEdit::Normal},
      {"Password: ", QLineEdit::Password},
  };

  DialogUtils::GenericDialog dlg("New Password", fields, false, this);
  if (dlg.exec() == QDialog::Accepted) {
    PasswordEntry entry;
    entry.site     = dlg.inputText(0);
    entry.username = dlg.inputText(1);
    entry.password = dlg.inputText(2);
    if (MainWindow::isDuplicateEntry(entry.site, entry.username)) {
      int choice = DialogUtils::confirmationWindow(
          "Confirmation", "There already exists an entry with the same site and username. Add anyway?");
      if (choice == QMessageBox::Yes) {
        entries.push_back(entry);
        addEntryToList(entry);
        SaveVault();
      } else {
        return;
      }
    }
  }
}

void MainWindow::RemovePassword(bool forceDelete) {
  const int row = passwordList->currentRow();
  if (row < 0) {
    return;
  }
  if (!forceDelete) {
    int choice = DialogUtils::confirmationWindow(QString("Confirmation"),
                                                 QString("Are you sure you want to delete this entry?"));

    if (choice != QMessageBox::Yes) {
      return;
    }
  }
  delete passwordList->takeItem(row);
  entries.remove(row);
  SaveVault();
}
void MainWindow::ExportVault() {
  DialogUtils::GenericDialog dlg("Export Vault", {{"Export to: ", QLineEdit::Normal}}, true, this);
  if (dlg.exec() != QDialog::Accepted) return;

  QString path = dlg.inputText(0);

  QJsonArray array;
  for (const auto &entry : entries) {
    array.append(entry.toJson());
  }
  QJsonDocument doc(array);

  QFile file(path);
  if (file.open(QIODevice::WriteOnly)) {
    file.write(doc.toJson());
  }
}
void MainWindow::ImportVault() {
  DialogUtils::GenericDialog dlg("Import Vault", {{"Import from: ", QLineEdit::Normal}}, true, this);
  if (dlg.exec() != QDialog::Accepted) return;

  QString path = dlg.inputText(0);
  QFile   file(path);
  if (!file.open(QIODevice::ReadOnly)) return;
  QJsonDocument doc   = QJsonDocument::fromJson(file.readAll());
  QJsonArray    array = doc.array();

  for (const auto &var : array) {
    PasswordEntry entry = PasswordEntry::fromJson(var.toObject());
    entries.push_back(entry);
    addEntryToList(entry);
  }
  SaveVault();
}
void MainWindow::SaveVault() {
  QJsonArray array;
  for (const auto &entry : entries) {
    array.append(entry.toJson());
  }
  QJsonDocument doc(array);

  QFile file("vault.json");
  if (file.open(QIODevice::WriteOnly)) {
    file.write(doc.toJson());
  }
}

void MainWindow::LoadVault() {
  QFile file("vault.json");
  if (!file.open(QIODevice::ReadOnly)) {
    qWarning() << "Could not open file: ";
    return;
  };
  QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
  if (doc.isNull()) {
    qWarning() << "Vault file is corrupted or not valid JSON";
    return;
  }
  QJsonArray array = doc.array();

  for (const auto &val : array) {
    PasswordEntry entry = PasswordEntry::fromJson(val.toObject());
    entries.push_back(entry);
    addEntryToList(entry);
  }
}

void MainWindow::FilterPasswords(const QString &text) {
  for (int i = 0; i < passwordList->count(); ++i) {
    QListWidgetItem *item    = passwordList->item(i);
    bool             matches = entries[i].site.contains(text, Qt::CaseInsensitive);
    passwordList->item(i)->setHidden(!matches);
  }
}

void MainWindow::addEntryToList(const PasswordEntry &entry) {
  QListWidgetItem *item      = new QListWidgetItem(passwordList);
  QWidget         *rowWidget = new QWidget;
  auto            *rowLayout = new QHBoxLayout(rowWidget);

  rowLayout->addWidget(new QLabel(entry.site), 2);
  rowLayout->addWidget(new QLabel(entry.username), 2);
  rowLayout->addWidget(new QLabel(entry.email.isEmpty() ? "N/A" : entry.email), 2);
  rowLayout->addWidget(new QLabel(QString(entry.password.length(), '*')), 2);

  auto *copyButton = new QPushButton("Copy");
  connect(copyButton, &QPushButton::clicked, this, [entry]() { Platform::WindowUtils::copyToClipboard(entry); });
  rowLayout->addWidget(copyButton);

  rowLayout->setContentsMargins(4, 2, 4, 2);
  item->setSizeHint(rowWidget->sizeHint() + QSize(40, 20));
  passwordList->addItem(item);
  passwordList->setItemWidget(item, rowWidget);
}

void MainWindow::editPassword(PasswordEntry &entry) { /*has to be called by editPasswordMenu*/ }

bool MainWindow::isDuplicateEntry(const QString &site, const QString &username) const {
  for (const PasswordEntry &entry : entries) {
    if (entry.site == site && entry.username == username) return true;
  }
  return false;
}

void MainWindow::editPasswordMenu() {
  auto *passMenu = new QWidget;
  passMenu->hide();

  // todo: stub, make this work
}