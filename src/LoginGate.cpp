#include "LoginGate.h"
#include "GenericDialog.h"
#include <QLineEdit>
#include <QDialog>
#include <qlineedit.h>
#include <qwindowdefs.h>

bool LoginGate::authenticate(QString &outMasterPassword){
    bool isFirstLaunch = true; //temp hardcoded
    QString title = isFirstLaunch ? "Set Master Password" : "Login";
    QString label = isFirstLaunch ? "Make New Master Password:" : "Enter Master Password:";
    GenericDialog dialog(title, {{label, QLineEdit::Password}}, false, nullptr);

    if (dialog.exec() != QDialog::Accepted) return false; //mf pressed the cancel button like a LOSER RAHHHH
    if (isFirstLaunch){
        outMasterPassword = dialog.inputText(0);
        return true;
    }

    return dialog.inputText(0) == outMasterPassword;
}