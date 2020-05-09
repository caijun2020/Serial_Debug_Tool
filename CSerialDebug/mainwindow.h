#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "SerialDebugWidget.h"
#include "QSerialPort.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *e);

private:
    Ui::MainWindow *ui;

    QSerialPort *serialPort;
    SerialDebugWidget *serialDebugW;
};

#endif // MAINWINDOW_H
