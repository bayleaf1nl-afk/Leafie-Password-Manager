#pragma once
#include "../vault/PasswordEntry.h"
#include <QAction>
#include <QBoxLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QObject>
#include <QPushButton>
#include <QWidget>
#include <qcontainerfwd.h>
#include <qtmetamacros.h>

namespace Utilities {
QString generatePassword();
bool    isStrongPassword();
} // namespace Utilities

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow();

private:
  //-------------//
  QLineEdit *website;
  QLineEdit *username;
  QLineEdit *password;

  QLineEdit             *genericInput;
  QDialogButtonBox      *genericButtonBox;
  QPushButton           *addButton;
  QPushButton           *delButton;
  QListWidget           *passwordList;
  QLineEdit             *searchBox;
  QMenuBar              *topMenu;
  QMenu                 *fileMenu;
  QMenu                 *editMenu;
  QMenu                 *settingsMenu;
  QMenu                 *helpMenu;
  QAction               *newPasswordAct;
  QAction               *deletePasswordAct;
  QAction               *editPasswordAct;
  QAction               *loginAct;
  QAction               *exportAct;
  QAction               *importAct;
  QLabel                *infoLabel;
  QVector<PasswordEntry> entries;

  //------------//

  void     createActions();
  void     createMenus();
  void     createLayout();
  void     showAboutDialog();
  void     SaveVault();
  void     LoadVault();
  void     addEntryToList(const PasswordEntry &entry);
  void     editPassword(PasswordEntry &entry);
  bool     isDuplicateEntry(const QString &site, const QString &username) const;
  void     editPasswordMenu();
  void     createFileMenu();
  void     createEditMenu();
  void     createSettingsMenu();
  void     createHelpMenu();
  QWidget *createLeftPanel();
  QWidget *createRightPanel();

private slots:
  void NewPassword();
  void RemovePassword();
  void ExportVault();
  void ImportVault();
  void FilterPasswords(const QString &text);
};