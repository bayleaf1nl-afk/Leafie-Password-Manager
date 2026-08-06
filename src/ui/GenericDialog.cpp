#include "GenericDialog.h"
#include <QBoxLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QLabel>
#include <QLine>
#include <QLineEdit>
#include <QObject>
#include <QProcess>
#include <QPushButton>
#include <QWidget>
#include <qcontainerfwd.h>
#include <qwindowdefs.h>

GenericDialog::GenericDialog(const QString &Title, const QVector<DialogField> &fields, bool showFileSelectionButton,
                             QWidget *parent)
    : QDialog(parent) {
  setWindowTitle(Title);
  auto *layout = new QVBoxLayout(this);
  for (const auto &field : fields) {
    auto *label = new QLabel(field.label, this);
    auto *input = new QLineEdit(this);
    input->setEchoMode(field.echoMode);
    layout->addWidget(label);
    layout->addWidget(input);
    genericInputs.push_back(input);
  }

  if (showFileSelectionButton) {
    auto *browseButton = new QPushButton("Browse...", this);
    connect(browseButton, &QPushButton::clicked, this, &GenericDialog::browseForFile);
    layout->addWidget(browseButton);
  }

  genericButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  connect(genericButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(genericButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  layout->addWidget(genericButtonBox);
}

QString GenericDialog::inputText(int index) const { return genericInputs.at(index)->text(); }

void GenericDialog::browseForFile() {
  QString path = QFileDialog::getOpenFileName(this, "Select File");
  if (!path.isEmpty() && !genericInputs.isEmpty()) {
    genericInputs.last()->setText(path);
  }
}