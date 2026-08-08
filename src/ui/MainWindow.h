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
#include <qaction.h>
#include <qcontainerfwd.h>
#include <qlineedit.h>
#include <qtmetamacros.h>
#include <qwidget.h>

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

  int                    editingRow   = -1;
  PasswordEntry         *editingEntry = nullptr;
  QLineEdit             *passwordEdit = nullptr;
  QLineEdit             *emailEdit    = nullptr;
  QLineEdit             *usernameEdit = nullptr;
  QLineEdit             *siteEdit     = nullptr;
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
  QAction               *deletePasswordNoConfirmAct;
  QAction               *editPasswordAct;
  QAction               *loginAct;
  QAction               *exportAct;
  QAction               *importAct;
  QLabel                *infoLabel;
  QVector<PasswordEntry> entries;
  QWidget               *rightWidget;
  QWidget               *windowWrapper;
  //------------//

  void     createActions();
  void     createMenus();
  void     createLayout();
  void     showAboutDialog();
  void     SaveVault();
  void     LoadVault();
  void     addEntryToList(const PasswordEntry &entry);
  bool     isDuplicateEntry(const QString &site, const QString &username) const;
  void     editPasswordMenu();
  void     createFileMenu();
  void     createEditMenu();
  void     closeEditMenu();
  void     createSettingsMenu();
  void     createHelpMenu();
  QWidget *createLeftPanel();
  QWidget *createRightPanel();
  void     setupFramelessWindow();

private slots:
  void NewPassword();
  void RemovePassword(bool forceDelete = false);
  void ExportVault();
  void ImportVault();
  void FilterPasswords(const QString &text);
  void clearEditPasswordFields();
  void resetEditPasswordFields();
  void editPassword();
};