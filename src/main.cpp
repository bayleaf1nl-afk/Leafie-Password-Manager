#include "LoginGate.h"
#include "MainWindow.h"
#include "PasswordEntry.h"
#include "PasswordManager.h"
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

int main(int argc, char *argv[])
{   
    QApplication app(argc, argv);
    app.setApplicationName("PasswordManager");

    QString masterPassword;
    if(!LoginGate::authenticate(masterPassword)){
        return 0;
    }

    MainWindow window;
    QFile file(":/styles.qss");
    

    if (file.open(QFile::ReadOnly))
        app.setStyleSheet(file.readAll());
    else
        qWarning() << file.errorString();

    QTimer::singleShot(
        10,
    [&window]
    {
        if (Platform::TilingWM::available()) 
        {
            Platform::TilingWM::requestFloating(&window);
        }
        else
        {
            Platform::StandardWM::requestFloating(&window);
        }
    });
    
    window.resize(800, 600);
    window.show();
    Platform::WindowUtils::centerWindow(&window);
    return app.exec();
}