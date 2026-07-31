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
#include <QDialogButtonBox>

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
        QLineEdit *genericInput;
        QDialogButtonBox *genericButtonBox;
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
        void ExportDialog();
        void ImportDialog();

};