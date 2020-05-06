#include "SerialCommunication.h"
#include <QSettings>
#include <QDebug>

#ifdef Q_OS_WIN
    #include <qt_windows.h>
    #include "win_qextserialport.h"
#else
    #include "posix_qextserialport.h"
#endif


//#define __SERIAL_CONSOLE_DEBUG__

SerialCommunication::SerialCommunication(COM_PORT_INIT_DATA *initData) :
    comPort(NULL)
{
    init();

    open(initData);
}

SerialCommunication::SerialCommunication() :
    comPort(NULL)
{
    init();
}

SerialCommunication::~SerialCommunication()
{
    deInit();
}

void SerialCommunication::run()
{
}


void SerialCommunication::init()
{
    // Reset tx/rx bytes count
    resetTxRxCnt();

    comInitData = new struct COM_PORT_INIT_DATA;

    // Init Tx buffer
    txBuffer = new char [TX_BUF_SIZE];
    memset(txBuffer, 0, TX_BUF_SIZE);

    // Init Rx buffer
    rxLoopBuffer = new LoopBuffer(RX_BUF_SIZE);
    //rxLoopBuffer->setMsgEndChar(MSG_END_FLAG_1);

    timerForRx = new QTimer(this);
    connect(timerForRx, SIGNAL(timeout()), this, SLOT(readDataFromCOM()));
}

void SerialCommunication::deInit()
{
    timerForRx->stop();

    if(comPort != NULL)
    {
        comPort->close();
    }

    delete comPort;
    delete comInitData;
    delete []txBuffer;
    delete rxLoopBuffer;
    delete timerForRx;
}

bool SerialCommunication::open(struct COM_PORT_INIT_DATA *initData)
{
    bool ret = false;
    QString portName = "";

    // Load COM port setting
    memcpy(comInitData, initData, sizeof(struct COM_PORT_INIT_DATA));

    // Init Tx buffer
    memset(txBuffer, 0, TX_BUF_SIZE);

    portName.append(comInitData->port);
    portName.prepend("\\\\.\\");   // Which is used to open the port number >= COM10

    close();

#ifdef Q_OS_WIN
    comPort = new Win_QextSerialPort(portName, QextSerialBase::Polling); // Polling Mode
#else
    comPort = new Posix_QextSerialPort(portName, QextSerialBase::Polling); // Polling Mode
#endif

    comPort->open(QIODevice::ReadWrite); // Read Write Mode
    comPort->setBaudRate(comInitData->baudrate);  // Baudrate
    comPort->setDataBits(comInitData->databits);  // Data bits
    comPort->setParity(comInitData->parity);  // Parity
    comPort->setStopBits(comInitData->stopbits); // Stopbit
    comPort->setFlowControl(comInitData->flowtype); // Flow control, FLOW_OFF, FLOW_HARDWARE, FLOW_XONXOFF
    comPort->setTimeout(10);    // Set timeout=10ms

    // If COM port is open, then start to read data from COM port
    if(isComPortOpen())
    {
        ret = true;
        timerForRx->start(50);  //timeOut = 50ms
    }

    return ret;
}

void SerialCommunication::close()
{
    timerForRx->stop();

    if(comPort != NULL)
    {
        comPort->close();
        delete comPort;
        comPort = NULL;
    }
}

bool SerialCommunication::isComPortOpen()
{
    bool ret = false;

    if(comPort != NULL)
    {
        ret = comPort->isOpen();
    }

    return ret;
}

int SerialCommunication::writeData(const char* txData, int len)
{
    int ret = 0;

#ifdef __SERIAL_CONSOLE_DEBUG__
    for(int i = 0; i < len; i++)
    {
        qDebug() << "txData[" << i << "]=" << (quint8)txData[i];
    }
#endif

    if(comPort != NULL)
    {
        if(comPort->isOpen()
                && comPort->isWritable())
        {
            ret = comPort->write(txData, len);
            txTotalBytesSize += len;
        }
        else
        {
            qDebug() << "comPort not open";
        }
    }

    return ret;
}

