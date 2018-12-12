# Name: Chase Lynd
# Class: ME 701
# Assignment: End-Of-Semester Project

# --------------------------------------------------
# GUI for setting up an Allegro A1339 angle sensor
# --------------------------------------------------

from PyQt5.QtWidgets import (QApplication, QMainWindow, QLineEdit, QDialog, 
                             QVBoxLayout, QHBoxLayout, QAction, QMessageBox,
                             QFileDialog, QComboBox, QPushButton, QSizePolicy,
                             QLabel, QGroupBox, QFormLayout)
from PyQt5.QtCore import QT_VERSION_STR, PYQT_VERSION_STR
import platform
import sys
import serial
import time

# this program will attempt to use the following default serial configuration if no options are specified in the GUI
ser = serial.Serial('COM1', baudrate = 9600, timeout = 1)

# signal at the start of a serial transmission
signal_str = '})]>'

class A1339_GUI(QMainWindow):
    
    def __init__(self):
        QMainWindow.__init__(self)

        # NOTE: The QLabels, QLineEdits, GPushButtons, etc. are organized in this code in
        #       the same order that they appear in the GUI going from top to bottom. The
        #       QGroupBoxes and QLayouts come next, followed up by the function definitions.
        
        # Create the File menu
        self.menuFile = self.menuBar().addMenu("&File")
        self.actionQuit = QAction("&Quit", self)
        self.actionQuit.triggered.connect(self.close)
        self.menuFile.addActions([self.actionQuit])

        # Set up "COM Port" line edit
        self.COMLineEdit = QLineEdit()
        self.COMLineEdit.setPlaceholderText("Enter COM1 or COM2 or COM3 or etc.")

        # Set up "Baud Rate" line edit
        self.baudLineEdit = QLineEdit()
        self.baudLineEdit.setPlaceholderText("Enter 9600 or 4800 or etc.")
        
        # Set up "Initialize Serial Connection" button
        self.serialBtn = QPushButton('Press to initialize serial connection', self)
        self.serialBtn.clicked.connect(self.init_serial_con)
        self.serialBtn.resize(self.serialBtn.sizeHint())
        
        # Set up message at the top of the "Device Unlock" Group Box
        self.unlockLabel1 = QLabel('<font size=3>The A1339 sensor must be unlocked before it is possible to write to EEPROM or Shadow Memory.</font>')
        self.unlockLabel2 = QLabel('<font size=3>EEPROM and Shadow Memory are automatically re-locked when power to the sensor is lost.</font>')
        
        # Set up "Unlock" button
        self.unlockBtn = QPushButton('Press and wait a few seconds to unlock A1339 sensor', self)
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

        # Set up "Write to Shadow Memory" button
        self.shadMemBtn = QPushButton('Write config settings to Shadow Memory', self)
        self.shadMemBtn.clicked.connect(self.write_to_shad_mem)
        self.shadMemBtn.resize(self.shadMemBtn.sizeHint())

        #Set up "Burn to EEPROM" button
        self.eepromBtn = QPushButton("Burn config settings to EEPROM", self)
        self.eepromBtn.clicked.connect(self.burn_to_eeprom)
        self.eepromBtn.resize(self.eepromBtn.sizeHint())

        # Set up "Serial Options" Group Box
        self.serialBox = QGroupBox("Serial Options")
        self.serialLayout = QVBoxLayout()
        self.serialFormLayout = QFormLayout()
        self.serialFormLayout.addRow("COM Port:", self.COMLineEdit)
        self.serialFormLayout.addRow("Baud Rate:", self.baudLineEdit)
        self.serialLayout.addLayout(self.serialFormLayout)
        self.serialLayout.addWidget(self.serialBtn)
        self.serialBox.setLayout(self.serialLayout)

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
        
        # Set up "buttonsLayout" to make the Shadow Memory and EEPROM buttons side-by-side
        buttonsLayout = QHBoxLayout()
        buttonsLayout.addWidget(self.shadMemBtn)
        buttonsLayout.addWidget(self.eepromBtn)

        # Set up the main GUI layout
        mainLayout = QVBoxLayout()
        mainLayout.addWidget(self.serialBox)
        mainLayout.addWidget(self.unlockBox)
        mainLayout.addWidget(self.configBox)
        mainLayout.addLayout(buttonsLayout)

        # Set up the main GUI widget
        widget = QDialog()
        widget.setLayout(mainLayout)
        self.setCentralWidget(widget)
    
    def init_serial_con(self):
        """
        Sets up the serial connection with the specified COM port and baud rate.
        This function is called if the user clicks the 'Press to initialize serial connection' button.
        If the 'Press to initialize serial connection' is never clicked, this program will try to use
        the default serial settings (COM Port = COM1 and Baud Rate = 9600). However, it is not likely
        the default serial settings will work.
        """
        # set up pop-up message for invalid (empty) serial settings
        self.serialInvalidMessage = QMessageBox()
        self.serialInvalidMessage.setText("Please enter settings for the COM port and baud rate")

        # make sure the user has specified serial communication parameters
        if str(self.COMLineEdit.text()) == "":
            self.serialInvalidMessage.show()
        elif str(self.baudLineEdit.text()) == "":
            self.serialInvalidMessage.show()
        else:
            # proceed with serial communication setup
            COM_str = str(self.COMLineEdit.text())
            baud_int = int(self.baudLineEdit.text())
            ser = serial.Serial(COM_str, baudrate = baud_int, timeout = 1)

            # set up and show pop-up message for successful serial communication setup
            self.serialMessage = QMessageBox()
            self.serialMessage.setText("Serial connection set up was successful")
            self.serialMessage.show()

    def unlock_device(self):
        """
        Unlocks the A1339 to enable writes to EEPROM and Shadow Memory.
        """
        # set up the transmission (message) that will be sent using serial communication.
        # the letter (U) is a 'code letter' that the program receiving the
        # the serial communication will use to determine what action to take.
        transU = 'U'

        # send the transmission using serial communication.
        # the time.sleep(3) function pauses the code execution for 3 seconds.
        # the pauses ensure that the transmission has been completely sent, received, and handled
        # by the receiving program. the signal_str is sent before each configuration option
        # transmission. the signal_str is received and handled by the receiving program.
        # the total length of the transmission (not including the signal_str) is always sent
        # before the actual transmission, as the length is required by the receiving program.
        ser.write(signal_str.encode())
        time.sleep(3)
        lenU = len(transU)
        ser.write(lenU.encode())
        time.sleep(3)
        ser.write(transU.encode())
        time.sleep(3)

        # make the unlock button green and disabled after unlocking the A1339 sensor
        self.unlockBtn.setStyleSheet("QPushButton { background-color: green }")
        self.unlockBtn.setText("A1339 sensor is now unlocked until power to the sensor is lost")
        self.unlockBtn.setEnabled(False)

    def write_to_shad_mem(self):
        """
        Writes the configuration settings to the A1339 Shadow Memory registers.
        """
        # set up pop-up messages for invalid Zero Position Offset or Hysteresis
        self.zeroPosInvalidMessage = QMessageBox()
        self.zeroPosInvalidMessage.setText("Please enter a value for Zero Position Offset between 0 and 360")
        self.hystInvalidMessage = QMessageBox()
        self.hystInvalidMessage.setText("Please enter a value for Hysteresis between 0 and 1.384")
        self.emptyMessage = QMessageBox()
        self.emptyMessage.setText("Please enter a value for Zero Position Offset and Hysteresis")

        # make sure the user has specified a Zero Position Offset
        if str(self.zeroPosLineEdit.text()) == "":
            self.emptyMessage.show()

        # make sure the user has specified a Hysteresis
        elif str(self.hystLineEdit.text()) == "":
            self.emptyMessage.show()

        # make sure Zero Position Offset is set between 0 and 360
        elif (float(self.zeroPosLineEdit.text()) < 0) or (float(self.zeroPosLineEdit.text()) > 360):
            self.zeroPosInvalidMessage.show()

        # make sure Hysteresis is set between 0 and 1.384
        elif (float(self.hystLineEdit.text()) < 0) or (float(self.hystLineEdit.text()) > 1.384):
            self.hystInvalidMessage.show()

        # proceed with writing to Shadow Memory
        else:

            # get the information from the text boxes
            rotDir_str = str(self.rotDirDropDown.currentText())
            zeroPos_str = str(self.zeroPosLineEdit.text())
            hyst_str = str(self.hystLineEdit.text())

            # set up the transmissions (messages) that will be sent using serial communication.
            # the letters (A, B, & C) at the beginning are 'code letters' that the program receiving the
            # the serial communication will use to determine what action to take.
            transA = 'A' + rotDir_str
            transB = 'B' + zeroPos_str
            transC = 'C' + hyst_str

            # send the transmissions using serial communication.
            # the time.sleep(3) function pauses the code execution for 3 seconds.
            # the pauses ensure that the transmission has been completely sent, received, and handled
            # by the receiving program. the signal_str is sent before each configuration option
            # transmission. the signal_str is received and handled by the receiving program.
            # the total length of the transmission (not including the signal_str) is always sent
            # before the actual transmission, as the length is required by the receiving program.
            ser.write(signal_str.encode())
            time.sleep(3)
            lenA = len(transA)
            ser.write(lenA.encode())
            time.sleep(3)
            ser.write(transA.encode())
            time.sleep(3)

            ser.write(signal_str.encode())
            time.sleep(3)
            lenB = len(transB)
            ser.write(lenB.encode())
            time.sleep(3)
            ser.write(transB.encode())
            time.sleep(3)

            ser.write(signal_str.encode())
            time.sleep(3)
            lenC = len(transC)
            ser.write(lenC.encode())
            time.sleep(3)
            ser.write(transC.encode())
            time.sleep(3)

            # set up and show pop-up message for successful write to Shadow Memory
            self.shadMemMessage = QMessageBox()
            self.shadMemMessage.setText("Configuration settings successfully written to Shadow Memory")
            self.shadMemMessage.show()

    def burn_to_eeprom(self):
        """
        Burns the configuration settings to the A1339 EEPROM registers.
        """
        # set up pop-up messages for invalid Zero Position Offset or Hysteresis
        self.zeroPosInvalidMessage = QMessageBox()
        self.zeroPosInvalidMessage.setText("Please enter a value for Zero Position Offset between 0 and 360")
        self.hystInvalidMessage = QMessageBox()
        self.hystInvalidMessage.setText("Please enter a value for Hysteresis between 0 and 1.384")
        self.emptyMessage = QMessageBox()
        self.emptyMessage.setText("Please enter a value for Zero Position Offset and Hysteresis")

        # make sure the user has specified a Zero Position Offset
        if str(self.zeroPosLineEdit.text()) == "":
            self.emptyMessage.show()

        # make sure the user has specified a Hysteresis
        elif str(self.hystLineEdit.text()) == "":
            self.emptyMessage.show()

        # make sure Zero Position Offset is set between 0 and 360
        elif (float(self.zeroPosLineEdit.text()) < 0) or (float(self.zeroPosLineEdit.text()) > 360):
            self.zeroPosInvalidMessage.show()

        # make sure Hysteresis is set between 0 and 1.384
        elif (float(self.hystLineEdit.text()) < 0) or (float(self.hystLineEdit.text()) > 1.384):
            self.hystInvalidMessage.show()

        # proceed with writing to EEPROM
        else:
            # double check that the user wants to burn settings to EEPROM
            self.choice = QMessageBox.warning(self, "WARNING!",
                                            "Are you sure you want to burn configuration settings to EEPROM?",
                                            QMessageBox.No | QMessageBox.Yes)
            
            if self.choice == QMessageBox.Yes:
                # go ahead and burn to EEPROM

                # get the information from the text boxes
                rotDir_str = str(self.rotDirDropDown.currentText())
                zeroPos_str = str(self.zeroPosLineEdit.text())
                hyst_str = str(self.hystLineEdit.text())

                # set up the transmissions (messages) that will be sent using serial communication.
                # the letters (D, E, & F) at the beginning are 'code letters' that the program receiving the
                # the serial communication will use to determine what action to take.
                transD = 'D' + rotDir_str
                transE = 'E' + zeroPos_str
                transF = 'F' + hyst_str

                # send the transmissions using serial communication.
                # the time.sleep(3) function pauses the code execution for 3 seconds.
                # the pauses ensure that the transmission has been completely sent, received, and handled
                # by the receiving program. the signal_str is sent before each configuration option
                # transmission. the signal_str is received and handled by the receiving program.
                # the total length of the transmission (not including the signal_str) is always sent
                # before the actual transmission, as the length is required by the receiving program.
                ser.write(signal_str.encode())
                time.sleep(3)
                lenD = len(transD)
                ser.write(lenD.encode())
                time.sleep(3)
                ser.write(transD.encode())
                time.sleep(3)

                ser.write(signal_str.encode())
                time.sleep(3)
                lenE = len(transE)
                ser.write(lenE.encode())
                time.sleep(3)
                ser.write(transE.encode())
                time.sleep(3)

                ser.write(signal_str.encode())
                time.sleep(3)
                lenF = len(transF)
                ser.write(lenF.encode())
                time.sleep(3)
                ser.write(transF.encode())
                time.sleep(3)

                # set up and show pop-up message for successful write to Shadow Memory
                self.eepromMessage = QMessageBox()
                self.eepromMessage.setText("Configuration settings successfully written to EEPROM")
                self.eepromMessage.show()
            else:
                # user elected NOT to burn to EEPROM, so do nothing
                pass
        
# final lines necessary to utilize the A1339_GUI class above
app = QApplication(sys.argv)
widget = A1339_GUI()
widget.show()
app.exec_()
