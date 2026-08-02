#include "LoginGate.h"
#include "GenericDialog.h"
#include <QLineEdit>
#include <QDialog>
#include <qdialog.h>
#include <qfiledevice.h>
#include <qlineedit.h>
#include <qlogging.h>
#include <qmessagebox.h>
#include <qwindowdefs.h>
#include <QFile>
#include <QTimer>
#include "platform/WindowManager.h"
bool LoginGate::authenticate(QString &outMasterPassword){
    QFile file("master.txt");
    bool isFirstLaunch = !file.exists();
    QString title = isFirstLaunch ? "Set Master Password" : "Login";
    QString label = isFirstLaunch ? "Make New Master Password:" : "Enter Master Password:";

    if (isFirstLaunch){
        GenericDialog dialog(title, {{label, QLineEdit::Password}}, false, nullptr);
        if (dialog.exec() != QDialog::Accepted) return false;
        Platform::attemptFloating(&dialog);
        outMasterPassword = dialog.inputText(0);
        if (!file.open(QIODevice::WriteOnly)) { QMessageBox::critical(nullptr, "Error", "Could not write to master password file"); return false; }
        
        file.write(outMasterPassword.toUtf8());
        return true;
    }

    if (!file.open(QIODevice::ReadOnly)) { QMessageBox::critical(nullptr, "Error", "Could not read from master password file"); return false; }
    QString stored = QString::fromUtf8(file.readAll());

    const int maxAttempts = 3;
    for (int attempt = 1; attempt <= maxAttempts; ++attempt){
        QString label = QString("Master Password (%1/%2 attempts): ").arg(attempt).arg(maxAttempts);
        GenericDialog dlg("Login", {{label, QLineEdit::Password}}, false);

        if (dlg.exec() != QDialog::Accepted) return false;
        if (dlg.inputText(0) == stored) return true;
    }

    QMessageBox::critical(nullptr, "Login Failed", "Too many incorrect attempts.");
    return false;
}