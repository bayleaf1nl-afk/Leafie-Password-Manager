#pragma once
#include <QWidget>

namespace Platform {
    void attemptFloating(QWidget *window);

    namespace StandardWM {
        void requestFloating(QWidget *);
    }
    
    namespace TilingWM {
        void requestFloating(QWidget *);
        bool available();
    }
    namespace WindowUtils{
        void centerWindow(QWidget* window);
    }


} // namespace Platform
