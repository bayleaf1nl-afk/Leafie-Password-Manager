#include "WindowManager.h"
#include <QApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <qdebug.h>
#include <qtenvironmentvariables.h>
namespace Platform::TilingWM {

bool available() {
  return qEnvironmentVariableIsSet("HYPRLAND_INSTANCE_SIGNATURE");
}

void requestFloating(QWidget *window) {
  QProcess proc;
  proc.start("hyprctl", {"clients", "-j"});
  proc.waitForFinished();

  auto json = QJsonDocument::fromJson(proc.readAllStandardOutput());

  qDebug() << QApplication::applicationName();

  for (auto value : json.array()) {
    auto obj = value.toObject();
    qDebug() << obj["class"].toString() << obj["address"].toString();
  }

  for (auto value : json.array()) {
    auto obj = value.toObject();

    if (obj["class"].toString() == QApplication::applicationName()) {

      QString address = obj["address"].toString();

      QProcess::startDetached(
          "hyprctl",
          {"dispatch", QString("hl.dispatch(hl.dsp.window.float({ action = "
                               "\"set\", window = \"address:%1\" }))")
                           .arg(address)});

      return;
    }
  }
}

} // namespace Platform::TilingWM
