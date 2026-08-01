#include "MainWindow.h"
#include "LoginGate.h"
#include "PasswordEntry.h"
#include "PasswordManager.h"
#include "GenericDialog.h"
#include "platform/WindowManager.h"
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
#include <qaction.h>
#include <qcoreapplication.h>
#include <qfiledevice.h>
#include <qjsonarray.h>
#include <qjsondocument.h>
#include <qkeysequence.h>
#include <qlineedit.h>
#include <qlistwidget.h>
#include <qlogging.h>
#include <qnamespace.h>
#include <qpicture.h>
#include <qpushbutton.h>
#include <qwindowdefs.h>
#include <QMessageBox>

MainWindow::MainWindow() {
  setWindowTitle("Leaf's Password Manager");
  createLayout();
  createActions();
  createMenus();

  LoadVault();
}



void MainWindow::createActions(){
    newPasswordAct = new QAction("Add Password", this);
    newPasswordAct->setShortcut(QKeySequence("Ctrl + N"));
    connect(newPasswordAct, &QAction::triggered, this, &MainWindow::NewPassword);

    deletePasswordAct = new QAction("Delete Password", this);
    deletePasswordAct->setShortcut(QKeySequence::Delete);
    connect(deletePasswordAct, &QAction::triggered, this, &MainWindow::RemovePassword);

    exportAct = new QAction("Export", this);
    connect(exportAct, &QAction::triggered, this, &MainWindow::ExportVault);

    importAct = new QAction("Import", this);
    connect(importAct, &QAction::triggered, this, &MainWindow::ImportVault);
}

void MainWindow::createMenus(){
    fileMenu = topMenu->addMenu("File");
    editMenu = topMenu->addMenu("Edit");
    settingsMenu = topMenu->addMenu("Settings");
    helpMenu = topMenu->addMenu("Help");

    //--------------------------------//

    fileMenu->addAction(exportAct);
    fileMenu->addAction(importAct);
    fileMenu->addSeparator();
    fileMenu->addAction("Quit", this, &QCoreApplication::quit);

    editMenu->addAction(newPasswordAct);
    editMenu->addAction(deletePasswordAct);

    helpMenu->addAction("About", this, &MainWindow::showAboutDialog);

    setMenuBar(topMenu);
    
}

void MainWindow::showAboutDialog(){
    QMessageBox::information(this, "About", "gng u cant be serious its a PASSWORD MANAGER");
}

void MainWindow::createLayout(){
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
  importButton = new QPushButton("Load", this);
  exportButton = new QPushButton("Save & Export", this);
  passwordList = new QListWidget(this);
  searchBox = new QLineEdit(this);

  layout->setContentsMargins(5, 5, 5, 5);
  layout->addWidget(passwordList);
  layout->addWidget(searchBox);
  layout->addWidget(topMenu);
  layout->addWidget(infoLabel);
  layout->addWidget(bottomFiller);

  connect(addButton, &QPushButton::clicked, this, &MainWindow::NewPassword);
  connect(delButton, &QPushButton::clicked, this, &MainWindow::RemovePassword);
  connect(exportButton, &QPushButton::clicked, this, &MainWindow::ExportVault);
  connect(importButton, &QPushButton::clicked, this, &MainWindow::ImportVault);
  connect(searchBox, &QLineEdit::textChanged, this, &MainWindow::FilterPasswords);
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
        
        entries.push_back(entry);
        passwordList->addItem(entry.site);
        SaveVault();
    }
}
void MainWindow::RemovePassword(){
    const int row = passwordList->currentRow();
    if (row < 0) {
        return;
    }
    delete passwordList->takeItem(row);
    entries.remove(row);
    SaveVault();
}
void MainWindow::ExportVault(){
    GenericDialog dlg("Export Vault", {{"Export to: ", QLineEdit::Normal}}, true, this);
    if (dlg.exec() != QDialog::Accepted) return;

    QString path = dlg.inputText(0);

    QJsonArray array;
    for (const auto &entry : entries){
        array.append(entry.toJson());
    }
    QJsonDocument doc(array);

    QFile file(path);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
    }
}
void MainWindow::ImportVault(){
    GenericDialog dlg("Import Vault", {{"Import to: ", QLineEdit::Normal}}, true, this);
    if (dlg.exec() != QDialog::Accepted) return;
    
    QString path = dlg.inputText(0);
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) return;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonArray array = doc.array();

    for (const auto &var : array){
        PasswordEntry entry = PasswordEntry::fromJson(var.toObject());
        entries.push_back(entry);
        passwordList->addItem(entry.site);
    }
    SaveVault();
}
void MainWindow::SaveVault(){
    QJsonArray array;
    for (const auto &entry: entries){
        array.append(entry.toJson());
    }
    QJsonDocument doc(array);

    QFile file("vault.json");
    if (file.open(QIODevice::WriteOnly)){
        file.write(doc.toJson());
    }
}
void MainWindow::LoadVault(){
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

    for (const auto &val : array){
        PasswordEntry entry = PasswordEntry::fromJson(val.toObject());
        entries.push_back(entry);
        passwordList->addItem(entry.site);
    }
}

void MainWindow::FilterPasswords(const QString &text){
    for (int i = 0; i < passwordList->count(); ++i){
        QListWidgetItem *item = passwordList->item(i);
        bool matches = item->text().contains(text, Qt::CaseInsensitive);
        item->setHidden(!matches);
    }
}