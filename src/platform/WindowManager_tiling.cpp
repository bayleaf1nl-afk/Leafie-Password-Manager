#include "WindowManager.h"
#include <QApplication>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <qtenvironmentvariables.h>
namespace Platform::TilingWM {

bool available() { return qEnvironmentVariableIsSet("HYPRLAND_INSTANCE_SIGNATURE"); }

void requestFloating(QWidget *window) {
  if (!window) return;

  QProcess proc;
  proc.start("hyprctl", {"clients", "-j"});
  proc.waitForFinished();

  auto json = QJsonDocument::fromJson(proc.readAllStandardOutput());

  // Several windows of this app can be mapped at once, so the class alone does not identify one.
  // Prefer the client whose title matches, then the focused one, and only then give up.
  QString titleMatch;
  QString focusedMatch;

  for (auto value : json.array()) {
    auto obj = value.toObject();
    if (obj["class"].toString() != QApplication::applicationName()) continue;

    if (titleMatch.isEmpty() && obj["title"].toString() == window->windowTitle()) {
      titleMatch = obj["address"].toString();
    }
    if (focusedMatch.isEmpty() && obj["focusHistoryID"].toInt(-1) == 0) {
      focusedMatch = obj["address"].toString();
    }
  }

  QString address = titleMatch.isEmpty() ? focusedMatch : titleMatch;
  if (address.isEmpty()) {
    qDebug() << "no hyprland client matched" << QApplication::applicationName() << window->windowTitle();
    return;
  }

  QProcess::startDetached("hyprctl", {"dispatch", QString("hl.dispatch(hl.dsp.window.float({ action = "
                                                          "\"set\", window = \"address:%1\" }))")
                                                      .arg(address)});
}

} // namespace Platform::TilingWM
