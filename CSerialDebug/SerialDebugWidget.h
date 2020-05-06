#ifndef SERIALDEBUGWIDGET_H
#define SERIALDEBUGWIDGET_H

#include <QWidget>
#include "SerialCommunication.h"

namespace Ui {
class SerialDebugWidget;
}

class SerialDebugWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit SerialDebugWidget(SerialCommunication *portHandler = NULL, QWidget *parent = 0);
    ~SerialDebugWidget();

    // Bind SerialCommunication handler
    void bindComHandler(SerialCommunication *portHandler);

    // Send data
    void sendData(QByteArray &data);
    void sendData(const char *data, uint32_t len);

public:
    typedef struct
    {
        int value;
        char text[50];
    }COMBOX_LIST;

signals:
    void newDataReady(QByteArray);

protected:
    void resizeEvent(QResizeEvent *e);

private slots:
    void on_pushButton_send_clicked();

    void on_pushButton_open_clicked();

    void updateUI();

    void on_pushButton_reset_clicked();

    void on_pushButton_clear_clicked();

    void updateIncomingData(QByteArray data);

    void autoSendData();

    void on_checkBox_autoSend_clicked(bool checked);

    void on_checkBox_timeStamp_clicked(bool checked);

private:

    Ui::SerialDebugWidget *ui;

    SerialCommunication *serialPort;    // COM port handler
    struct COM_PORT_INIT_DATA *comInitData; // COM port init data

    QString widgetFontType; // Store the font type of widget
    int widgetFontSize;     // Store the font size of widget

    bool isOpenFlag;    // True: COM port is open
    bool timeStampFlag; // This flag is used to enable timestamp info

    QTimer refreshUITimer;
    QTimer autoSendTimer;

    void initWidgetFont();  // Init the Font type and size
    void initWidgetStyle(); // Init widget style

    // Enable/Disable funtion UI
    void setFunctionUI(bool enable);

    // Update Rx data in textBrowser
    void updateLogData(QString logStr);
};

#endif // SERIALDEBUGWIDGET_H
