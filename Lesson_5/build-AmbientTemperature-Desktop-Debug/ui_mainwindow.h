/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
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
    QLCDNumber *lcdNumber_light;
    QLabel *label_light;
    QProgressBar *progressBar_light;
    QLabel *label_2;
    QLCDNumber *lcdNumber_light_2;
    QProgressBar *progressBar_light_2;
    QLabel *label_light_2;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(498, 411);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        textEdit_Status = new QTextEdit(centralWidget);
        textEdit_Status->setObjectName(QString::fromUtf8("textEdit_Status"));
        textEdit_Status->setEnabled(true);
        textEdit_Status->setGeometry(QRect(20, 90, 211, 241));
        comboBox_Interface = new QComboBox(centralWidget);
        comboBox_Interface->setObjectName(QString::fromUtf8("comboBox_Interface"));
        comboBox_Interface->setGeometry(QRect(20, 30, 151, 22));
        label = new QLabel(centralWidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 10, 46, 13));
        pushButton_open = new QPushButton(centralWidget);
        pushButton_open->setObjectName(QString::fromUtf8("pushButton_open"));
        pushButton_open->setGeometry(QRect(180, 30, 75, 23));
        pushButton_close = new QPushButton(centralWidget);
        pushButton_close->setObjectName(QString::fromUtf8("pushButton_close"));
        pushButton_close->setEnabled(false);
        pushButton_close->setGeometry(QRect(260, 30, 75, 23));
        lcdNumber_light = new QLCDNumber(centralWidget);
        lcdNumber_light->setObjectName(QString::fromUtf8("lcdNumber_light"));
        lcdNumber_light->setGeometry(QRect(280, 90, 71, 23));
        lcdNumber_light->setFrameShape(QFrame::Box);
        lcdNumber_light->setFrameShadow(QFrame::Plain);
        lcdNumber_light->setSegmentStyle(QLCDNumber::Flat);
        label_light = new QLabel(centralWidget);
        label_light->setObjectName(QString::fromUtf8("label_light"));
        label_light->setGeometry(QRect(280, 70, 91, 16));
        progressBar_light = new QProgressBar(centralWidget);
        progressBar_light->setObjectName(QString::fromUtf8("progressBar_light"));
        progressBar_light->setGeometry(QRect(280, 120, 71, 211));
        progressBar_light->setMaximum(40);
        progressBar_light->setValue(0);
        progressBar_light->setTextVisible(false);
        progressBar_light->setOrientation(Qt::Vertical);
        progressBar_light->setInvertedAppearance(false);
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(80, 70, 101, 17));
        lcdNumber_light_2 = new QLCDNumber(centralWidget);
        lcdNumber_light_2->setObjectName(QString::fromUtf8("lcdNumber_light_2"));
        lcdNumber_light_2->setGeometry(QRect(380, 90, 71, 23));
        lcdNumber_light_2->setFrameShape(QFrame::Box);
        lcdNumber_light_2->setFrameShadow(QFrame::Plain);
        lcdNumber_light_2->setSegmentStyle(QLCDNumber::Flat);
        progressBar_light_2 = new QProgressBar(centralWidget);
        progressBar_light_2->setObjectName(QString::fromUtf8("progressBar_light_2"));
        progressBar_light_2->setGeometry(QRect(380, 120, 71, 211));
        progressBar_light_2->setMaximum(40);
        progressBar_light_2->setValue(0);
        progressBar_light_2->setTextVisible(false);
        progressBar_light_2->setOrientation(Qt::Vertical);
        progressBar_light_2->setInvertedAppearance(false);
        label_light_2 = new QLabel(centralWidget);
        label_light_2->setObjectName(QString::fromUtf8("label_light_2"));
        label_light_2->setGeometry(QRect(380, 70, 81, 16));
        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 498, 22));
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
        label->setText(QCoreApplication::translate("MainWindow", "Port", nullptr));
        pushButton_open->setText(QCoreApplication::translate("MainWindow", "Open", nullptr));
        pushButton_close->setText(QCoreApplication::translate("MainWindow", "Close", nullptr));
        label_light->setText(QCoreApplication::translate("MainWindow", "Temp  (\302\260C)", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Mote output", nullptr));
        label_light_2->setText(QCoreApplication::translate("MainWindow", "Batt  (mV)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
