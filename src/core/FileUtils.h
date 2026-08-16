// core/FileUtils.h
#pragma once
#include <QString>

namespace FileUtils {
QString appDataPath();    // resolves + ensures the app data directory exists
QString vaultPath();      // appDataPath() + "/vault.json"
QString masterFilePath(); // appDataPath() + "/master.txt"
} // namespace FileUtils
