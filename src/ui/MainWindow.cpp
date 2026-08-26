#include "MainWindow.h"
#include "../core/PasswordUtils.h"
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
#include <QFontDatabase>
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
#include <qabstractitemview.h>
#include <qboxlayout.h>
#include <qcontainerfwd.h>
#include <qhashfunctions.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpicture.h>
#include <qpushbutton.h>
#include <qsize.h>
#include <qsizepolicy.h>

MainWindow::MainWindow(PasswordManager manager) : passwordManager(std::move(manager)) {
  setWindowTitle("Leaf's Password Manager");
  topMenu = new QMenuBar(this);

  createLayout();
  createActions();
  createMenus();

  for (const auto &entry : passwordManager.entries()) {
    addEntryToList(entry);
  }
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
  connect(editPasswordAct, &QAction::triggered, this, &MainWindow::editPasswordMenu);

  exportAct = new QAction("Export", this);
  connect(exportAct, &QAction::triggered, this, &MainWindow::ExportVault);

  importAct = new QAction("Import", this);
  connect(importAct, &QAction::triggered, this, &MainWindow::ImportVault);
}

void MainWindow::createMenus() {
  QFontDatabase::addApplicationFont(":/fonts/MonaspaceArgonNF-Medium.otf");
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

  mainLayout->addWidget(createLeftPanel());
  mainLayout->addWidget(createRightPanel());
}

//!!todo: panel creation and general UI might want to be moved in their separate folder, depending on how big it gets.
// ideally we'd want a header and cpp file purely to construct the MainWindow UI. its not really annoying as much
// as its a pita to do ctrl F all the time

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

  auto *buttonLayout = new QHBoxLayout;
  addButton          = new QPushButton("Add", leftWidget);
  addButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  delButton = new QPushButton("Delete", leftWidget);
  delButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  searchBox = new QLineEdit(leftWidget);

  buttonLayout->addWidget(searchBox, 3);
  buttonLayout->addWidget(addButton, 1);
  buttonLayout->addWidget(delButton, 1);
  passwordList = new QListWidget(leftWidget);
  passwordList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

  layout->setContentsMargins(5, 5, 5, 5);
  layout->addWidget(passwordList);
  layout->addWidget(bottomFiller);
  layout->addLayout(buttonLayout);

  connect(addButton, &QPushButton::clicked, this, &MainWindow::NewPassword);
  connect(delButton, &QPushButton::clicked, this, &MainWindow::RemovePassword);
  connect(searchBox, &QLineEdit::textChanged, this, &MainWindow::FilterPasswords);

  return leftWidget;
}

QWidget *MainWindow::createRightPanel() {
  rightWidget       = new QWidget;
  auto *rightLayout = new QVBoxLayout(rightWidget);

  rightWidget->setObjectName("editPanel");
  rightLayout->setContentsMargins(18, 18, 18, 18);
  rightLayout->setSpacing(8);

  siteEdit     = new QLineEdit;
  usernameEdit = new QLineEdit;
  emailEdit    = new QLineEdit;
  passwordEdit = new QLineEdit;
  passwordEdit->setEchoMode(QLineEdit::Password);

  rightLayout->addWidget(new QLabel("Site"));
  rightLayout->addWidget(siteEdit);

  rightLayout->addSpacing(12);

  rightLayout->addWidget(new QLabel("Username"));
  rightLayout->addWidget(usernameEdit);

  rightLayout->addSpacing(12);

  rightLayout->addWidget(new QLabel("Email"));
  rightLayout->addWidget(emailEdit);

  rightLayout->addSpacing(12);

  rightLayout->addWidget(new QLabel("Password"));
  rightLayout->addWidget(passwordEdit);

  rightLayout->addSpacing(12);

  auto *strengthLabel = new QLabel;
  rightLayout->addWidget(strengthLabel);

  connect(passwordEdit, &QLineEdit::textChanged, this, [this, strengthLabel](const QString &text) {
    QStringList context = {siteEdit->text(), usernameEdit->text(), emailEdit->text()};
    double      score   = PasswordUtils::estimateStrength(text, context);
    strengthLabel->setText(PasswordUtils::strengthLabel(score));
  });

  auto *genButtonLayout = new QHBoxLayout;
  auto *quickGenButton  = new QPushButton("Generate");
  auto *advGenButton    = new QPushButton("⚙");
  genButtonLayout->addWidget(quickGenButton);
  genButtonLayout->addWidget(advGenButton);
  rightLayout->addLayout(genButtonLayout);

  connect(quickGenButton, &QPushButton::clicked, this,
          [this]() { passwordEdit->setText(PasswordUtils::generatePassword(m_lastGenerationSettings)); });

  connect(advGenButton, &QPushButton::clicked, this, [this]() {
    DialogUtils::PasswordGenerationWindow dlg(this, m_lastGenerationSettings);
    if (dlg.exec() == QDialog::Accepted) {
      m_lastGenerationSettings = dlg.currentSettings();
      passwordEdit->setText(dlg.generatedPassword());
    }
  });

  rightLayout->addSpacing(13);

  auto *buttonLayout = new QHBoxLayout;
  auto *saveButton   = new QPushButton("Save");
  buttonLayout->addWidget(saveButton);
  auto *clearButton = new QPushButton("Clear");
  buttonLayout->addWidget(clearButton);
  auto *resetButton = new QPushButton("Reset");
  buttonLayout->addWidget(resetButton);
  auto *cancelButton = new QPushButton("Cancel");
  buttonLayout->addWidget(cancelButton);

  rightLayout->addLayout(buttonLayout);

  connect(saveButton, &QPushButton::clicked, this, &MainWindow::editPassword);
  connect(clearButton, &QPushButton::clicked, this, &MainWindow::clearEditPasswordFields);
  connect(resetButton, &QPushButton::clicked, this, &MainWindow::resetEditPasswordFields);
  connect(cancelButton, &QPushButton::clicked, this, &MainWindow::closeEditMenu);

  rightLayout->addStretch();

  rightWidget->hide();
  return rightWidget;
}

