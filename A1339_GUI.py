# Name: Chase Lynd
# Class: ME 701
# Assignment: End-Of-Semester Project

from PyQt5.QtWidgets import (QApplication, QMainWindow, QLineEdit, QDialog, 
                             QVBoxLayout, QHBoxLayout, QAction, QMessageBox,
                             QFileDialog, QComboBox, QPushButton, QSizePolicy,
                             QLabel, QGroupBox, QFormLayout)
from PyQt5.QtCore import QT_VERSION_STR, PYQT_VERSION_STR
import platform
import sys

class A1339_GUI(QMainWindow):
    
    def __init__(self):
        QMainWindow.__init__(self)
        
        # Create the File menu
        self.menuFile = self.menuBar().addMenu("&File")
        self.actionQuit = QAction("&Quit", self)
        self.actionQuit.triggered.connect(self.close)
        self.menuFile.addActions([self.actionQuit])
        
        widget = QDialog()
        
        self.unlockLabel1 = QLabel('<font size=3>The A1339 encoder must be unlocked before it possible to write to EEPROM or Shadow Memory.</font>')
        self.unlockLabel2 = QLabel('<font size=3>EEPROM and Shadow Memory are automatically re-locked when power to the encoder is lost.</font>')
        
        # Set up "Unlock" button
        self.unlockBtn = QPushButton('Press to unlock A1339 encoder', self)
        self.unlockBtn.clicked.connect(self.unlock_device)
        self.unlockBtn.clicked.connect(self.make_btn_green)
        self.unlockBtn.resize(self.unlockBtn.sizeHint())

        # Set up "Set Zero Position" button
        # self.zeroPosBtn = QPushButton('Press to set encoder zero position to current encoder position', self)
        # self.zeroPosBtn.clicked.connect(self.set_zero_pos_to_current)
        # self.message = QMessageBox()
        # self.message.setText("Zero position has been set")
        # self.zeroPosBtn.clicked.connect(self.message.exec)
        # self.zeroPosBtn.resize(self.zeroPosBtn.sizeHint())

        # Set up "Rotation Direction" dropdown menu
        self.rotDirDropDown = QComboBox()
        #self.funcDropDown.setInsertPolicy(2)
        #self.funcDropDown.setEditable(True)
        self.rotDirDropDown.addItem('CW')
        self.rotDirDropDown.addItem('CCW')

        # Set up "Zero Position Offset" line edit
        self.zeroPosLineEdit = QLineEdit()
        self.zeroPosLineEdit.setPlaceholderText("Enter value from 0 to 360 (in degrees)")

        # Set up "Hysteresis" line edit
        self.hystLineEdit = QLineEdit()
        self.hystLineEdit.setPlaceholderText("Enter value from 0 to 1.384 (in degrees)")

        # Set up "Write to Shadow Memory" button
        self.shadMemBtn = QPushButton('Write config settings to Shadow Memory', self)
        self.shadMemBtn.clicked.connect(self.write_to_shad_mem)
        # The pop-up message below should probably be moved to be within the write_to_shad_mem function
        self.shadMemMessage = QMessageBox()
        self.shadMemMessage.setText("Configuration settings successfully written to Shadow Memory")
        self.shadMemBtn.clicked.connect(self.shadMemMessage.exec)
        self.shadMemBtn.resize(self.shadMemBtn.sizeHint())

        #Set up "Burn to EEPROM" button
        self.eepromBtn = QPushButton("Burn config settings to EEPROM", self)
        self.eepromBtn.clicked.connect(self.burn_to_eeprom)
        self.eepromBtn.resize(self.eepromBtn.sizeHint())

        
        # Set up "Device Unlock" Group Box
        self.unlockBox = QGroupBox("Device Unlock")
        self.unlockLayout = QVBoxLayout()
        self.unlockLayout.addWidget(self.unlockLabel1)
        self.unlockLayout.addWidget(self.unlockLabel2)
        self.unlockLayout.addWidget(self.unlockBtn)
        self.unlockBox.setLayout(self.unlockLayout)
        
        # Set up "Zero Position" Group Box
        # self.zeroPosBox = QGroupBox("Zero Position")
        # self.zeroPosLayout = QVBoxLayout()
        # self.zeroPosLayout.addWidget(self.zeroPosBtn)
        # self.zeroPosBox.setLayout(self.zeroPosLayout)

        # Set up "Configuration Options" Form Layout
        # self.configFormLayout = QFormLayout()
        # self.configFormLayout.addRow("Rotation Direction:", self.rotDirDropDown)
        # self.configFormLayout.addRow("Zero Position Offset:", self.zeroPosLineEdit)
        # self.configFormLayout.addRow("Hysteresis:", self.hystLineEdit)

        # Set up "Configuration Options" Group Box
        self.configBox = QGroupBox("Configuration Options")
        self.configLayout = QFormLayout()
        self.configLayout.addRow("Rotation Direction:", self.rotDirDropDown)
        self.configLayout.addRow("Zero Position Offset:", self.zeroPosLineEdit)
        self.configLayout.addRow("Hysteresis:", self.hystLineEdit)
        self.configBox.setLayout(self.configLayout)
        
        # Set up the main GUI layout
        mainLayout = QVBoxLayout()
        mainLayout.addWidget(self.unlockBox)
        mainLayout.addWidget(self.configBox)

        burnButtonsLayout = QHBoxLayout()
        burnButtonsLayout.addWidget(self.shadMemBtn)
        burnButtonsLayout.addWidget(self.eepromBtn)

        mainLayout.addLayout(burnButtonsLayout)
        
        widget.setLayout(mainLayout)
        
        self.setCentralWidget(widget)
    
    def unlock_device(self):
        """
        Unlocks the A1339 to enable writes to EEPROM and Shadow Memory.
        """
        
    def make_btn_green(self):
        self.unlockBtn.setStyleSheet("QPushButton { background-color: green }")
        self.unlockBtn.setText("A1339 encoder is now unlocked until power to the encoder is lost")
        self.unlockBtn.setEnabled(False)
        pass

    def set_zero_pos_to_current(self):
        """

        """

    def write_to_shad_mem(self):
        """

        """

    def burn_to_eeprom(self):
        """

        """
        self.choice = QMessageBox.warning(self, "WARNING!",
                                          "Are you sure you want to burn configuration settings to EEPROM?",
                                          QMessageBox.No | QMessageBox.Yes)

        if self.choice == QMessageBox.Yes:
            print("Go for it")
            # Go ahead and burn to EEPROM
            # NEED TO IMPLEMENT
        else:
            pass
        



        
app = QApplication(sys.argv)
widget = A1339_GUI()
widget.show()
app.exec_()
