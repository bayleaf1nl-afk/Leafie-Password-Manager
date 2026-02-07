import cryptography
import json
from PySide6.QtWidgets import *

passwordExample = {
    "gmail": "bayleaf@gmail.john",
    "website": "https://www.bayleaf.com",
    "password": "IAMAVERYSECUREPASSWORDRAAAAAAAAAAAAAGH"
}
# TODO: make this appear iff it doesnt exist & encrypt it
# with open("password.json", "w") as PassFile:
#     data = json.dump(passwordExample, PassFile)

class MainWindow(QMainWindow): 
    def __init__(self):
        super().__init__()
        self.setWindowTitle("RAAAAAAAAAAAAAAGH")
        button = QPushButton("water me")
        self.setCentralWidget(button)
    


program = QApplication([])
window = MainWindow()
window.show()

program.exec()

