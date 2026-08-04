#pragma once
#include <QWidget>
#include "../PasswordEntry.h"
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
        void copyToClipboard(const PasswordEntry &entry);
    }


} // namespace Platform
