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
                qDebug() << "Parsed serial input: " << str;

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
                qDebug() << "Parsed serial input: " << str;
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

            //Handle link loss notification
            //e.g. LinkLost: 1 -> 2
            else if (str.contains("LinkLost:")) {
                int lost_src;
                int lost_dest;
                // Get the current scene from the GraphWidget to modify the visual graph
                QGraphicsScene *scene = widget->scene();

                //Extract node IDs from the string
                QStringList list = str.split(QRegExp("\\s"));
                qDebug() << "Parsed serial input: " << str;
                if (!list.isEmpty()) {
                    qDebug() << "List size: " << list.size();
                    for (int i = 0; i < list.size(); ++i) {
                        qDebug() << "List value " << i << ": " << list.at(i);
                        if (list.at(i) == "LinkLost:") {
                            lost_src = list.at(i+1).toInt();
                            lost_dest = list.at(i+3).toInt();
                            qDebug() << "Link lost between nodes: " << lost_src << " and " << lost_dest;
                            
                            // Remove any matching existing edge between the specified nodes
                            for (Edge *existing_edge : edges) {
                                if ((existing_edge->sourceNode() == nodes.at(lost_src)) &&
                                    (existing_edge->destNode() == nodes.at(lost_dest))) {
                                    scene->removeItem(existing_edge);
                                }
                            }

                            // Add a new red edge to indicate the lost connection
                            Edge *edge = new Edge(nodes.at(lost_src),
                                      nodes.at(lost_dest), 1);
                            scene->addItem(edge);
                            edges.push_back(edge);
                        }
                    }
                }
            }
            this->repaint();    // Force the GUI to refresh and reflect the latest topology changes
            str.clear();        // Reset the input buffer for the next line of serial data
        }
    }
}

//Setup the dock widget and place each node in its initial position according to their roles (master, cluster heads, normal nodes)
void MainWindow::createDockWindows()
{
    // Create a dock area labeled "Network" and allow docking on left/right sides
    QDockWidget *dock = new QDockWidget(tr("Network"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    // Access the graphics scene for adding nodes
    QGraphicsScene *scene = widget->scene();

    // Add all nodes into the graphics scene for visualization
    for (int i = 0; i < nodes.size(); ++i) {
        scene->addItem(nodes.at(i));
    }

    // Position the master node at the top center
    nodes.at(0)->setPos(0, -200);

    // Arrange cluster heads in the middle layer, spaced horizontally
    nodes.at(1)->setPos(-200, 0);
    nodes.at(2)->setPos(0, 0);
    nodes.at(3)->setPos(200, 0);

    // Place remaining nodes under their respective cluster heads
    // Group 1: Nodes under cluster head 1
    nodes.at(4)->setPos(-250, 150);
    nodes.at(5)->setPos(-150, 150);

    // Group 2: Nodes under cluster head 2
    nodes.at(6)->setPos(-50, 150);
    nodes.at(7)->setPos(50, 150);

    // Group 3: Node under cluster head 3
    nodes.at(8)->setPos(200, 150);

    // Put the GraphWidget into the dock and add it to the main window
    dock->setWidget(widget);
    addDockWidget(Qt::RightDockWidgetArea, dock);
}


void MainWindow::send(QByteArray data) {
    uart->send(data);
}

// Constructor for GraphWidget, initializes the view and its scene
GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-400, -400, 800, 800);          // Define the visible area of the scene 
    setScene(scene);
    setCacheMode(CacheBackground);                      // Cache the background for better performance
    setViewportUpdateMode(BoundingRectViewportUpdate);  // Only update areas that have changed, for better performance
    setRenderHint(QPainter::Antialiasing);              // Turn on anti-aliasing to make shapes and lines smoother
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(800, 800);                           // Set a minimum window size to avoid too small a view

    // Set the title shown when this view is opened as a window
    setWindowTitle(tr("Network Topology"));
}

// Start the layout refresh timer if it's not already running
void GraphWidget::startLayoutRefresh()
{
    if (!timerId)
        timerId = startTimer(50);  // 50 ms interval for layout updates
}

void GraphWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    QVector<Node *> nodes;
    const QList<QGraphicsItem *> items = scene()->items();
    for (QGraphicsItem *item : items) {
        if (Node *node = qgraphicsitem_cast<Node *>(item))
            nodes << node;
    }

    // Ask each node to calculate the forces acting on it (repulsion/attraction)
    for (Node *node : qAsConst(nodes))
        node->calculateForces();

    // Attempt to apply the computed positions, check if any node actually moved
    bool layoutChanged = false;
    for (Node *node : qAsConst(nodes)) {
        if (node->advancePosition())
            layoutChanged = true;
    }

    // If no node changed its position, stop the timer to save resources
    if (!layoutChanged) {
        killTimer(timerId);
        timerId = 0;
    }
}

void Node::calculateForces()
{
    if (!scene() || scene()->mouseGrabberItem() == this) {
        newPos = pos();
        return;
    }

    QRectF sceneRect = scene()->sceneRect();
    newPos = pos(); 
    newPos.setX(qMin(qMax(newPos.x(), sceneRect.left() + 10), sceneRect.right() - 10));
    newPos.setY(qMin(qMax(newPos.y(), sceneRect.top() + 10), sceneRect.bottom() - 10));
}

bool Node::advancePosition()
{
    if (newPos == pos())
        return false;

    setPos(newPos);
    return true;
}

// Custom background painting for the topology graph view
void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);  // This implementation ignores the suggested redraw area

    // Define the full scene area to apply the background
    const QRectF sceneBounds = sceneRect();

    // Draw a background gradient from top-left to bottom-right
    QLinearGradient backgroundGradient(sceneBounds.topLeft(), sceneBounds.bottomRight());
    backgroundGradient.setColorAt(0, Qt::white);       // Lighter at the top-left
    backgroundGradient.setColorAt(1, Qt::lightGray);   // Darker at the bottom-right

    // Fill the visible area with the gradient
    painter->fillRect(rect.intersected(sceneBounds), backgroundGradient);

    // Draw the border of the scene area
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneBounds);
}

//Not yet edited!!!!!
//--------------------------------------------------------------------

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
