#include "mainwindow.h"
#include "ui_mainwindow.h"

//Libraries needed for implementation
#include <qdebug.h>
#include <math.h>
#include <vector>

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QFile>
#include <QDir>
#include <QFileDialog>

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QtMath>
#include <QtWidgets>

#include <QMessageBox>
#include <QPixmap>

//Enable time display when message received
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {

    // m_record = false;

    //set up ui from .ui file
    ui->setupUi(this);

    // Get available COM Ports
    this->uart = new Uart(this);
    QList<QextPortInfo> ports = uart->getUSBPorts();
    for (int i = 0; i < ports.size(); i++) {
        ui->comboBox_Interface->addItem(ports.at(i).portName.toLocal8Bit().constData());
    }

    //Warning message if no USB port is found
    if (ui->comboBox_Interface->count() == 0){
        ui->textEdit_Status->insertPlainText("No USB ports available.\n");
    }

    //Initialize the topology graph
    widget = new GraphWidget;
    for (int i = 0; i<8; ++i){
        nodes.push_back(new Node(widget, this));
    }
    createDockWindows();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

//Once this button is pressed, device connected to the PC could be opened and data could be seen
void MainWindow::on_pushButton_open_clicked() {

    port.setQueryMode(QextSerialPort::EventDriven);
    //Port name has to be compatible with linux convention
    port.setPortName("/dev/" + ui->comboBox_Interface->currentText());
    port.setBaudRate(BAUD115200);
    port.setFlowControl(FLOW_OFF);
    port.setParity(PAR_NONE);
    port.setDataBits(DATA_8);
    port.setStopBits(STOP_1);
    port.open(QIODevice::ReadWrite);

    if (!port.isOpen())
    {
        error.setText("Open port unsuccessful, try again!");
        error.show();
        return;
    }

    //Initialize UART connection
    QString portname = "/dev/";
    portname.append(ui->comboBox_Interface->currentText());
    uart->open(portname);
    if (!uart->isOpen())
    {
        error.setText("Open UART port unsuccessful, try again!");
        error.show();
        return;
    }

    //Once data is sensed, trigger mainwindow receive function and read serial data
    QObject::connect(&port, SIGNAL(readyRead()), this, SLOT(receive()));

    ui->pushButton_close->setEnabled(true);         //Enable close button
    ui->pushButton_open->setEnabled(false);         //Disable open button
    ui->comboBox_Interface->setEnabled(false);      //Disable dropdown interfaces
}

void MainWindow::on_pushButton_close_clicked() {
    if (port.isOpen()) port.close();
    if (uart->isOpen()) uart->close();
    ui->pushButton_close->setEnabled(false);
    ui->pushButton_open->setEnabled(true);
    ui->comboBox_Interface->setEnabled(true);
}

//Message example: Node: 1 SensorType: 1 Value: 12
void MainWindow::receive(QString str) {
    static QString str;
    char ch;
    while (port.getChar(&ch)){
        str.append(ch);

        //Detect end of line and decode from here
        if (ch == '\n'){
            str.remove("\n", Qt::CaseSensitive);

            // Prepend current time to the incoming message string
            QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
            QString logLine = timestamp + " | " + str;

            // Append the time-stamped message to the QTextEdit for status display
            ui->textEdit_Status->append(logLine);
            ui->textEdit_Status->ensureCursorVisible();  // Auto-scroll to the latest line

            if(str.contains("SensorType:")){
                QStringList list = str.split(QRegExp("\\s"));
                //Display information in Qt console
                qDebug() << "Received via Serial Link: " << str;

                //Deals with different type of sensors
                if(!list.isEmpty()){
                    int i = 0;
                    int sensorType = list.at(i+3).toInt();
                    int nodeID = list.at(i+1).toInt();
                    double value = list.at(i+5).toInt();
                    qDebug() << "List size " << list.size();
                    qDebug() << "List value "<< i <<" "<< list.at(i);
                    switch(sensorType){
                        //Light sensor
                        case 1:
                            nodeStates[nodeID].light = value;
                            ui->value_light->display(value);
                            qDebug() << "Val light" << QString::number(value);
                            break;
                        //Distance sensor
                        case 2:
                            nodeStates[nodeID].distance = value;
                            ui->value_distance->display(value);
                            qDebug() << "Val distance" << QString::number(value);
                            break;
                    }
                        evaluateParkingStatus(nodeID);
                }
            }

            //Change of network topology -- new link is added
            //e.g. NewLink: 1 -> 2
            else if (str.contains("NewLink:")){
                int new_src;
                int new_dest;
                // Get the current scene from the GraphWidget to modify the visual graph
                QGraphicsScene *scene = widget->scene();

                //Extract node IDs from the string
                QStringList list = str.split(QRegExp("\\s"));
                qDebug() << "Received via Serial Link: " << str;
                if (!list.isEmpty()) {
                    qdebug() << "List size: " << list.size();
                    for (int i = 0; i < list.size(); ++i) {
                        qdebug() << "List value " << i << ": " << list.at(i);
                        if(list.at(0) == "NewLink:") {
                            new_src = list.at(1).toInt();
                            new_dest = list.at(3).toInt();
                            printf("%d\n",new_src);
                            printf("%d\n",new_dest);
                            qDebug() << "Link between nodes: " << node1 << " and " << node2;
                            widget->addEdge(node1, node2);
                        }
                    } 
                }
            }
        }
    }
}

void MainWindow::send(QByteArray data) {
    uart->send(data);
}

//Not yet edited!!!!!
//--------------------------------------------------------------------
void MainWindow::packet_received(QByteArray str) {
    if (str.length() == 0) return;

    switch (str.at(0)) {
    case SERIAL_PACKET_TYPE_POWER_TEST:
        RadioTest radioTest;
        radioTest.number = str.at(1);
        radioTest.tx_power = str.at(2);
        radioTest.rssi = str.at(3);

        int row = ui->tableWidget->rowCount();
        row++;
        ui->tableWidget->setRowCount(row);
        ui->tableWidget->scrollToBottom();
        QTableWidgetItem *index = new QTableWidgetItem(QString::number(radioTest.number));
        ui->tableWidget->setItem(row-1, 0, index);
        QTableWidgetItem *distance = new QTableWidgetItem(QString::number(ui->doubleSpinBox_distance->value())); //tr("%1 %2").arg(row).arg(str)
        ui->tableWidget->setItem(row-1, 1, distance);
        QTableWidgetItem *TxPower = new QTableWidgetItem(QString::number(radioTest.tx_power));
        ui->tableWidget->setItem(row-1, 2, TxPower);
        QTableWidgetItem *RxPower = new QTableWidgetItem(QString::number(radioTest.rssi));
        ui->tableWidget->setItem(row-1, 3, RxPower);

        break;
    }
}

void MainWindow::on_pushButton_start_clicked() {
    if (!m_record) {
        ui->pushButton_start->setEnabled(false);
        ui->doubleSpinBox_distance->setEnabled(false);
        ui->pushButton_stop->setEnabled(true);
        m_record = true;
    }
}

void MainWindow::on_pushButton_stop_clicked() {
    if (m_record) {
        ui->pushButton_stop->setEnabled(false);
        ui->pushButton_start->setEnabled(true);
        ui->doubleSpinBox_distance->setEnabled(true);
        m_record = false;
    }
}

//Copytable function deleted
void MainWindow::on_pushButton_clearTable_clicked() {
    ui->tableWidget->setRowCount(0);
}

void MainWindow::on_pushButtonSetPower_clicked()
{
    QByteArray data = QByteArray((int) 6, (char) 0);
    data.insert(0, "cmd:");
    data[4] = SERIAL_PACKET_TYPE_CONFIGURE_TEST;
    data[5] = (signed char) ui->spinBoxPower->value();
    this->send(data);
}

//Function to evaluate parking lot status
void MainWindow::evaluateParkingStatus(int nodeID)
{
    const auto &state = nodeStates[nodeID];

    if (state.light >= 0 && state.distance >= 0) {
        if (state.light < 20 && state.distance < 30) {
            QPixmap image(":images/occupied.jpg");
            pop_up.setText("Node %1: OCCUPIED").arg(nodeID);
            pop_up.setIconPixmap(image);
            pop_up.show();
            ui->textEdit_Status->append(QString("Node %1: Car detected - spot occupied").arg(nodeID));
        } else {
            QPixmap image(":images/empty.jpg");
            pop_up.setText("Node %1: EMPTY").arg(nodeID);
            pop_up.setIconPixmap(image);
            pop_up.show();
            ui->textEdit_Status->append(QString("Node %1: No car - spot available").arg(nodeID));
        }
    }
}
