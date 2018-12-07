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
import serial
import time

# default serial configuration if no options are specified in the GUI
# ser = serial.Serial('COM4', baudrate = 9600, timeout = 1)

# signal at the start of a serial transmission
signal_str = '})]>'

class A1339_GUI(QMainWindow):
    
    def __init__(self):
        QMainWindow.__init__(self)
        
        # Create the File menu
        self.menuFile = self.menuBar().addMenu("&File")
        self.actionQuit = QAction("&Quit", self)
        self.actionQuit.triggered.connect(self.close)
        self.menuFile.addActions([self.actionQuit])
        
        widget = QDialog()
        
        # Set up message at the top of the GUI
        self.unlockLabel1 = QLabel('<font size=3>The A1339 encoder must be unlocked before it possible to write to EEPROM or Shadow Memory.</font>')
        self.unlockLabel2 = QLabel('<font size=3>EEPROM and Shadow Memory are automatically re-locked when power to the encoder is lost.</font>')
        
        # Set up "Initialize Serial Connection" button
        self.serialBtn = QPushButton('Press to initialize serial connection', self)
        self.serialBtn.clicked.connect(self.init_serial_con)
        self.serialBtn.resize(self.serialBtn.sizeHint())

        # Set up "Unlock" button
        self.unlockBtn = QPushButton('Press and wait a few seconds to unlock A1339 encoder', self)
        self.unlockBtn.clicked.connect(self.unlock_device)
        self.unlockBtn.resize(self.unlockBtn.sizeHint())

        # Set up "Rotation Direction" dropdown menu
        self.rotDirDropDown = QComboBox()
        self.rotDirDropDown.addItem('CW')
        self.rotDirDropDown.addItem('CCW')

        # Set up "Zero Position Offset" line edit
        self.zeroPosLineEdit = QLineEdit()
        self.zeroPosLineEdit.setPlaceholderText("Enter value from 0 to 360 (in degrees)")

        # Set up "Hysteresis" line edit
        self.hystLineEdit = QLineEdit()
        self.hystLineEdit.setPlaceholderText("Enter value from 0 to 1.384 (in degrees)")

        # Set up "COM Port" line edit
        self.COMLineEdit = QLineEdit()
        self.COMLineEdit.setPlaceholderText("Enter COM1 or COM2 or COM3 or etc.")

        # Set up "Baud Rate" line edit
        self.baudLineEdit = QLineEdit()
        self.baudLineEdit.setPlaceholderText("Enter 9600 or 4800 or etc.")

        # Set up "Write to Shadow Memory" button
        self.shadMemBtn = QPushButton('Write config settings to Shadow Memory', self)
        self.shadMemBtn.clicked.connect(self.write_to_shad_mem)
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

        # Set up "Configuration Options" Group Box
        self.configBox = QGroupBox("Configuration Options")
        self.configLayout = QFormLayout()
        self.configLayout.addRow("Rotation Direction:", self.rotDirDropDown)
        self.configLayout.addRow("Zero Position Offset:", self.zeroPosLineEdit)
        self.configLayout.addRow("Hysteresis:", self.hystLineEdit)
        self.configBox.setLayout(self.configLayout)

        # Set up "Serial Options" Group Box
        self.serialBox = QGroupBox("Serial Options")
        self.serialLayout = QVBoxLayout()
        self.serialFormLayout = QFormLayout()
        self.serialFormLayout.addRow("COM Port:", self.COMLineEdit)
        self.serialFormLayout.addRow("Baud Rate:", self.baudLineEdit)
        self.serialLayout.addLayout(self.serialFormLayout)
        self.serialLayout.addWidget(self.serialBtn)
        self.serialBox.setLayout(self.serialLayout)
        
        # Set up the main GUI layout
        mainLayout = QVBoxLayout()
        mainLayout.addWidget(self.serialBox)
        mainLayout.addWidget(self.unlockBox)
        mainLayout.addWidget(self.configBox)

        buttonsLayout = QHBoxLayout()
        buttonsLayout.addWidget(self.shadMemBtn)
        buttonsLayout.addWidget(self.eepromBtn)

        mainLayout.addLayout(buttonsLayout)
        
        widget.setLayout(mainLayout)
        
        self.setCentralWidget(widget)
    
    def init_serial_con(self):
        """
        Sets up the serial connection with the specified COM port and baud rate.
        """
        COM_str = str(self.COMLineEdit.text())
        baud_int = int(self.baudLineEdit.text())
        # ser = serial.Serial(COM_str, baudrate = baud_int, timeout = 1)
        # time.sleep(3)
        self.serialMessage = QMessageBox()
        self.serialMessage.setText("Serial connection set up was successful")
        self.serialMessage.show()

    def unlock_device(self):
        """
        Unlocks the A1339 to enable writes to EEPROM and Shadow Memory.
        """
        # ser.write(signal_str.encode())
        # time.sleep(3)
        
        transU = 'U'
        lenU = len(transU)
        # ser.write(lenU.encode())
        # time.sleep(3)

        # ser.write(transU.encode())
        # time.sleep(3)

        self.unlockBtn.setStyleSheet("QPushButton { background-color: green }")
        self.unlockBtn.setText("A1339 encoder is now unlocked until power to the encoder is lost")
        self.unlockBtn.setEnabled(False)

    def write_to_shad_mem(self):
        """
        Writes the configuration settings to the A1339 Shadow Memory registers.
        """
        rotDir_str = str(self.rotDirDropDown.currentText())
        zeroPos_str = str(self.zeroPosLineEdit.text())
        hyst_str = str(self.hystLineEdit.text())

        transA = 'A' + rotDir_str
        transB = 'B' + zeroPos_str
        transC = 'C' + hyst_str

        # ser.write(signal_str.encode())
        # time.sleep(3)
        lenA = len(transA)
        # ser.write(lenA.encode())
        # time.sleep(3)
        # ser.write(transA.encode())
        # time.sleep(3)

        # ser.write(signal_str.encode())
        # time.sleep(3)
        lenB = len(transB)
        # ser.write(lenB.encode())
        # time.sleep(3)
        # ser.write(transB.encode())
        # time.sleep(3)

        # ser.write(signal_str.encode())
        # time.sleep(3)
        lenC = len(transC)
        # ser.write(lenC.encode())
        # time.sleep(3)
        # ser.write(transC.encode())
        # time.sleep(3)

        self.shadMemMessage = QMessageBox()
        self.shadMemMessage.setText("Configuration settings successfully written to Shadow Memory")
        self.shadMemMessage.show()

    def burn_to_eeprom(self):
        """
        Burns the configuration settings to the A1339 EEPROM registers.
        """
        self.choice = QMessageBox.warning(self, "WARNING!",
                                          "Are you sure you want to burn configuration settings to EEPROM?",
                                          QMessageBox.No | QMessageBox.Yes)

        if self.choice == QMessageBox.Yes:
            # Go ahead and burn to EEPROM
            rotDir_str = str(self.rotDirDropDown.currentText())
            zeroPos_str = str(self.zeroPosLineEdit.text())
            hyst_str = str(self.hystLineEdit.text())

            transD = 'D' + rotDir_str
            transE = 'E' + zeroPos_str
            transF = 'F' + hyst_str

            # ser.write(signal_str.encode())
            # time.sleep(3)
            lenD = len(transD)
            # ser.write(lenD.encode())
            # time.sleep(3)
            # ser.write(transD.encode())
            # time.sleep(3)

            # ser.write(signal_str.encode())
            # time.sleep(3)
            lenE = len(transE)
            # ser.write(lenE.encode())
            # time.sleep(3)
            # ser.write(transE.encode())
            # time.sleep(3)

            # ser.write(signal_str.encode())
            # time.sleep(3)
            lenF = len(transF)
            # ser.write(lenF.encode())
            # time.sleep(3)
            # ser.write(transF.encode())
            # time.sleep(3)
        else:
            pass
        

        
app = QApplication(sys.argv)
widget = A1339_GUI()
widget.show()
app.exec_()
