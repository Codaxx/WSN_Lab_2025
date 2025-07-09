#include "mainwindow.h"
#include "ui_mainwindow.h"

//Libraries needed for implementation
#include <qdebug.h>
#include <math.h>
#include <vector>

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

    //set up ui from .ui file
    ui->setupUi(this);

    // Collect all checkboxes into a vector
    QVector<QCheckBox *> checkboxes = {
        ui->park1, ui->park2, ui->park3, ui->park4, ui->park5, ui->park6, ui->park7,
        ui->work1, ui->work2, ui->work3, ui->work4, ui->work5, ui->work6, ui->work7
    };

    // Disable mouse click for each checkbox
    for (QCheckBox *checkbox : checkboxes) {
        checkbox->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        checkbox->setFocusPolicy(Qt::NoFocus);  // Optional: Prevent keyboard focus
    }

    // Get available COM Ports
    // this->uart = new Uart(this);
    // QList<QextPortInfo> ports = uart->getUSBPorts();
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();
    for (int i = 0; i < ports.size(); i++) {
        QString portName = ports.at(i).portName;
        if (portName.contains("ttyACM", Qt::CaseSensitive)){
            ui->comboBox_Interface->addItem(portName.toLocal8Bit().constData());
        }
    }

    //Warning message if no USB port is found
    if (ui->comboBox_Interface->count() == 0){
        ui->textEdit_Status->insertPlainText("No USB ports available.\nPlease have another try! \n");
    }

    //Initialize the topology graph
    widget = new GraphWidget;
    for (int i = 0; i<9; ++i){
        nodes.push_back(new Node(widget, this));
    }
    createDockWindows();

    this->showMaximized();
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

    // Initialize UART connection
    // QString portname = "/dev/";
    // portname.append(ui->comboBox_Interface->currentText());
    // uart->open(portname);
    // if (!uart->isOpen())
    // {
    //     error.setText("Open UART port unsuccessful, try again!");
    //     error.show();
    //     return;
    // }

    //Once data is sensed, trigger mainwindow receive function and read serial data
    QObject::connect(&port, SIGNAL(readyRead()), this, SLOT(receive()));

    ui->pushButton_close->setEnabled(true);         //Enable close button
    ui->pushButton_open->setEnabled(false);         //Disable open button
    ui->comboBox_Interface->setEnabled(false);      //Disable dropdown interfaces
}

void MainWindow::on_pushButton_close_clicked() {
    if (port.isOpen()) port.close();
    // if (uart->isOpen()) uart->close();
    ui->pushButton_close->setEnabled(false);
    ui->pushButton_open->setEnabled(true);
    ui->comboBox_Interface->setEnabled(true);
}

