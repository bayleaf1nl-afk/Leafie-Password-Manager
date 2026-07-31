#include "GenericDialog.h"
#include "MainWindow.h"
#include <qboxlayout.h>
#include <qcontainerfwd.h>
#include <qdialogbuttonbox.h>
#include <qlabel.h>
#include <qline.h>
#include <qlineedit.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qwidget.h>
#include <qwindowdefs.h>
#include <QDialog>
#include <QProcess>
#include <QFileDialog>

GenericDialog::GenericDialog(const QString &Title,
                            const QVector<DialogField> &fields,
                            bool showFileSelectionButton,
                            QWidget *parent)
: QDialog(parent)
{
    setWindowTitle(Title);
    auto* layout = new QVBoxLayout(this);
    for (const auto &field : fields) {
        auto *label = new QLabel(field.label, this);
        auto *input = new QLineEdit(this);
        input->setEchoMode(field.echoMode);    
        layout->addWidget(label);
        layout->addWidget(input);
        genericInputs.push_back(input);
    }

    if (showFileSelectionButton){
        auto *browseButton = new QPushButton("Browse...", this);
        connect(browseButton, &QPushButton::clicked, this, &GenericDialog::browseForFile);
        layout->addWidget(browseButton);
    }

    genericButtonBox = new QDialogButtonBox
    (QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(genericButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(genericButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(genericButtonBox);

}

QString GenericDialog::inputText(int index) const{
    return genericInputs.at(index)->text();
}

void GenericDialog::browseForFile(){
    QString path = QFileDialog::getOpenFileName(this, "Select File");
    if (!path.isEmpty() && !genericInputs.isEmpty()){
        genericInputs.last()->setText(path);
    }
}