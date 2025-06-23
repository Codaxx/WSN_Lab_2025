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
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QTextEdit *textEdit_Status;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QComboBox *comboBox_Interface;
    QPushButton *pushButton_open;
    QPushButton *pushButton_close;
    QPushButton *pushButton_send;
    QLabel *label_2;
    QPlainTextEdit *plainTextEdit_command;
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_3;
    QPlainTextEdit *Battery_info;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(615, 446);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        textEdit_Status = new QTextEdit(centralWidget);
        textEdit_Status->setObjectName(QString::fromUtf8("textEdit_Status"));
        textEdit_Status->setEnabled(true);
        textEdit_Status->setGeometry(QRect(150, 0, 431, 331));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 0, 121, 118));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName(QString::fromUtf8("label"));

        verticalLayout->addWidget(label);

        comboBox_Interface = new QComboBox(layoutWidget);
        comboBox_Interface->setObjectName(QString::fromUtf8("comboBox_Interface"));

        verticalLayout->addWidget(comboBox_Interface);

        pushButton_open = new QPushButton(layoutWidget);
        pushButton_open->setObjectName(QString::fromUtf8("pushButton_open"));

        verticalLayout->addWidget(pushButton_open);

        pushButton_close = new QPushButton(layoutWidget);
        pushButton_close->setObjectName(QString::fromUtf8("pushButton_close"));
        pushButton_close->setEnabled(false);

        verticalLayout->addWidget(pushButton_close);

        pushButton_send = new QPushButton(centralWidget);
        pushButton_send->setObjectName(QString::fromUtf8("pushButton_send"));
        pushButton_send->setEnabled(false);
        pushButton_send->setGeometry(QRect(10, 220, 121, 27));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 160, 111, 17));
        plainTextEdit_command = new QPlainTextEdit(centralWidget);
        plainTextEdit_command->setObjectName(QString::fromUtf8("plainTextEdit_command"));
        plainTextEdit_command->setEnabled(false);
        plainTextEdit_command->setGeometry(QRect(10, 180, 121, 31));
        verticalLayoutWidget = new QWidget(centralWidget);
        verticalLayoutWidget->setObjectName(QString::fromUtf8("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(10, 260, 121, 71));
        verticalLayout_2 = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_3 = new QLabel(verticalLayoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout_2->addWidget(label_3);

        Battery_info = new QPlainTextEdit(verticalLayoutWidget);
        Battery_info->setObjectName(QString::fromUtf8("Battery_info"));
        Battery_info->setEnabled(false);

        verticalLayout_2->addWidget(Battery_info);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 615, 22));
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
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Ext Serial Port", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Port", nullptr));
        pushButton_open->setText(QCoreApplication::translate("MainWindow", "Open", nullptr));
        pushButton_close->setText(QCoreApplication::translate("MainWindow", "Close", nullptr));
        pushButton_send->setText(QCoreApplication::translate("MainWindow", "Send", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "Command", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "Battery vol (mV)", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
