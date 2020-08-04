Serial Debug Tool

Development by Qt 4.8.1

Version: V1.0 2020-May-09
1. Change class name SerialCommunication to QSerialPort
2. Add a new signal newDataTx(QByteArray) in class QSerialPort 


Version: V1.0 2020-Apr-23
1. Implemented serial port basic transmit/receive functions
2. Automatically get available ports on Windows OS
3. Baudrate(9600,19200,38400,57600,115200), Parity(None/Odd/Even), Databits(5/6/7/8, Stopbits(1/1.5/2), Flow type(Off/Hardware/XonXoff)
4. Tx and Rx support Chinese character display
5. Support timestamp display
6. Show Tx and Rx bytes
7. Support auto send, send interval is configured from 1 to 100000 ms


Version: V1.0 2020-Aug-04
1. Add hex format and auto clear in UI SerialDebugWidget
2. Use signals and slots to start/stop timerForRx in class QSerialPort for multi-thread programming
3. Add signals and slots startPolling(), stopPolling() and slots startPollingTimer() and stopPollingTimer()  in class QSerialPort


