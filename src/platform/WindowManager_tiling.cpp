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
  // Titles are not unique either, so focus breaks the tie: a just-shown dialog is the focused client.
  QString focusedTitleMatch;
  QString titleMatch;
  QString focusedMatch;

  for (auto value : json.array()) {
    auto obj = value.toObject();
    if (obj["class"].toString() != QApplication::applicationName()) continue;

    const QString address   = obj["address"].toString();
    const bool    sameTitle = obj["title"].toString() == window->windowTitle();
    const bool    focused   = obj["focusHistoryID"].toInt(-1) == 0;

    if (sameTitle && focused && focusedTitleMatch.isEmpty()) focusedTitleMatch = address;
    if (sameTitle && titleMatch.isEmpty()) titleMatch = address;
    if (focused && focusedMatch.isEmpty()) focusedMatch = address;
  }

  QString address = focusedTitleMatch;
  if (address.isEmpty()) address = titleMatch;
  if (address.isEmpty()) address = focusedMatch;
  if (address.isEmpty()) {
    qDebug() << "no hyprland client matched" << QApplication::applicationName() << window->windowTitle();
    return;
  }

  QProcess::startDetached("hyprctl", {"dispatch", QString("hl.dispatch(hl.dsp.window.float({ action = "
                                                          "\"set\", window = \"address:%1\" }))")
                                                      .arg(address)});
}

} // namespace Platform::TilingWM
