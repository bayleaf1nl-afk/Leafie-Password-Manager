#pragma once
#include <QDialogButtonBox>
#include <QDialog>
#include <cstddef>
#include <QLineEdit>
#include <qcontainerfwd.h>
#include <qlineedit.h>
#include <qobject.h>
#include <qtmetamacros.h>
#include <qwidget.h>
#include <qwindowdefs.h>
#include <QDialog>
#include <QVector>
struct DialogField{
    QString label;
    QLineEdit::EchoMode echoMode = QLineEdit::Normal;
};


class GenericDialog : public QDialog{
    Q_OBJECT
    public:
        explicit GenericDialog( const QString &Title = "GRAHHHHHHHHHHH",
                                const QVector<DialogField> &fields = {},
                                bool showFileSelectionButton = false,
                                QWidget *parent = nullptr); //constructor init
        QString inputText(int index) const;
    private:
        QVector<QLineEdit*> genericInputs;
        QDialogButtonBox *genericButtonBox;
    private slots:
        void browseForFile();
};