bool MainWindow::isEntryComplete(const PasswordEntry &entry, ValidationMode mode) const {
  if (mode == ValidationMode::Strict) {
    return !entry.site.isEmpty() && !entry.username.isEmpty() && !entry.email.isEmpty() && !entry.password.isEmpty();
  } else {
    return !entry.site.isEmpty() && !entry.password.isEmpty();
  }
}

void MainWindow::NewPassword() {
  QVector<DialogField> fields = {
      {"Site: ", QLineEdit::Normal},
      {"Username: ", QLineEdit::Normal},
      {"Email: ", QLineEdit::Normal},
      {"Password: ", QLineEdit::Password, true},
  };

  DialogUtils::GenericDialog dlg("New Password", fields, false, this);
  if (dlg.exec() != QDialog::Accepted) return;

  PasswordEntry entry;
  entry.site     = dlg.inputText(0);
  entry.username = dlg.inputText(1);
  entry.email    = dlg.inputText(2);
  entry.password = dlg.inputText(3);

  if (!isEntryComplete(entry, ValidationMode::Strict)) {
    QMessageBox::warning(this, "Missing Info",
                         "All fields are required. [This can be bypassed by editing the password after creation, but "
                         "site & password are strictly required.]");
    return;
  }
  if (passwordManager.isDuplicateEntry(entry.site, entry.username)) {
    int choice = DialogUtils::confirmationWindow(
        "Confirmation", "There already exists an entry with the same site and username. Add anyway?");
    if (choice != QMessageBox::Yes) {
      return;
    }
  }
  if (!passwordManager.addEntry(entry)) {
    return;
  }
  addEntryToList(entry);
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
  if (!passwordManager.removeEntry(row)) return;
  delete passwordList->takeItem(row);
}
void MainWindow::ExportVault() {
  DialogUtils::GenericDialog dlg("Export Vault", {{"Export to: ", QLineEdit::Normal}}, true, this);
  if (dlg.exec() != QDialog::Accepted) return;

  passwordManager.exportVault(dlg.inputText(0));
}
void MainWindow::ImportVault() {
  DialogUtils::GenericDialog dlg("Import Vault", {{"Import from: ", QLineEdit::Normal}}, true, this);
  if (dlg.exec() != QDialog::Accepted) return;

  if (!passwordManager.importVault(dlg.inputText(0))) return;

  passwordList->clear();

  for (const auto &entry : passwordManager.entries())
    addEntryToList(entry);
}

void MainWindow::FilterPasswords(const QString &text) {
  for (int i = 0; i < passwordList->count(); ++i) {
    QListWidgetItem *item    = passwordList->item(i);
    bool             matches = passwordManager.entries()[i].site.contains(text, Qt::CaseInsensitive);
    passwordList->item(i)->setHidden(!matches);
  }
}