//Message example: Node: 1 SensorType: 1 Value: 12
void MainWindow::receive() {
    static QString str;
    char ch;
    while (port.getChar(&ch)){
        str.append(ch);

        //Detect end of line and decode from here
        if (ch == '\n'){
            str.remove("\n", Qt::CaseSensitive);
            str.remove("\r");

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
                    // When nodeid is fetched, set this node as active
                    switch (nodeID) {
                        case 1: ui->work1->setChecked(true); break;
                        case 2: ui->work2->setChecked(true); break;
                        case 3: ui->work3->setChecked(true); break;
                        case 4: ui->work4->setChecked(true); break;
                        case 5: ui->work5->setChecked(true); break;
                        case 6: ui->work6->setChecked(true); break;
                        case 7: ui->work7->setChecked(true); break;
                    }
                    double value = list.at(i+5).toInt();
                    qDebug() << "List size " << list.size();
                    qDebug() << "List value "<< i <<" "<< list.at(i);
                    switch(sensorType){
                        //Light sensor
                        case 1:
                            nodeStates[nodeID].light = value;
                            switch (nodeID) {
                                case 1: ui->value_light_1->display(value); break;
                                case 2: ui->value_light_2->display(value); break;
                                case 3: ui->value_light_3->display(value); break;
                                case 4: ui->value_light_4->display(value); break;
                                case 5: ui->value_light_5->display(value); break;
                                case 6: ui->value_light_6->display(value); break;
                                case 7: ui->value_light_7->display(value); break;
                            }
                            qDebug() << "Val light" << QString::number(value);
                            break;
                        //Distance sensor
                        case 2:
                            nodeStates[nodeID].distance = value;
                            switch (nodeID) {
                                case 1: ui->value_distance_1->display(value); break;
                                case 2: ui->value_distance_2->display(value); break;
                                case 3: ui->value_distance_3->display(value); break;
                                case 4: ui->value_distance_4->display(value); break;
                                case 5: ui->value_distance_5->display(value); break;
                                case 6: ui->value_distance_6->display(value); break;
                                case 7: ui->value_distance_7->display(value); break;
                            }
                            qDebug() << "Val distance" << QString::number(value);
                            break;
                    }
                    evaluateParkingStatus(nodeID);
                    updateGraphBoxStyle();
                }
            }

            //Change of network topology -- new link is added
            //e.g. NewLink 1 -> 2
            else if (str.contains("Newlink")){
                int new_src;
                int new_dest;
                // Get the current scene from the GraphWidget to modify the visual graph
                QGraphicsScene *scene = widget->scene();

                //Extract node IDs from the string
                QStringList list = str.split(QRegExp("\\s"));
                qDebug() << "Parsed serial input: " << str;
                if (!list.isEmpty()) {
                    qDebug() << "List size: " << list.size();
                    for (int i = 0; i < list.size(); i++) {
                        // qDebug() << "List value " << i << ": " << list.at(i);
                        new_src = list.at(1).toInt();
                        //If nodeID is 255, then it should be node 0 which is the master node, and all other node numbers should be added 1
                        new_src = (new_src == 255) ? 0 : new_src+1;
                        new_dest = list.at(3).toInt();
                        new_dest = (new_dest == 255) ? 0 : new_dest+1;
                        printf("%d\n",new_src);
                        printf("%d\n",new_dest);
                        qDebug() << "New link between nodes: " << new_src << " and " << new_dest;

                        for(Edge *existing_edge: edges){
                            if((existing_edge->sourceNode() == nodes.at(new_src))
                                     && (existing_edge->destNode() == nodes.at(new_dest))){
                                scene->removeItem(existing_edge);
                            }
                        }

                        // Add a new green edge to indicate the lost connection
                        Edge *edge = new Edge(nodes.at(new_src),
                                            nodes.at(new_dest), 0);
                        scene->addItem(edge);
                        edges.push_back(edge);
                    } 
                }
            }

            // Change to only receive LinkLost: 1, delete all edges manually
            // Handle node loss notification
            //e.g. LinkLost: 1
            else if (str.contains("LinkLost:")) {
                int lost_src;
                // int lost_dest;
                // Get the current scene from the GraphWidget to modify the visual graph
                QGraphicsScene *scene = widget->scene();

                //Extract node IDs from the string
                QStringList list = str.split(QRegExp("\\s"));
                qDebug() << "Parsed serial input: " << str;
                if (!list.isEmpty()) {

                    qDebug() << "List size: " << list.size();

                    for (int i = 0; i < list.size(); i++) {
                        qDebug() << "List value " << i << ": " << list.at(i);
                        
                        lost_src = list.at(1).toInt();
                        // Uncheck the corresponding checkbox for the lost source node
                        switch (lost_src) {
                            case 1: ui->work1->setChecked(false); break;
                            case 2: ui->work2->setChecked(false); break;
                            case 3: ui->work3->setChecked(false); break;
                            case 4: ui->work4->setChecked(false); break;
                            case 5: ui->work5->setChecked(false); break;
                            case 6: ui->work6->setChecked(false); break;
                            case 7: ui->work7->setChecked(false); break;
                        }
                        // lost_dest = list.at(i+3).toInt();
                        // qDebug() << "Link lost between nodes: " << lost_src << " and " << lost_dest;
                        qDebug() << "Node " << lost_src << " is offline, removing all related edges";
                            
                        // Remove any matching existing edge from and to the lost node
                        for (Edge *existing_edge : edges) {
                            if ((existing_edge->sourceNode() == nodes.at(lost_src)) ||
                                (existing_edge->destNode() == nodes.at(lost_src))) {
                                scene->removeItem(existing_edge);
                            }
                        }

                        // Add a new red edge to indicate the lost connection
                        // Edge *edge = new Edge(nodes.at(lost_src),
                        //         nodes.at(lost_dest), 1);
                        // scene->addItem(edge);
                        // edges.push_back(edge);
                    }
                }
            }

            //e.g. ClusterHead: 2 3 6
            else if (str.contains("ClusterHead:")) {
                // Split the message
                QStringList list = str.split(QRegExp("\\s"));
                qDebug() << "Parsed serial input: " << str;

                for (int i = 0; i < list.size(); i++) {
                    qDebug() << "List value " << i << ": " << list.at(i);
                }

                head1 = list.at(1).toInt();
                head2 = list.at(2).toInt();
                head3 = list.at(3).toInt();   

                nodes.at(head1)->setPos(-175, -75);  
                nodes.at(head2)->setPos(0, 0);    
                nodes.at(head3)->setPos(175, -50);   
                
                qDebug() << "Assigned cluster heads:" << head1 << head2 << head3;
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
    QDockWidget *dock = new QDockWidget(tr("Network Topology"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    // Access the graphics scene for adding nodes
    QGraphicsScene *scene = widget->scene();

    // Add all nodes into the graphics scene for visualization
    for (int i = 0; i < 9; i++) {
        scene->addItem(nodes.at(i));
    }

    // Position the master node at the top center
    nodes.at(0)->setPos(0, -200);

    // Arrange cluster heads in the middle layer, spaced horizontally
    nodes.at(1)->setPos(-175, -75);
    nodes.at(2)->setPos(0, 0);
    nodes.at(3)->setPos(175, -50);

    // Place remaining nodes under their respective cluster heads
    // Group 1: Nodes under cluster head 1
    nodes.at(4)->setPos(-250, 100);
    nodes.at(5)->setPos(-150, 75);

    // Group 2: Nodes under cluster head 2
    nodes.at(6)->setPos(-50, 150);
    nodes.at(7)->setPos(50, 150);

    // Group 3: Node under cluster head 3
    nodes.at(8)->setPos(200, 125);

    // Put the GraphWidget into the dock and add it to the main window
    dock->setWidget(widget);
    addDockWidget(Qt::RightDockWidgetArea, dock);
}


void MainWindow::send(QByteArray data) {
    // uart->send(data);
    port.write(data);

    qint64 bytesWritten = port.write(data);
    qDebug() << "Sent" << bytesWritten << "bytes:" << data;
}

// Set graphbox to green if parking is available and red if slot is not available or node is not working
void MainWindow::updateGraphBoxStyle(){
    bool allChecked = ui->park1->isChecked() && ui->work1->isChecked();
    if(allChecked){
        ui->slot_1->setStyleSheet("QGroupBox { background-color: lightgreen; }");
    } else {
        ui->slot_1->setStyleSheet("");
    }
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


//Function for handling nodes

Node::Node(GraphWidget *graphWidget, MainWindow *w)
    : graph(graphWidget)
{
    parentWindow = w;
    setFlag(ItemSendsGeometryChanges);
    setCacheMode(DeviceCoordinateCache);
    setZValue(-1);
}

// Return a list of all edges connected to this node
QVector<Edge *> Node::edges() const
{
    return edgeList;
}

void Node::addEdge(Edge *edge)
{
    edgeList << edge;
    edge->adjust();
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

QRectF Node::boundingRect() const
{
    qreal adjust = 2;
    return QRectF( -10 - adjust, -10 - adjust, 23 + adjust, 23 + adjust);
}

QPainterPath Node::shape() const
{
    QPainterPath path;
    path.addEllipse(-10, -10, 20, 20);
    return path;
}

QVariant Node::itemChange(GraphicsItemChange change, const QVariant &value)
{
    switch (change) {
    case ItemPositionHasChanged:
        for (Edge *edge : qAsConst(edgeList))
            edge->adjust();
        graph->startLayoutRefresh();
        break;
    default:
        break;
    };

    return QGraphicsItem::itemChange(change, value);
}

void Node::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // Remove any pen outline initially (used for base fill)
    painter->setPen(Qt::NoPen);

    // Set a dark gray background as the base layer for the node
    painter->setBrush(Qt::darkGray);
    painter->drawEllipse(-7, -7, 20, 20);

    // Create a radial gradient for color effect
    QRadialGradient gradient(-3, -3, 10);

    // Get the list of all nodes from the main window to find this node's index
    std::vector<Node*> nodes = parentWindow->nodes;
    std::vector<Node*>::iterator it = std::find(nodes.begin(), nodes.end(), this);
    int index = std::distance(nodes.begin(), it);

    // Node coloring based on role
    if (index == 0) {
        // Master node: deep blue
        gradient.setColorAt(0, QColor(70, 130, 180));    // Steel Blue
        gradient.setColorAt(1, QColor(30, 60, 90));      // Darker blue
    } 
    else if (index >= 1 && index <= 3) {
        // Cluster heads: light blue
        gradient.setColorAt(0, QColor(135, 206, 250));  // Light Sky Blue
        gradient.setColorAt(1, QColor(70, 130, 180));   // Steel Blue
    } 
    else {
        // Regular nodes: light orange
        gradient.setColorAt(0, QColor(255, 204, 153));  // Light Orange
        gradient.setColorAt(1, QColor(205, 133, 63));   // Peru
    }
    // Apply the gradient to the painter
    painter->setBrush(gradient);

    // Draw the main outline of the node
    painter->setPen(QPen(Qt::black, 0));
    painter->drawEllipse(-10, -10, 20, 20);

    // Draw node ID number on top of the node
    QFont font = painter->font();
    font.setPointSize(12);
    painter->setFont(font);
    painter->drawText(-5, 5, QString::number(index));
}


// Constructor for the Edge class, initializes the edge between two nodes and sets its type
Edge::Edge(Node *sourceNode, Node *destNode, unsigned int edgeType)
    : source(sourceNode), dest(destNode)
{
    setAcceptedMouseButtons(Qt::NoButton);
    source->addEdge(this);
    dest->addEdge(this);

    // Store the edge type (e.g., new link, lost link)
    edge_type = edgeType;

    // Compute the initial positions for rendering the edge
    adjust();
}

Node *Edge::sourceNode() const
{
    return source;
}

Node *Edge::destNode() const
{
    return dest;
}

// Update the source and destination points of the edge for drawing
void Edge::adjust()
{
    // If either node is missing, skip adjustment
    if (!source || !dest)
        return;

    // Compute a straight line between the source and destination node positions
    QLineF line(mapFromItem(source, 0, 0), mapFromItem(dest, 0, 0));
    qreal length = line.length();

    // Notify the graphics system that the item's shape will change
    prepareGeometryChange();

    // Offset the edge endpoints slightly away from the node centers to avoid drawing directly over the node graphics
    if (length > qreal(20.)) {
        QPointF edgeOffset((line.dx() * 10) / length, (line.dy() * 10) / length);
        sourcePoint = line.p1() + edgeOffset;
        destPoint = line.p2() - edgeOffset;
    } else {
        // If nodes are too close, collapse the edge to a single point
        sourcePoint = destPoint = line.p1();
    }
}

// Return the bounding rectangle for this edge, used by Qt to optimize redraws and event detection
QRectF Edge::boundingRect() const
{
    if (!source || !dest)
        return QRectF();

    qreal penWidth = 1;
    qreal extra = (penWidth + arrowSize) / 2.0;

    // Create a rectangle that spans from the source point to the destination point
    QRectF rawBounds(sourcePoint, QSizeF(destPoint.x() - sourcePoint.x(),
                                     destPoint.y() - sourcePoint.y()));

    return rawBounds.normalized().adjusted(-extra, -extra, extra, extra);
}

// Custom painting of the edge line and arrow head
void Edge::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    // If either endpoint is missing, skip drawing
    if (!source || !dest)
        return;

    // Create a straight line from the source to the destination point
    QLineF line(sourcePoint, destPoint);

    // If the line has no length, there's nothing to draw
    if (qFuzzyCompare(line.length(), qreal(0.)))
        return;

    // Set the pen style based on the edge type (color and line style)
    switch (this->edge_type) {
    case 0: painter->setPen(QPen(Qt::green, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        break;
    case 1: painter->setPen(QPen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        break;
    }

    // Draw the main line of the edge
    painter->drawLine(line);

    // Calculate the angle of the line to orient the arrowhead correctly
    double angle = std::atan2(-line.dy(), line.dx());

    // Compute the two side points of the arrowhead triangle at the destination
    QPointF destArrowP1 = destPoint + QPointF(sin(angle - M_PI / 3) * arrowSize,
                                              cos(angle - M_PI / 3) * arrowSize);
    QPointF destArrowP2 = destPoint + QPointF(sin(angle - M_PI + M_PI / 3) * arrowSize,
                                              cos(angle - M_PI + M_PI / 3) * arrowSize);

    // Set the arrowhead fill color to match the edge color
    switch (this->edge_type) {
    case 0: painter->setBrush(Qt::green);       // New link
        break;
    case 1: painter->setBrush(Qt::red);         // Lost link
        break;
    }

    // Draw the arrowhead as a filled triangle
    painter->drawPolygon(QPolygonF() << line.p2() << destArrowP1 << destArrowP2);
}


//Function to evaluate parking lot status
void MainWindow::evaluateParkingStatus(int nodeID)
{
    const auto &state = nodeStates[nodeID];

    if (state.light >= 0 && state.distance >= 0) {
        bool occupied = (state.light < 20 && state.distance < 30);

        // Switch to select the correct checkbox
        switch (nodeID) {
            case 1: ui->park1->setChecked(!occupied); break;
            case 2: ui->park2->setChecked(!occupied); break;
            case 3: ui->park3->setChecked(!occupied); break;
            case 4: ui->park4->setChecked(!occupied); break;
            case 5: ui->park5->setChecked(!occupied); break;
            case 6: ui->park6->setChecked(!occupied); break;
            case 7: ui->park7->setChecked(!occupied); break;
        }

        // Write to text log
        if (occupied) {
            ui->textEdit_Status->append(QString("Node %1: Car detected - spot occupied").arg(nodeID));
        } else {
            ui->textEdit_Status->append(QString("Node %1: No car - spot available").arg(nodeID));
        }
    }
}

void MainWindow::on_pushButtonSetPower_clicked()
{
    QByteArray data = QByteArray((int) 6, (char) 0);
    data.insert(0, "cmd:");
    data[4] = SERIAL_PACKET_TYPE_CONFIGURE_TEST;
    data[5] = (signed char) ui->spinBoxPower->value();
    this->send(data);
}

