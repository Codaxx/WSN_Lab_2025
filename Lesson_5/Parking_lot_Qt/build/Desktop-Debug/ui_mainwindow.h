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
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QListView>
#include <QtWidgets/QMainWindow>
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
    QLabel *label_light;
    QLCDNumber *value_light;
    QLCDNumber *value_distance;
    QLabel *label_distance;
    QListView *parking_lot_1;
    QListView *parking_lot_3;
    QListView *parking_lot_4;
    QListView *parking_lot_5;
    QListView *parking_lot_6;
    QListView *parking_lot_7;
    QListView *parking_lot_8;
    QGroupBox *groupBox;
    QLabel *label_2;
    QMenuBar *menuBar;
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
        textEdit_Status->setGeometry(QRect(10, 40, 771, 351));
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
        label_light = new QLabel(centralWidget);
        label_light->setObjectName(QString::fromUtf8("label_light"));
        label_light->setGeometry(QRect(820, 70, 161, 31));
        value_light = new QLCDNumber(centralWidget);
        value_light->setObjectName(QString::fromUtf8("value_light"));
        value_light->setGeometry(QRect(810, 110, 171, 31));
        value_light->setFrameShape(QFrame::Box);
        value_light->setFrameShadow(QFrame::Plain);
        value_light->setSegmentStyle(QLCDNumber::Flat);
        value_distance = new QLCDNumber(centralWidget);
        value_distance->setObjectName(QString::fromUtf8("value_distance"));
        value_distance->setGeometry(QRect(810, 220, 171, 31));
        value_distance->setFrameShape(QFrame::Box);
        value_distance->setFrameShadow(QFrame::Plain);
        value_distance->setSegmentStyle(QLCDNumber::Flat);
        label_distance = new QLabel(centralWidget);
        label_distance->setObjectName(QString::fromUtf8("label_distance"));
        label_distance->setGeometry(QRect(820, 180, 161, 31));
        parking_lot_1 = new QListView(centralWidget);
        parking_lot_1->setObjectName(QString::fromUtf8("parking_lot_1"));
        parking_lot_1->setGeometry(QRect(20, 410, 151, 151));
        parking_lot_3 = new QListView(centralWidget);
        parking_lot_3->setObjectName(QString::fromUtf8("parking_lot_3"));
        parking_lot_3->setGeometry(QRect(480, 410, 151, 151));
        parking_lot_4 = new QListView(centralWidget);
        parking_lot_4->setObjectName(QString::fromUtf8("parking_lot_4"));
        parking_lot_4->setGeometry(QRect(730, 410, 151, 151));
        parking_lot_5 = new QListView(centralWidget);
        parking_lot_5->setObjectName(QString::fromUtf8("parking_lot_5"));
        parking_lot_5->setGeometry(QRect(20, 600, 151, 151));
        parking_lot_6 = new QListView(centralWidget);
        parking_lot_6->setObjectName(QString::fromUtf8("parking_lot_6"));
        parking_lot_6->setGeometry(QRect(240, 590, 151, 151));
        parking_lot_7 = new QListView(centralWidget);
        parking_lot_7->setObjectName(QString::fromUtf8("parking_lot_7"));
        parking_lot_7->setGeometry(QRect(480, 590, 151, 151));
        parking_lot_8 = new QListView(centralWidget);
        parking_lot_8->setObjectName(QString::fromUtf8("parking_lot_8"));
        parking_lot_8->setGeometry(QRect(730, 590, 151, 151));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(220, 430, 191, 121));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 30, 58, 18));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1018, 23));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        MainWindow->setStatusBar(statusBar);

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
        label_light->setText(QCoreApplication::translate("MainWindow", "Light Sensor Readout", nullptr));
        label_distance->setText(QCoreApplication::translate("MainWindow", "Distance Sensor Readout", nullptr));
        groupBox->setTitle(QCoreApplication::translate("MainWindow", "GroupBox", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
