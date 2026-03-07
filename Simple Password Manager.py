import cryptography
import json
from PySide6 import QtCore, QtWidgets, QtGui
import sys
import random
passwordExample = {
    "gmail": "bayleaf@gmail.john",
    "website": "https://www.bayleaf.com",
    "password": "IAMAVERYSECUREPASSWORDRAAAAAAAAAAAAAGH"
}
# TODO: make this appear iff it doesnt exist & encrypt it

# with open("password.json", "w") as PassFile:
#     data = json.dump(passwordExample, PassFile)

class MainWindow(QtWidgets.QWidget): 
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Password Manager")
        self.Layout = QtWidgets.QVBoxLayout()
        self.ListWidget = QtWidgets.QListWidget()
        self.AddPasword = QtWidgets.QPushButton("Add Entry")
        self.DeletePassword = QtWidgets.QPushButton("Remove Entry")
        self.Layout.addWidget(QtWidgets.QLabel("Saved Passwords"))
        self.Layout.addWidget(self.ListWidget)
        #self.Layout.addWidget(self.AddPassword)
        #self.Layout.addWidget(self.DeletePassword)

        self.setLayout(self.Layout)

        self.AddPasword.clicked.connect(self.AddEntry)
        self.DeletePassword.clicked.connect(self.DeleteEntry)

        self.Authenticate()
    
    def Authenticate(self):
        password, isCorrect = QtWidgets.QInputDialog.getText(self, "Master Password", "Enter Master Password", QtWidgets.QLineEdit.Password)
        if not isCorrect: sys.exit()
    def AddEntry(self):
        pass
    def DeleteEntry(self):
        pass
if __name__ == "__main__":
    app = QtWidgets.QApplication([])
    window = MainWindow()
    window.resize(800, 600)
    window.show()

    sys.exit(app.exec())
# program = QApplication([])
# window = MainWindow()
# window.show()

# program.exec()
