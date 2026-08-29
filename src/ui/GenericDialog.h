#pragma once
#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QVector>
#include <QWidget>
#include <qcontainerfwd.h>
#include <qtmetamacros.h>
#include <qwindowdefs.h>

class QBoxLayout;
class QLayout;
class QVBoxLayout;
class SecureString;

struct DialogField {
  QString             label;
  QLineEdit::EchoMode echoMode          = QLineEdit::Normal;
  bool                hasGenerateButton = false; // im too lazy
};

namespace DialogUtils {

// Hosts arbitrary widgets: owns the layout, the button box, accept/reject wiring and the platform
// floating request, and knows nothing about the types of the widgets it lays out.
class GenericDialog : public QDialog {
  Q_OBJECT
public:
  // Dialog: the widget is a normal Qt child and dies with the dialog.
  // Caller: the widget is unparented again when the dialog is destroyed, so it outlives the dialog
  // and the caller stays responsible for deleting it.
  enum class Ownership { Dialog, Caller };

  explicit GenericDialog(const QString                    &title   = {},
                         QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok |
                                                                     QDialogButtonBox::Cancel,
                         QWidget *parent = nullptr);

  // field-list form, kept so existing call sites keep working
  explicit GenericDialog(const QString &Title, const QVector<DialogField> &fields,
                         bool showFileSelectionButton = false, QWidget *parent = nullptr);

  ~GenericDialog() override;

  // Adds any widget to the content area and hands it straight back, typed.
  template <typename T> T *addWidget(T *widget, Ownership ownership = Ownership::Dialog) {
    hostWidget(widget, ownership);
    return widget;
  }

  // Same, preceded by a QLabel.
  template <typename T>
  T *addLabelledWidget(const QString &label, T *widget, Ownership ownership = Ownership::Dialog) {
    addLabel(label);
    hostWidget(widget, ownership);
    return widget;
  }

  // For composite rows the caller assembles itself; the dialog takes the layout, not the widgets.
  void addRow(QLayout *row);
  void addLabel(const QString &text);
  void addStretch(int stretch = 1);

  QDialogButtonBox *buttonBox() const { return genericButtonBox; }
  QVBoxLayout      *contentLayout() const { return content; }

  // Enables/disables the accept button; for callers doing their own validation.
  void setAcceptEnabled(bool enabled);

  QString inputText(int index) const; // field-list form only

protected:
  void showEvent(QShowEvent *event) override;

private:
  void hostWidget(QWidget *widget, Ownership ownership);
  void buildFields(const QVector<DialogField> &fields, bool showFileSelectionButton);

  QVBoxLayout                *content;
  QDialogButtonBox           *genericButtonBox;
  QVector<QLineEdit *>        genericInputs;
  QVector<QPointer<QWidget>>  callerOwned;
  bool                        floatingRequested = false;

private slots:
  void browseForFile();
};

// Simple helpers for the widgets this app keeps needing. Each one builds the widget, wires its
// behaviour and hands back the typed pointer, so nothing type-specific has to live in the dialog.
namespace Widgets {

QLineEdit *addTextField(GenericDialog &dialog, const QString &label,
                        QLineEdit::EchoMode echoMode = QLineEdit::Normal);

// line edit + Generate button + live strength label
QLineEdit *addGeneratedPasswordField(GenericDialog &dialog, const QString &label);

// line edit + Browse... button feeding that same field
QLineEdit *addFilePathField(GenericDialog &dialog, const QString &label,
                            const QString &caption = QStringLiteral("Select File"));

// Moves the field's text into locked memory and clears the widget behind it.
SecureString takeSecret(QLineEdit *field);

} // namespace Widgets

} // namespace DialogUtils
