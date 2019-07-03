import sys
from PyQt5.QtWidgets import QApplication, QWidget, QCheckBox, QLineEdit
from PyQt5.QtWidgets import QHBoxLayout, QVBoxLayout
from PyQt5.QtCore import Qt


class MyApp(QWidget):
    
    orderLnStr = ""
    def __init__(self):
        super().__init__()

        self.initUI()

    def initUI(self):
        self.orderLn = QLineEdit()

        cbJJ = QCheckBox('JJ ', self)
        cbJB = QCheckBox('JB ', self)
        cbTS = QCheckBox('TS ', self)
        cbJJ.stateChanged.connect(self.orderChanged)
        cbJB.stateChanged.connect(self.orderChanged)
        cbTS.stateChanged.connect(self.orderChanged)

        vbox1 = QVBoxLayout()
        vbox1.addWidget(self.orderLn)
        vbox1.addWidget(cbJJ)
        vbox1.addWidget(cbJB)
        vbox1.addWidget(cbTS)

        hbox1 = QHBoxLayout()
        hbox1.addStretch(1)
        hbox1.addLayout(vbox1)
        hbox1.addStretch(1)
        self.setLayout(hbox1)

        self.setWindowTitle('order sheet')
        self.setGeometry(300, 300, 300, 200)
        self.show()

    def orderChanged(self, state):
        if state == Qt.Checked:
            MyApp.orderLnStr += self.sender().text()
        else:
            MyApp.orderLnStr = MyApp.orderLnStr.replace(self.sender().text(), "", 1)
        self.orderLn.setText(MyApp.orderLnStr)

if __name__ == '__main__':

    app = QApplication(sys.argv)
    ex = MyApp()
    sys.exit(app.exec_())
