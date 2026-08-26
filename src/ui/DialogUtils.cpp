#include "DialogUtils.h"
#include "../core/PasswordUtils.h"
#include <QMessageBox>
#include <qboxlayout.h>
#include <qcheckbox.h>
#include <qdialogbuttonbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qwidget.h>
namespace DialogUtils {

int confirmationWindow(const QString &title, const QString &text) {
  auto msgBox = QMessageBox();
  msgBox.setWindowTitle(title);
  msgBox.setText(text);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);
  msgBox.setIcon(QMessageBox::NoIcon);
  int choice = msgBox.exec();
  return choice;
}

PasswordGenerationWindow::PasswordGenerationWindow(QWidget *parent, const PasswordUtils::GenerationSettings &initial)
    : QDialog(parent) {
  setWindowTitle("Generate Password");
  auto *layout = new QVBoxLayout(this);

  previewLabel = new QLabel;
  previewLabel->setStyleSheet("font-weight:600;");
  layout->addWidget(previewLabel);

  strengthLabel = new QLabel;
  layout->addWidget(strengthLabel);

  layout->addWidget(new QLabel("Length: "));
  lengthSpin = new QSpinBox;
  lengthSpin->setRange(8, 64);
  lengthSpin->setValue(16);
  layout->addWidget(lengthSpin);

  symbolsCheck   = new QCheckBox("Include symbols");
  digitsCheck    = new QCheckBox("Include digits");
  uppercaseCheck = new QCheckBox("Include uppercase letters");
  digitsCheck->setChecked(true);
  uppercaseCheck->setChecked(true);

  layout->addWidget(symbolsCheck);
  layout->addWidget(digitsCheck);
  layout->addWidget(uppercaseCheck);

  connect(lengthSpin, &QSpinBox::valueChanged, this, &PasswordGenerationWindow::regenerate);
  connect(symbolsCheck, &QCheckBox::toggled, this, &PasswordGenerationWindow::regenerate);
  connect(digitsCheck, &QCheckBox::toggled, this, &PasswordGenerationWindow::regenerate);
  connect(uppercaseCheck, &QCheckBox::toggled, this, &PasswordGenerationWindow::regenerate);

  auto *regenButton = new QPushButton("Regenerate");
  connect(regenButton, &QPushButton::clicked, this, &PasswordGenerationWindow::regenerate);
  layout->addWidget(regenButton);

  auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  layout->addWidget(buttonBox);

  regenerate();
}
QString PasswordGenerationWindow::generatedPassword() const { return currentPassword; }

void PasswordGenerationWindow::regenerate() {
  PasswordUtils::GenerationSettings settings;
  settings.length       = lengthSpin->value();
  settings.useSymbols   = symbolsCheck->isChecked();
  settings.useDigits    = digitsCheck->isChecked();
  settings.useUppercase = uppercaseCheck->isChecked();

  currentPassword = PasswordUtils::generatePassword(settings);
  previewLabel->setText(currentPassword);

  double score = PasswordUtils::estimateStrength(currentPassword, {});
  strengthLabel->setText(PasswordUtils::strengthLabel(score));
}

} // namespace DialogUtils
