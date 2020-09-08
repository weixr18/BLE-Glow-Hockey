import re
import sys
import binascii
from PyQt5.QtSerialPort import QSerialPort, QSerialPortInfo
from PyQt5.QtWidgets import QApplication, QMainWindow


class MyMainWindow(QMainWindow):
    def __init__(self, parent=None):
        super(MyMainWindow, self).__init__(parent)

    # FPGA receive data
    def com_Receive_Data_From_FPGA(self):
        global com_FPGA
        try:
            rxData = bytes(com_FPGA.readAll())
        except:
            print('严重错误', '串口接收数据错误')

        Data = binascii.b2a_hex(rxData).decode('ascii')

        self.com_PSoC_Send_Data(Data)

    # PSoC send data
    def com_PSoC_Send_Data(self, txData: str):

        global com_PSoC
        if len(txData) == 0:
            return
        print(txData)

        Data = txData.replace(' ', '')
        if len(Data) % 2 == 1:
            Data = Data[0:len(Data) - 1]
        if Data.isalnum() is False:
            print('错误', '包含非十六进制数')
        try:
            hexData = binascii.a2b_hex(Data)
        except:
            print('错误', '转换编码错误')
            return
        try:
            com_PSoC.write(hexData)
        except:
            print('异常', '十六进制发送错误')
            return

    
    def com_PSoC_Receive_Data(self):
        print(12345678)

    pass


com_PSoC = QSerialPort()
com_FPGA = QSerialPort()


if __name__ == '__main__':

    MY_DEBUG = True
    if(len(sys.argv) < 3):
        print("Please input PSoC and FPGA COM names.")

    else:
        com_PSoC_name = sys.argv[1]
        com_FPGA_name = sys.argv[2]
        app = QApplication(sys.argv)
        myWin = MyMainWindow()


        com_info_list = QSerialPortInfo.availablePorts()
        for info in com_info_list:

            # PSoC port initialize
            if (info.portName() == com_PSoC_name):
                com_PSoC.setPort(info)
                if com_PSoC.open(QSerialPort.ReadWrite):
                    com_PSoC.setDataTerminalReady(True)
                    com_PSoC.setBaudRate(115200)
                    # com_PSoC.readyRead.connect(myWin.com_PSoC_Receive_Data)
                    print("PSoC " + com_PSoC_name + " open success.")
                else:
                    print("PSoC " + com_PSoC_name + " open failed.")
                pass

            # FPGA port initialize
            elif (info.portName() == com_FPGA_name):
                com_FPGA.setPort(info)
                if com_FPGA.open(QSerialPort.ReadWrite):
                    com_FPGA.setDataTerminalReady(True)
                    com_FPGA.setBaudRate(115200)
                    com_FPGA.readyRead.connect(myWin.com_Receive_Data_From_FPGA)
                    print("FPGA " + com_FPGA_name + " open success.")
                else:
                    print("FPGA " + com_FPGA_name + " open fault.")
                pass

        sys.exit(app.exec_())
        
        pass
    pass