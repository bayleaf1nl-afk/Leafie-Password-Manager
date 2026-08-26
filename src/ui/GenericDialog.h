#pragma once
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QObject>
#include <QVector>
#include <QWidget>
#include <qcontainerfwd.h>
#include <qtmetamacros.h>
#include <qwindowdefs.h>
struct DialogField {
  QString             label;
  QLineEdit::EchoMode echoMode          = QLineEdit::Normal;
  bool                hasGenerateButton = false; // im too lazy
};

namespace DialogUtils {

class GenericDialog : public QDialog {
  Q_OBJECT
public:
  explicit GenericDialog(const QString &Title = "GRAHHHHHHHHHHH", const QVector<DialogField> &fields = {},
                         bool     showFileSelectionButton = false,
                         QWidget *parent                  = nullptr); // constructor init
  QString inputText(int index) const;

private:
  QVector<QLineEdit *> genericInputs;
  QDialogButtonBox    *genericButtonBox;
private slots:
  void browseForFile();
};

} // namespace DialogUtils
