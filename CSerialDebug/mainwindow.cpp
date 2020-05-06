#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    serialPort(new SerialCommunication),
    serialDebugW(new SerialDebugWidget)
{
    ui->setupUi(this);

    serialDebugW->setParent(ui->centralWidget);
    serialDebugW->bindComHandler(serialPort);

    // Set Window Title
    this->setWindowTitle(tr("Serial Debug"));

    // Set Menu Bar Version Info
    ui->menuVersion->addAction("V1.0 2020-Apr-23");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete serialPort;
    delete serialDebugW;
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    serialDebugW->resize(ui->centralWidget->size());
}