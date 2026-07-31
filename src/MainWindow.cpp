#include "MainWindow.h"
#include <QMainWindow>
#include <QWidget>
#include <QObject>
#include <QDialog>



void MainWindow::NewPassword(){
    passwordList->addItem(". . .");
}
void MainWindow::RemovePassword(){
    const int row = passwordList->currentRow();
    if (row < 0) {
        return;
    }
    delete passwordList->takeItem(row);
}
void MainWindow::OpenLoginDialog(){
    return;
}
void MainWindow::ExportVault(){
    return;
}
void MainWindow::ImportVault(){
    return;
}
