/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.13
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QTextEdit *textEdit_Status;
    QComboBox *comboBox_Interface;
    QLabel *label;
    QPushButton *pushButton_open;
    QPushButton *pushButton_close;
    QLabel *label_3;
    QPushButton *pushButtonSetPower;
    QSpinBox *spinBoxPower;
    QGroupBox *slot_2;
    QLabel *light2;
    QLCDNumber *value_light_2;
    QLabel *distance2;
    QLCDNumber *value_distance_2;
    QCheckBox *park2;
    QCheckBox *work2;
    QGroupBox *slot_1;
    QLabel *light1;
    QLCDNumber *value_light_1;
    QLabel *distanc1;
    QLCDNumber *value_distance_1;
    QCheckBox *park1;
    QCheckBox *work1;
    QGroupBox *slot_3;
    QLabel *label_17;
    QLCDNumber *value_light_3;
    QLabel *label_18;
    QLCDNumber *value_distance_3;
    QCheckBox *park3;
    QCheckBox *work3;
    QGroupBox *slot_4;
    QLabel *label_19;
    QLCDNumber *value_light_4;
    QLabel *label_20;
    QLCDNumber *value_distance_4;
    QCheckBox *park4;
    QCheckBox *work4;
    QGroupBox *slot_5;
    QLabel *label_21;
    QLCDNumber *value_light_5;
    QLabel *label_22;
    QLCDNumber *value_distance_5;
    QCheckBox *park5;
    QCheckBox *work5;
    QGroupBox *slot_6;
    QLabel *label_23;
    QLCDNumber *value_light_6;
    QLabel *label_24;
    QLCDNumber *value_distance_6;
    QCheckBox *park6;
    QCheckBox *work6;
    QGroupBox *slot_7;
    QLabel *label_25;
    QLCDNumber *value_light_7;
    QLabel *label_26;
    QLCDNumber *value_distance_7;
    QCheckBox *park7;
    QCheckBox *work7;
    QGroupBox *slot_8;
    QLabel *label_29;
    QLCDNumber *value_light_9;
    QLabel *label_30;
    QLCDNumber *value_distance_9;
    QCheckBox *park8;
    QCheckBox *work8;
    QMenuBar *menuBar;
    QMenu *menuParking_Monitor;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1018, 831);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        textEdit_Status = new QTextEdit(centralWidget);
        textEdit_Status->setObjectName(QString::fromUtf8("textEdit_Status"));
        textEdit_Status->setEnabled(true);
        textEdit_Status->setGeometry(QRect(10, 40, 831, 351));
        comboBox_Interface = new QComboBox(centralWidget);
        comboBox_Interface->setObjectName(QString::fromUtf8("comboBox_Interface"));
        comboBox_Interface->setGeometry(QRect(50, 0, 151, 27));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 0, 46, 27));
        pushButton_open = new QPushButton(centralWidget);
        pushButton_open->setObjectName(QString::fromUtf8("pushButton_open"));
        pushButton_open->setGeometry(QRect(210, 0, 75, 27));
        pushButton_close = new QPushButton(centralWidget);
        pushButton_close->setObjectName(QString::fromUtf8("pushButton_close"));
        pushButton_close->setEnabled(false);
        pushButton_close->setGeometry(QRect(290, 0, 75, 27));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(550, 0, 51, 27));
        pushButtonSetPower = new QPushButton(centralWidget);
        pushButtonSetPower->setObjectName(QString::fromUtf8("pushButtonSetPower"));
        pushButtonSetPower->setGeometry(QRect(660, 0, 97, 27));
        spinBoxPower = new QSpinBox(centralWidget);
        spinBoxPower->setObjectName(QString::fromUtf8("spinBoxPower"));
        spinBoxPower->setGeometry(QRect(610, 0, 44, 26));
        spinBoxPower->setMinimum(-40);
        spinBoxPower->setMaximum(8);
        slot_2 = new QGroupBox(centralWidget);
        slot_2->setObjectName(QString::fromUtf8("slot_2"));
        slot_2->setGeometry(QRect(240, 410, 191, 151));
        slot_2->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255)"));
        light2 = new QLabel(slot_2);
        light2->setObjectName(QString::fromUtf8("light2"));
        light2->setGeometry(QRect(0, 30, 81, 18));
        value_light_2 = new QLCDNumber(slot_2);
        value_light_2->setObjectName(QString::fromUtf8("value_light_2"));
        value_light_2->setGeometry(QRect(110, 30, 64, 23));
        value_light_2->setStyleSheet(QString::fromUtf8("border: 1px solid black"));
        distance2 = new QLabel(slot_2);
        distance2->setObjectName(QString::fromUtf8("distance2"));
        distance2->setGeometry(QRect(0, 60, 101, 18));
        value_distance_2 = new QLCDNumber(slot_2);
        value_distance_2->setObjectName(QString::fromUtf8("value_distance_2"));
        value_distance_2->setGeometry(QRect(110, 60, 64, 23));
        value_distance_2->setStyleSheet(QString::fromUtf8("border: 1px solid black"));
        park2 = new QCheckBox(slot_2);
        park2->setObjectName(QString::fromUtf8("park2"));
        park2->setGeometry(QRect(50, 90, 121, 24));
        park2->setFocusPolicy(Qt::NoFocus);
        work2 = new QCheckBox(slot_2);
        work2->setObjectName(QString::fromUtf8("work2"));
        work2->setGeometry(QRect(50, 120, 121, 24));
        work2->setFocusPolicy(Qt::NoFocus);
        slot_1 = new QGroupBox(centralWidget);
        slot_1->setObjectName(QString::fromUtf8("slot_1"));
        slot_1->setGeometry(QRect(10, 410, 191, 151));
        slot_1->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255)"));
        light1 = new QLabel(slot_1);
        light1->setObjectName(QString::fromUtf8("light1"));
        light1->setGeometry(QRect(0, 30, 81, 18));
        value_light_1 = new QLCDNumber(slot_1);
        value_light_1->setObjectName(QString::fromUtf8("value_light_1"));
        value_light_1->setGeometry(QRect(110, 30, 64, 23));
        QFont font;
        font.setBold(true);
        value_light_1->setFont(font);
        value_light_1->setStyleSheet(QString::fromUtf8("border: 1px solid black\n"
""));
        distanc1 = new QLabel(slot_1);
        distanc1->setObjectName(QString::fromUtf8("distanc1"));
        distanc1->setGeometry(QRect(0, 60, 101, 18));
        value_distance_1 = new QLCDNumber(slot_1);
        value_distance_1->setObjectName(QString::fromUtf8("value_distance_1"));
        value_distance_1->setGeometry(QRect(110, 60, 64, 23));
        value_distance_1->setStyleSheet(QString::fromUtf8("border: 1px solid black"));
        park1 = new QCheckBox(slot_1);
        park1->setObjectName(QString::fromUtf8("park1"));
        park1->setGeometry(QRect(50, 90, 121, 24));
        park1->setFocusPolicy(Qt::NoFocus);
        work1 = new QCheckBox(slot_1);
        work1->setObjectName(QString::fromUtf8("work1"));
        work1->setGeometry(QRect(50, 120, 121, 24));
        work1->setFocusPolicy(Qt::NoFocus);
        slot_3 = new QGroupBox(centralWidget);
        slot_3->setObjectName(QString::fromUtf8("slot_3"));
        slot_3->setGeometry(QRect(470, 410, 191, 151));
        slot_3->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255)"));
        label_17 = new QLabel(slot_3);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(0, 30, 91, 18));
        value_light_3 = new QLCDNumber(slot_3);
        value_light_3->setObjectName(QString::fromUtf8("value_light_3"));
        value_light_3->setGeometry(QRect(110, 30, 64, 23));
        value_light_3->setStyleSheet(QString::fromUtf8("border: 1px solid black"));
        label_18 = new QLabel(slot_3);
        label_18->setObjectName(QString::fromUtf8("label_18"));
        label_18->setGeometry(QRect(0, 60, 101, 18));
        value_distance_3 = new QLCDNumber(slot_3);
        value_distance_3->setObjectName(QString::fromUtf8("value_distance_3"));
        value_distance_3->setGeometry(QRect(110, 60, 64, 23));
        value_distance_3->setStyleSheet(QString::fromUtf8("border: 1px solid black"));
        park3 = new QCheckBox(slot_3);
        park3->setObjectName(QString::fromUtf8("park3"));
        park3->setGeometry(QRect(50, 90, 121, 24));
        park3->setFocusPolicy(Qt::NoFocus);
        work3 = new QCheckBox(slot_3);
        work3->setObjectName(QString::fromUtf8("work3"));
        work3->setGeometry(QRect(50, 120, 121, 24));
        work3->setFocusPolicy(Qt::NoFocus);
        slot_4 = new QGroupBox(centralWidget);
        slot_4->setObjectName(QString::fromUtf8("slot_4"));
        slot_4->setGeometry(QRect(690, 410, 191, 151));
        slot_4->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255)"));
        label_19 = new QLabel(slot_4);
        label_19->setObjectName(QString::fromUtf8("label_19"));
        label_19->setGeometry(QRect(0, 30, 91, 18));
        value_light_4 = new QLCDNumber(slot_4);
        value_light_4->setObjectName(QString::fromUtf8("value_light_4"));
        value_light_4->setGeometry(QRect(110, 30, 64, 23));
        value_light_4->setStyleSheet(QString::fromUtf8("border: 1px solid black"));
        label_20 = new QLabel(slot_4);
        label_20->setObjectName(QString::fromUtf8("label_20"));
        label_20->setGeometry(QRect(0, 60, 101, 18));
        value_distance_4 = new QLCDNumber(slot_4);
        value_distance_4->setObjectName(QString::fromUtf8("value_distance_4"));
        value_distance_4->setGeometry(QRect(110, 60, 64, 23));
        value_distance_4->setStyleSheet(QString::fromUtf8("border: 1px solid black"));
        park4 = new QCheckBox(slot_4);
        park4->setObjectName(QString::fromUtf8("park4"));
        park4->setGeometry(QRect(50, 90, 121, 24));
        park4->setFocusPolicy(Qt::NoFocus);
        work4 = new QCheckBox(slot_4);
        work4->setObjectName(QString::fromUtf8("work4"));
        work4->setGeometry(QRect(50, 120, 121, 24));
        work4->setFocusPolicy(Qt::NoFocus);
        slot_5 = new QGroupBox(centralWidget);
        slot_5->setObjectName(QString::fromUtf8("slot_5"));
        slot_5->setGeometry(QRect(10, 590, 191, 151));
        slot_5->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255)"));
        label_21 = new QLabel(slot_5);
        label_21->setObjectName(QString::fromUtf8("label_21"));
        label_21->setGeometry(QRect(0, 30, 81, 18));
        value_light_5 = new QLCDNumber(slot_5);
        value_light_5->setObjectName(QString::fromUtf8("value_light_5"));
        value_light_5->setGeometry(QRect(110, 30, 64, 23));
        value_light_5->setStyleSheet(QString::fromUtf8("border: 1px solid black"));
        label_22 = new QLabel(slot_5);
        label_22->setObjectName(QString::fromUtf8("label_22"));
        label_22->setGeometry(QRect(0, 60, 101, 18));
        value_distance_5 = new QLCDNumber(slot_5);
        value_distance_5->setObjectName(QString::fromUtf8("value_distance_5"));
        value_distance_5->setGeometry(QRect(110, 60, 64, 23));
        value_distance_5->setStyleSheet(QString::fromUtf8("border: 1px solid black"));
        park5 = new QCheckBox(slot_5);
        park5->setObjectName(QString::fromUtf8("park5"));
        park5->setGeometry(QRect(50, 90, 121, 24));
        park5->setFocusPolicy(Qt::NoFocus);
        work5 = new QCheckBox(slot_5);
        work5->setObjectName(QString::fromUtf8("work5"));
        work5->setGeometry(QRect(50, 120, 121, 24));
        work5->setFocusPolicy(Qt::NoFocus);
        slot_6 = new QGroupBox(centralWidget);
        slot_6->setObjectName(QString::fromUtf8("slot_6"));
        slot_6->setGeometry(QRect(240, 590, 191, 151));
        slot_6->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255)"));
        label_23 = new QLabel(slot_6);
        label_23->setObjectName(QString::fromUtf8("label_23"));
        label_23->setGeometry(QRect(0, 30, 91, 18));
        value_light_6 = new QLCDNumber(slot_6);
        value_light_6->setObjectName(QString::fromUtf8("value_light_6"));
        value_light_6->setGeometry(QRect(110, 30, 64, 23));
        value_light_6->setStyleSheet(QString::fromUtf8("border: 1px solid black"));
        label_24 = new QLabel(slot_6);
        label_24->setObjectName(QString::fromUtf8("label_24"));
        label_24->setGeometry(QRect(0, 60, 101, 18));
        value_distance_6 = new QLCDNumber(slot_6);
        value_distance_6->setObjectName(QString::fromUtf8("value_distance_6"));
        value_distance_6->setGeometry(QRect(110, 60, 64, 23));
        value_distance_6->setStyleSheet(QString::fromUtf8("border: 1px solid black"));
        park6 = new QCheckBox(slot_6);
        park6->setObjectName(QString::fromUtf8("park6"));
        park6->setGeometry(QRect(50, 90, 121, 24));
        park6->setFocusPolicy(Qt::NoFocus);
        work6 = new QCheckBox(slot_6);
        work6->setObjectName(QString::fromUtf8("work6"));
        work6->setGeometry(QRect(50, 120, 121, 24));
        work6->setFocusPolicy(Qt::NoFocus);
        slot_7 = new QGroupBox(centralWidget);
        slot_7->setObjectName(QString::fromUtf8("slot_7"));
        slot_7->setGeometry(QRect(470, 590, 191, 151));
        slot_7->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255)"));
        label_25 = new QLabel(slot_7);
        label_25->setObjectName(QString::fromUtf8("label_25"));
        label_25->setGeometry(QRect(0, 30, 81, 18));
        value_light_7 = new QLCDNumber(slot_7);
        value_light_7->setObjectName(QString::fromUtf8("value_light_7"));
        value_light_7->setGeometry(QRect(110, 30, 64, 23));
        value_light_7->setStyleSheet(QString::fromUtf8("border: 1px solid black"));
        label_26 = new QLabel(slot_7);
        label_26->setObjectName(QString::fromUtf8("label_26"));
        label_26->setGeometry(QRect(0, 60, 111, 18));
        value_distance_7 = new QLCDNumber(slot_7);
        value_distance_7->setObjectName(QString::fromUtf8("value_distance_7"));
        value_distance_7->setGeometry(QRect(110, 60, 64, 23));
        value_distance_7->setStyleSheet(QString::fromUtf8("border: 1px solid black"));
        park7 = new QCheckBox(slot_7);
        park7->setObjectName(QString::fromUtf8("park7"));
        park7->setGeometry(QRect(50, 90, 121, 24));
        park7->setFocusPolicy(Qt::NoFocus);
        work7 = new QCheckBox(slot_7);
        work7->setObjectName(QString::fromUtf8("work7"));
        work7->setGeometry(QRect(50, 120, 121, 24));
        work7->setFocusPolicy(Qt::NoFocus);
        slot_8 = new QGroupBox(centralWidget);
        slot_8->setObjectName(QString::fromUtf8("slot_8"));
        slot_8->setGeometry(QRect(690, 590, 191, 151));
        slot_8->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255)"));
        label_29 = new QLabel(slot_8);
        label_29->setObjectName(QString::fromUtf8("label_29"));
        label_29->setGeometry(QRect(0, 30, 81, 18));
        value_light_9 = new QLCDNumber(slot_8);
        value_light_9->setObjectName(QString::fromUtf8("value_light_9"));
        value_light_9->setGeometry(QRect(110, 30, 64, 23));
        value_light_9->setStyleSheet(QString::fromUtf8("border: 1px solid black"));
        label_30 = new QLabel(slot_8);
        label_30->setObjectName(QString::fromUtf8("label_30"));
        label_30->setGeometry(QRect(0, 60, 111, 18));
        value_distance_9 = new QLCDNumber(slot_8);
        value_distance_9->setObjectName(QString::fromUtf8("value_distance_9"));
        value_distance_9->setGeometry(QRect(110, 60, 64, 23));
        value_distance_9->setStyleSheet(QString::fromUtf8("border: 1px solid black"));
        park8 = new QCheckBox(slot_8);
        park8->setObjectName(QString::fromUtf8("park8"));
        park8->setGeometry(QRect(50, 90, 121, 24));
        park8->setFocusPolicy(Qt::NoFocus);
        work8 = new QCheckBox(slot_8);
        work8->setObjectName(QString::fromUtf8("work8"));
        work8->setGeometry(QRect(50, 120, 121, 24));
        work8->setFocusPolicy(Qt::NoFocus);
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1018, 23));
        menuParking_Monitor = new QMenu(menuBar);
        menuParking_Monitor->setObjectName(QString::fromUtf8("menuParking_Monitor"));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

        menuBar->addAction(menuParking_Monitor->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Port:", nullptr));
        pushButton_open->setText(QCoreApplication::translate("MainWindow", "Open", nullptr));
        pushButton_close->setText(QCoreApplication::translate("MainWindow", "Close", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Power:", nullptr));
        pushButtonSetPower->setText(QCoreApplication::translate("MainWindow", "Set", nullptr));
        slot_2->setTitle(QCoreApplication::translate("MainWindow", "Parking Slot 2", nullptr));
        light2->setText(QCoreApplication::translate("MainWindow", "Light sensor", nullptr));
        distance2->setText(QCoreApplication::translate("MainWindow", "Distance sensor", nullptr));
        park2->setText(QCoreApplication::translate("MainWindow", "Spot available", nullptr));
        work2->setText(QCoreApplication::translate("MainWindow", "Node working", nullptr));
        slot_1->setTitle(QCoreApplication::translate("MainWindow", "Parking Slot 1", nullptr));
        light1->setText(QCoreApplication::translate("MainWindow", "Light sensor", nullptr));
        distanc1->setText(QCoreApplication::translate("MainWindow", "Distance sensor", nullptr));
        park1->setText(QCoreApplication::translate("MainWindow", "Spot available", nullptr));
        work1->setText(QCoreApplication::translate("MainWindow", "Node working", nullptr));
        slot_3->setTitle(QCoreApplication::translate("MainWindow", "Parking Slot 3", nullptr));
        label_17->setText(QCoreApplication::translate("MainWindow", "Light sensor", nullptr));
        label_18->setText(QCoreApplication::translate("MainWindow", "Distance sensor", nullptr));
        park3->setText(QCoreApplication::translate("MainWindow", "Spot available", nullptr));
        work3->setText(QCoreApplication::translate("MainWindow", "Node working", nullptr));
        slot_4->setTitle(QCoreApplication::translate("MainWindow", "Parking Slot 4", nullptr));
        label_19->setText(QCoreApplication::translate("MainWindow", "Light sensor", nullptr));
        label_20->setText(QCoreApplication::translate("MainWindow", "Distance sensor", nullptr));
        park4->setText(QCoreApplication::translate("MainWindow", "Spot available", nullptr));
        work4->setText(QCoreApplication::translate("MainWindow", "Node working", nullptr));
        slot_5->setTitle(QCoreApplication::translate("MainWindow", "Parking Slot 5", nullptr));
        label_21->setText(QCoreApplication::translate("MainWindow", "Light sensor", nullptr));
        label_22->setText(QCoreApplication::translate("MainWindow", "Distance sensor", nullptr));
        park5->setText(QCoreApplication::translate("MainWindow", "Spot available", nullptr));
        work5->setText(QCoreApplication::translate("MainWindow", "Node working", nullptr));
        slot_6->setTitle(QCoreApplication::translate("MainWindow", "Parking Slot 6", nullptr));
        label_23->setText(QCoreApplication::translate("MainWindow", "Light sensor", nullptr));
        label_24->setText(QCoreApplication::translate("MainWindow", "Distance sensor", nullptr));
        park6->setText(QCoreApplication::translate("MainWindow", "Spot available", nullptr));
        work6->setText(QCoreApplication::translate("MainWindow", "Node working", nullptr));
        slot_7->setTitle(QCoreApplication::translate("MainWindow", "Parking Slot 7", nullptr));
        label_25->setText(QCoreApplication::translate("MainWindow", "Light sensor", nullptr));
        label_26->setText(QCoreApplication::translate("MainWindow", "Distance sensor", nullptr));
        park7->setText(QCoreApplication::translate("MainWindow", "Spot available", nullptr));
        work7->setText(QCoreApplication::translate("MainWindow", "Node working", nullptr));
        slot_8->setTitle(QCoreApplication::translate("MainWindow", "Parking Slot 8", nullptr));
        label_29->setText(QCoreApplication::translate("MainWindow", "Light sensor", nullptr));
        label_30->setText(QCoreApplication::translate("MainWindow", "Distance sensor", nullptr));
        park8->setText(QCoreApplication::translate("MainWindow", "Spot available", nullptr));
        work8->setText(QCoreApplication::translate("MainWindow", "Node working", nullptr));
        menuParking_Monitor->setTitle(QCoreApplication::translate("MainWindow", "Parking Monitor", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