void SerialCommunication::readDataFromCOM()
{
    if(NULL == comPort)
    {
        return;
    }

    if(comPort->isOpen())
    {
        QByteArray temp = comPort->readAll();   //Read all the data to the serial port buffer

        if(!temp.isEmpty())
        {
            rxLoopBuffer->writeData(temp);

            rxTotalBytesSize += temp.length();

            // Emit signal
            emit newDataReady(temp);
        }
    }
}

void SerialCommunication::setBaudRate(BaudRateType baudrate)
{
    if(NULL == comPort)
    {
        return;
    }

    comInitData->baudrate = baudrate;
    comPort->setBaudRate(comInitData->baudrate);
}

void SerialCommunication::setParity(ParityType parity)
{
    if(NULL == comPort)
    {
        return;
    }

    comInitData->parity = parity;
    comPort->setParity(comInitData->parity);
}

void SerialCommunication::setDataBits(DataBitsType databits)
{
    if(NULL == comPort)
    {
        return;
    }

    comInitData->databits = databits;
    comPort->setDataBits(comInitData->databits);
}

void SerialCommunication::setStopBits(StopBitsType stopbits)
{
    if(NULL == comPort)
    {
        return;
    }

    comInitData->stopbits = stopbits;
    comPort->setStopBits(comInitData->stopbits);
}

void SerialCommunication::setFlowControl(FlowType flowtype)
{
    if(NULL == comPort)
    {
        return;
    }

    comInitData->flowtype = flowtype;
    comPort->setFlowControl(comInitData->flowtype); //flow control, FLOW_OFF, FLOW_HARDWARE, FLOW_XONXOFF
}

QStringList SerialCommunication::getAvailablePorts()
{
    QStringList portList;
    portList.clear();

#ifdef Q_OS_WIN
    QString path = "HKEY_LOCAL_MACHINE\\HARDWARE\\DEVICEMAP\\SERIALCOMM";
    QSettings settings(path, QSettings::NativeFormat);
    QStringList key = settings.allKeys();

    for(int i = 0; i < key.size(); i++)
    {
        QString port = getComInfoFromReg(i, "value");
        if(!port.isEmpty())
        {
            portList.append(port);
        }
    }
#endif

    return portList;
}

#ifdef Q_OS_WIN
QString SerialCommunication::getComInfoFromReg(int index, QString keyorvalue)
{
    QString commresult = "";
    HKEY hKey;
    QString keymessage;
    QString valuemessage;
    DWORD keysize, type, valuesize;
    wchar_t keyname[256] = {0};
    char keyvalue[256] = {0};

    if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_READ |  KEY_QUERY_VALUE, &hKey) != 0)
    {
        //qDebug() << "RegOpenKeyEx failed";
        return commresult;
    }

    keysize = sizeof(keyname);
    valuesize = sizeof(keyvalue);

    if(RegEnumValue(hKey, index, keyname, &keysize, 0, &type, (BYTE*)keyvalue, &valuesize) == 0)
    {
        keymessage.append(QString::fromStdWString(keyname));

        for(int j = 0; j < (int)valuesize; j++)
        {
            if (keyvalue[j] != 0x00)
            {
                valuemessage.append(keyvalue[j]);
            }
        }

        //qDebug() << "keymessage = " << keymessage;
        //qDebug() << "valuemessage = " << valuemessage;

        if(keyorvalue == "key")
        {
            commresult = keymessage;
        }

        if(keyorvalue == "value")
        {
            commresult = valuemessage;
        }
    }

    RegCloseKey(hKey);  // Close reg

    return commresult;
}
#endif

uint32_t SerialCommunication::getTotalTxBytes() const
{
    return txTotalBytesSize;
}

uint32_t SerialCommunication::getTotalRxBytes() const
{
    return rxTotalBytesSize;
}

void SerialCommunication::resetTxRxCnt()
{
    txTotalBytesSize = 0;
    rxTotalBytesSize = 0;
}

bool SerialCommunication::getUndealData(uint8_t *dataP, uint32_t &len)
{
    QByteArray temp;
    bool ret = getUndealData(temp);

    if(ret)
    {
        len = temp.size();
        memcpy(dataP, temp.data(), temp.size());
    }

    return ret;
}

bool SerialCommunication::getUndealData(QByteArray &data)
{
    bool ret = false;

    data = rxLoopBuffer->readAll();
    if(!data.isEmpty())
    {
        ret = true;
    }

    return ret;
}
