#include "WindowManager.h"
#include <QApplication>
#include <QScreen>

namespace Platform::WindowUtils {
    void centerWindow(QWidget* window){
        QScreen* screen = window->screen(); 
        if (!screen) screen = QApplication::primaryScreen();
        if (!screen) return;

        QRect geometry = screen->availableGeometry();
        window->move(geometry.center() - window->rect().center());
    }
}