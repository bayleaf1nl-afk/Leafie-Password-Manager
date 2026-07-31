#include "MainWindow.h"
#include "PasswordEntry.h"
#include "PasswordManager.h"
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
#include <qcursor.h>
#include <qframe.h>
#include <qmainwindow.h>
#include <qmenubar.h>
#include <qnamespace.h>
#include <qpaintdevice.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qtimer.h>
#include <qwidget.h>
#include <QProcess>
#include <QTimer>
#include "platform/WindowManager.h"
#include <QScreen>

MainWindow::MainWindow() {
  setWindowTitle("Leaf's Password Manager");
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
  passwordList = new QListWidget(this);
  searchBox = new QLineEdit(this);

  layout->setContentsMargins(5, 5, 5, 5);
  layout->addWidget(addButton);
  layout->addWidget(delButton);
  layout->addWidget(passwordList);
  layout->addWidget(searchBox);
  layout->addWidget(topMenu);
  layout->addWidget(infoLabel);
  layout->addWidget(bottomFiller);

  connect(addButton, &QPushButton::clicked, this, &MainWindow::NewPassword);

  connect(delButton, &QPushButton::clicked, this, &MainWindow::RemovePassword);
}

int main(int argc, char *argv[])
{   
    QApplication app(argc, argv);
    app.setApplicationName("PasswordManager");
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