void MainWindow::addEntryToList(const PasswordEntry &entry) {
  const int row = passwordList->count();

  QListWidgetItem *item      = new QListWidgetItem(passwordList);
  QWidget         *rowWidget = new QWidget;
  auto            *rowLayout = new QHBoxLayout(rowWidget);

  rowLayout->addWidget(new QLabel(entry.site), 2);
  rowLayout->addWidget(new QLabel(entry.username), 3);
  rowLayout->addWidget(new QLabel(entry.email.isEmpty() ? "N/A" : entry.email), 2);
  rowLayout->addWidget(new QLabel(QString(entry.password.length(), QChar(0x2022))),
                       2); // evil fancy • password dot again

  auto *copyButton = new QPushButton("Copy");
  connect(copyButton, &QPushButton::clicked, this,
          [this, row]() { Platform::WindowUtils::copyToClipboard(passwordManager.entries()[row]); });
  rowLayout->addWidget(copyButton);

  auto *revealButton = new QPushButton("👁");
  connect(revealButton, &QPushButton::clicked, this, [this, row]() { setRevealedRow(row); });
  rowLayout->addWidget(revealButton);

  rowLayout->setContentsMargins(4, 2, 4, 2);
  item->setSizeHint(rowWidget->sizeHint() + QSize(40, 20));
  passwordList->setItemWidget(item, rowWidget);
}

void MainWindow::refreshPasswordEntry(int row) {
  if (row < 0 || row >= passwordList->count()) return;

  QListWidgetItem *item      = passwordList->item(row);
  QWidget         *rowWidget = passwordList->itemWidget(item);
  if (!rowWidget) return;

  const PasswordEntry &entry     = passwordManager.entries()[row];
  QHBoxLayout         *rowLayout = qobject_cast<QHBoxLayout *>(rowWidget->layout());
  if (!rowLayout) return;

  // what the fuck
  static_cast<QLabel *>(rowLayout->itemAt(0)->widget())->setText(entry.site);
  static_cast<QLabel *>(rowLayout->itemAt(1)->widget())->setText(entry.username);
  static_cast<QLabel *>(rowLayout->itemAt(2)->widget())->setText(entry.email.isEmpty() ? "N/A" : entry.email);

  bool revealed = (row == m_revealedRow);
  static_cast<QLabel *>(rowLayout->itemAt(3)->widget())
      ->setText(revealed ? entry.password : QString(entry.password.length(), QChar(0x2022))); // fancy password dot •
}

void MainWindow::setRevealedRow(int row) {
  m_revealedRow = (m_revealedRow == row) ? -1 : row;
  for (int i = 0; i < passwordList->count(); ++i) {
    refreshPasswordEntry(i);
  }
}

void MainWindow::editPassword() {
  if (editingRow < 0 || editingRow >= passwordManager.entries().size()) {
    return;
  }

  PasswordEntry entry;

  entry.site     = siteEdit->text();
  entry.username = usernameEdit->text();
  entry.email    = emailEdit->text();
  entry.password = passwordEdit->text();
  if (!isEntryComplete(entry, ValidationMode::Loose)) {
    QMessageBox::warning(this, "Missing Info", "Site and password cannot be empty.");
    return;
  }
  if (!passwordManager.updateEntry(editingRow, entry)) return;
  refreshPasswordEntry(editingRow);
  closeEditMenu();
}

void MainWindow::editPasswordMenu() {
  const int row = passwordList->currentRow();
  if (row < 0) return;

  editingRow = row;

  siteEdit->setText(passwordManager.entries()[row].site);
  usernameEdit->setText(passwordManager.entries()[row].username);
  emailEdit->setText(passwordManager.entries()[row].email);
  passwordEdit->setText(passwordManager.entries()[row].password);

  rightWidget->show();
}

void MainWindow::clearEditPasswordFields() {
  siteEdit->clear();
  usernameEdit->clear();
  emailEdit->clear();
  passwordEdit->clear();
}

void MainWindow::resetEditPasswordFields() {
  if (editingRow < 0 || editingRow >= passwordManager.entries().size()) {
    return;
  }

  const PasswordEntry &entry = passwordManager.entries()[editingRow];

  siteEdit->setText(entry.site);
  usernameEdit->setText(entry.username);
  emailEdit->setText(entry.email);
  passwordEdit->setText(entry.password);
}

void MainWindow::closeEditMenu() {
  clearEditPasswordFields();
  editingRow = -1;
  rightWidget->hide();
}
