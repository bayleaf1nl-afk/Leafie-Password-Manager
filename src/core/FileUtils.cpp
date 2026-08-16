// core/FileUtils.cpp
#include "FileUtils.h"
#include <QDir>
#include <QStandardPaths>

QString FileUtils::appDataPath() {
  QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir().mkpath(path); // create it if it doesn't exist yet — first launch won't have it
  return path;
}

QString FileUtils::vaultPath() { return appDataPath() + "/vault.json"; }
QString FileUtils::masterFilePath() { return appDataPath() + "/master.txt"; }
