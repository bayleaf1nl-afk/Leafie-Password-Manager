cmake_minimum_required(VERSION 3.21)

project(PasswordManager)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g -fsanitize=address,undefined")

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SODIUM REQUIRED libsodium)



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
    src/core/FileUtils.h
    src/core/PasswordUtils.h

    src/vault/PasswordEntry.cpp
    src/core/LoginGate.cpp
    src/ui/GenericDialog.cpp
    src/ui/DialogUtils.cpp
    src/ui/MainWindow.cpp
    src/platform/WindowManager_standard.cpp
    src/platform/WindowManager_tiling.cpp
    src/platform/WindowUtils.cpp
    src/vault/PasswordManager.cpp
    src/core/FileUtils.cpp
    src/core/PasswordUtils.cpp
    ${RESOURCES}
)

target_link_libraries(PasswordManager PRIVATE Qt6::Widgets)
target_link_libraries(PasswordManager PRIVATE ${SODIUM_LIBRARIES})
target_include_directories(PasswordManager PRIVATE ${SODIUM_INCLUDE_DIRS})
target_include_directories(PasswordManager PRIVATE third_party/zxcvbn-cpp/native-src)
file(GLOB ZXCVBN_SOURCES third_party/zxcvbn-cpp/native-src/zxcvbn/*.cpp)
list(FILTER ZXCVBN_SOURCES EXCLUDE REGEX ".*js.*\\.cpp$")
message(STATUS "ZXCVBN_SOURCES: ${ZXCVBN_SOURCES}")
target_sources(PasswordManager PRIVATE ${ZXCVBN_SOURCES})
set_source_files_properties(${ZXCVBN_SOURCES} PROPERTIES CXX_STANDARD 14)
