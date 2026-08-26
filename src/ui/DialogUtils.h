#include "../core/PasswordUtils.h"
#include <QCheckBox>
#include <QSpinBox>
#include <QString>
#include <qdialog.h>
#include <qlabel.h>
#include <qobject.h>
#include <qwidget.h>

namespace DialogUtils {

int confirmationWindow(const QString &title, const QString &text);

class PasswordGenerationWindow : public QDialog {
public:
  explicit PasswordGenerationWindow(QWidget *parent = nullptr, const PasswordUtils::GenerationSettings &initial = {});
  QString                           generatedPassword() const;
  PasswordUtils::GenerationSettings currentSettings() const;

private:
  QSpinBox  *lengthSpin;
  QCheckBox *symbolsCheck;
  QCheckBox *digitsCheck;
  QCheckBox *uppercaseCheck;
  QLabel    *previewLabel;
  QString    currentPassword;
  QLabel    *strengthLabel;

  void regenerate();
};

} // namespace DialogUtils
