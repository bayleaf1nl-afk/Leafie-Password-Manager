cmake_minimum_required(VERSION 3.21)

project(PasswordManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address,undefined")

find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_resources(RESOURCES
    resources.qrc
)

qt_add_executable(PasswordManager
    src/main.cpp

    src/ui/MainWindow.h
    src/vault/PasswordEntry.h
    src/vault/PasswordManager.h
    src/ui/GenericDialog.h
    src/core/LoginGate.h
    src/platform/WindowManager.h
    src/ui/DialogUtils.h

    src/vault/PasswordEntry.cpp
    src/core/LoginGate.cpp
    src/ui/GenericDialog.cpp
    src/ui/DialogUtils.cpp
    src/ui/MainWindow.cpp
    src/platform/WindowManager_standard.cpp
    src/platform/WindowManager_tiling.cpp
    src/platform/WindowUtils.cpp
    ${RESOURCES}
)

target_link_libraries(PasswordManager PRIVATE Qt6::Widgets)
