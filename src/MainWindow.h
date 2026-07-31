#pragma once
#include <QMainWindow>
#include <QWidget>
#include <qaction.h>
#include <qlabel.h>
#include <qmainwindow.h>
#include <qtmetamacros.h>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QLabel>
#include <QDialog>

namespace Utilities {
    QString generatePassword();
    bool isStrongPassword();
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        MainWindow();

    private:
        void createActions();
        void createMenus();
        QPushButton* addButton;
        QPushButton* delButton;
        QListWidget* passwordList;
        QLineEdit* searchBox;
        QMenuBar* topMenu;
        QMenu *fileMenu;
        QMenu *editMenu;
        QMenu *formatMenu;
        QMenu *helpMenu;
        QActionGroup *alignmentGroup;
        QAction *newPasswordAct;
        QAction *deletePasswordAct;
        QAction *loginAct;
        QAction *exportAct;
        QAction *importAct;
        QLabel *infoLabel;
        //------------//
    private slots:
        void NewPassword();
        void RemovePassword();
        void OpenLoginDialog();
        void ExportVault();
        void ImportVault();

};