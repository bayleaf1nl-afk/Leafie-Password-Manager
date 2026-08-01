#include "LoginGate.h"
#include "GenericDialog.h"
#include <QLineEdit>
#include <QDialog>
#include <qfiledevice.h>
#include <qlineedit.h>
#include <qlogging.h>
#include <qmessagebox.h>
#include <qwindowdefs.h>
#include <QFile>

bool LoginGate::authenticate(QString &outMasterPassword){
    QFile file("master.txt");
    bool isFirstLaunch = !file.exists();
    QString title = isFirstLaunch ? "Set Master Password" : "Login";
    QString label = isFirstLaunch ? "Make New Master Password:" : "Enter Master Password:";
    GenericDialog dialog(title, {{label, QLineEdit::Password}}, false, nullptr);

    if (dialog.exec() != QDialog::Accepted) return false; //mf pressed the cancel button like a LOSER RAHHHH
    if (isFirstLaunch){
        outMasterPassword = dialog.inputText(0);
        if (!file.open(QIODevice::WriteOnly)) { QMessageBox::critical(nullptr, "Error", "Could not write to master password file"); return false; }
        
        file.write(outMasterPassword.toUtf8());
        return true;
    }
    bool readFile = file.open(QIODevice::ReadOnly);
    if (!file.open(QIODevice::ReadOnly)) { QMessageBox::critical(nullptr, "Error", "Could not read from master password file"); return false; }
    QString stored = QString::fromUtf8(file.readAll());
    return dialog.inputText(0) == stored;
}