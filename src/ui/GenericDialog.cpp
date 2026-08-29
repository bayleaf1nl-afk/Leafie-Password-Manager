#include "GenericDialog.h"
#include "../core/PasswordUtils.h"
#include "../core/SecureString.h"
#include "../platform/WindowManager.h"
#include <QBoxLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QPushButton>
#include <QShowEvent>
#include <QWidget>
#include <qcontainerfwd.h>
#include <qwindowdefs.h>

namespace DialogUtils {

GenericDialog::GenericDialog(const QString &title, QDialogButtonBox::StandardButtons buttons, QWidget *parent)
    : QDialog(parent) {
  setWindowTitle(title);

  auto *outer = new QVBoxLayout(this);
  content     = new QVBoxLayout;
  outer->addLayout(content);

  genericButtonBox = new QDialogButtonBox(buttons, this);
  connect(genericButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(genericButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  outer->addWidget(genericButtonBox);
}

GenericDialog::GenericDialog(const QString &Title, const QVector<DialogField> &fields, bool showFileSelectionButton,
                             QWidget *parent)
    : GenericDialog(Title, QDialogButtonBox::Ok | QDialogButtonBox::Cancel, parent) {
  buildFields(fields, showFileSelectionButton);
}

GenericDialog::~GenericDialog() {
  for (const QPointer<QWidget> &widget : callerOwned) {
    if (!widget) continue;
    content->removeWidget(widget);
    widget->setParent(nullptr);
  }
}

void GenericDialog::hostWidget(QWidget *widget, Ownership ownership) {
  if (!widget) return;
  content->addWidget(widget);
  if (ownership == Ownership::Caller) callerOwned.push_back(widget);
}

void GenericDialog::addRow(QLayout *row) {
  if (row) content->addLayout(row);
}

void GenericDialog::addLabel(const QString &text) { content->addWidget(new QLabel(text, this)); }

void GenericDialog::addStretch(int stretch) { content->addStretch(stretch); }

void GenericDialog::setAcceptEnabled(bool enabled) {
  if (auto *ok = genericButtonBox->button(QDialogButtonBox::Ok)) ok->setEnabled(enabled);
}

void GenericDialog::showEvent(QShowEvent *event) {
  QDialog::showEvent(event);
  if (floatingRequested) return;
  floatingRequested = true;
  Platform::attemptFloating(this);
}

void GenericDialog::buildFields(const QVector<DialogField> &fields, bool showFileSelectionButton) {
  for (const auto &field : fields) {
    QLineEdit *input = field.hasGenerateButton
                           ? Widgets::addGeneratedPasswordField(*this, field.label, field.echoMode)
                           : Widgets::addTextField(*this, field.label, field.echoMode);
    genericInputs.push_back(input);
  }

  if (showFileSelectionButton) {
    auto *browseButton = new QPushButton("Browse...", this);
    connect(browseButton, &QPushButton::clicked, this, &GenericDialog::browseForFile);
    content->addWidget(browseButton);
  }
}

QString GenericDialog::inputText(int index) const { return genericInputs.at(index)->text(); }

void GenericDialog::browseForFile() {
  QString path = QFileDialog::getOpenFileName(this, "Select File");
  if (!path.isEmpty() && !genericInputs.isEmpty()) {
    genericInputs.last()->setText(path);
  }
}

namespace Widgets {

QLineEdit *addTextField(GenericDialog &dialog, const QString &label, QLineEdit::EchoMode echoMode) {
  auto *input = new QLineEdit;
  input->setEchoMode(echoMode);
  return dialog.addLabelledWidget(label, input);
}

QLineEdit *addGeneratedPasswordField(GenericDialog &dialog, const QString &label, QLineEdit::EchoMode echoMode) {
  dialog.addLabel(label);

  auto *input = new QLineEdit;
  input->setEchoMode(echoMode);

  auto *row       = new QHBoxLayout;
  auto *genButton = new QPushButton("Generate");
  row->addWidget(input);
  row->addWidget(genButton);
  dialog.addRow(row);

  auto *strengthLabel = dialog.addWidget(new QLabel);

  QObject::connect(genButton, &QPushButton::clicked, input,
                   [input]() { input->setText(PasswordUtils::generatePassword({})); });
  QObject::connect(input, &QLineEdit::textChanged, strengthLabel, [strengthLabel](const QString &text) {
    strengthLabel->setText(PasswordUtils::strengthLabel(PasswordUtils::estimateStrength(text, {})));
  });

  return input;
}

QLineEdit *addFilePathField(GenericDialog &dialog, const QString &label, const QString &caption) {
  dialog.addLabel(label);

  auto *input = new QLineEdit;

  auto *row          = new QHBoxLayout;
  auto *browseButton = new QPushButton("Browse...");
  row->addWidget(input);
  row->addWidget(browseButton);
  dialog.addRow(row);

  QObject::connect(browseButton, &QPushButton::clicked, input, [input, &dialog, caption]() {
    QString path = QFileDialog::getOpenFileName(&dialog, caption);
    if (!path.isEmpty()) input->setText(path);
  });

  return input;
}

SecureString takeSecret(QLineEdit *field) {
  if (!field) return {};
  SecureString secret(field->text());
  field->clear(); // Qt may keep its own copies of the plaintext; this only shortens their lifetime
  return secret;
}

} // namespace Widgets

} // namespace DialogUtils
