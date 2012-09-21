/********************************************************************************
** Form generated from reading UI file 'PureMediaServer.ui'
**
** Created: Fri Sep 21 22:36:41 2012
**      by: Qt User Interface Compiler version 4.8.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PUREMEDIASERVER_H
#define UI_PUREMEDIASERVER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PureMediaServer
{
public:
    QAction *actionExit;
    QAction *actionOpen_conf;
    QAction *actionSave_conf;
    QWidget *centralwidget;
    QPlainTextEdit *textEdit;
    QPushButton *updateButton;
    QPushButton *restartPD;
    QCheckBox *layer1Check;
    QCheckBox *layer2Check;
    QCheckBox *layer3Check;
    QCheckBox *layer4Check;
    QCheckBox *layer5Check;
    QCheckBox *layer6Check;
    QCheckBox *layer7Check;
    QCheckBox *layer8Check;
    QSpinBox *layer1Add;
    QSpinBox *layer2Add;
    QSpinBox *layer3Add;
    QSpinBox *layer4Add;
    QSpinBox *layer5Add;
    QSpinBox *layer6Add;
    QSpinBox *layer7Add;
    QSpinBox *layer8Add;
    QCheckBox *readDMX;
    QSpinBox *universe;
    QSpinBox *winpositionx;
    QLabel *label;
    QLabel *label_2;
    QSpinBox *winpositiony;
    QSpinBox *winsizey;
    QLabel *label_3;
    QSpinBox *winsizex;
    QLabel *layer2;
    QLabel *layer3;
    QLabel *layer4;
    QLabel *layer5;
    QLabel *layer6;
    QLabel *layer7;
    QLabel *layer8;
    QCheckBox *window;
    QPushButton *ChangePath;
    QSpinBox *ipAddress1;
    QSpinBox *ipAddress2;
    QSpinBox *ipAddress3;
    QSpinBox *ipAddress4;
    QLabel *layer1;
    QStatusBar *statusBar;
    QMenuBar *menuBar;
    QMenu *menuFile;

    void setupUi(QMainWindow *PureMediaServer)
    {
        if (PureMediaServer->objectName().isEmpty())
            PureMediaServer->setObjectName(QString::fromUtf8("PureMediaServer"));
        PureMediaServer->resize(549, 633);
        actionExit = new QAction(PureMediaServer);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionOpen_conf = new QAction(PureMediaServer);
        actionOpen_conf->setObjectName(QString::fromUtf8("actionOpen_conf"));
        actionSave_conf = new QAction(PureMediaServer);
        actionSave_conf->setObjectName(QString::fromUtf8("actionSave_conf"));
        centralwidget = new QWidget(PureMediaServer);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        textEdit = new QPlainTextEdit(centralwidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(10, 10, 531, 91));
        updateButton = new QPushButton(centralwidget);
        updateButton->setObjectName(QString::fromUtf8("updateButton"));
        updateButton->setGeometry(QRect(250, 430, 113, 32));
        restartPD = new QPushButton(centralwidget);
        restartPD->setObjectName(QString::fromUtf8("restartPD"));
        restartPD->setGeometry(QRect(130, 430, 113, 32));
        layer1Check = new QCheckBox(centralwidget);
        layer1Check->setObjectName(QString::fromUtf8("layer1Check"));
        layer1Check->setGeometry(QRect(10, 110, 87, 23));
        layer2Check = new QCheckBox(centralwidget);
        layer2Check->setObjectName(QString::fromUtf8("layer2Check"));
        layer2Check->setGeometry(QRect(10, 150, 87, 23));
        layer3Check = new QCheckBox(centralwidget);
        layer3Check->setObjectName(QString::fromUtf8("layer3Check"));
        layer3Check->setGeometry(QRect(10, 190, 87, 23));
        layer4Check = new QCheckBox(centralwidget);
        layer4Check->setObjectName(QString::fromUtf8("layer4Check"));
        layer4Check->setGeometry(QRect(10, 230, 87, 23));
        layer5Check = new QCheckBox(centralwidget);
        layer5Check->setObjectName(QString::fromUtf8("layer5Check"));
        layer5Check->setGeometry(QRect(10, 270, 87, 23));
        layer6Check = new QCheckBox(centralwidget);
        layer6Check->setObjectName(QString::fromUtf8("layer6Check"));
        layer6Check->setGeometry(QRect(10, 310, 87, 23));
        layer7Check = new QCheckBox(centralwidget);
        layer7Check->setObjectName(QString::fromUtf8("layer7Check"));
        layer7Check->setGeometry(QRect(10, 350, 87, 23));
        layer8Check = new QCheckBox(centralwidget);
        layer8Check->setObjectName(QString::fromUtf8("layer8Check"));
        layer8Check->setGeometry(QRect(10, 390, 87, 23));
        layer1Add = new QSpinBox(centralwidget);
        layer1Add->setObjectName(QString::fromUtf8("layer1Add"));
        layer1Add->setGeometry(QRect(30, 110, 55, 27));
        layer1Add->setMaximum(512);
        layer2Add = new QSpinBox(centralwidget);
        layer2Add->setObjectName(QString::fromUtf8("layer2Add"));
        layer2Add->setGeometry(QRect(30, 150, 55, 27));
        layer2Add->setMaximum(512);
        layer3Add = new QSpinBox(centralwidget);
        layer3Add->setObjectName(QString::fromUtf8("layer3Add"));
        layer3Add->setGeometry(QRect(30, 190, 55, 27));
        layer3Add->setMaximum(512);
        layer4Add = new QSpinBox(centralwidget);
        layer4Add->setObjectName(QString::fromUtf8("layer4Add"));
        layer4Add->setGeometry(QRect(30, 230, 55, 27));
        layer4Add->setMaximum(512);
        layer5Add = new QSpinBox(centralwidget);
        layer5Add->setObjectName(QString::fromUtf8("layer5Add"));
        layer5Add->setGeometry(QRect(30, 270, 55, 27));
        layer5Add->setMaximum(512);
        layer6Add = new QSpinBox(centralwidget);
        layer6Add->setObjectName(QString::fromUtf8("layer6Add"));
        layer6Add->setGeometry(QRect(30, 310, 55, 27));
        layer6Add->setMaximum(512);
        layer7Add = new QSpinBox(centralwidget);
        layer7Add->setObjectName(QString::fromUtf8("layer7Add"));
        layer7Add->setGeometry(QRect(30, 350, 55, 27));
        layer7Add->setMaximum(512);
        layer8Add = new QSpinBox(centralwidget);
        layer8Add->setObjectName(QString::fromUtf8("layer8Add"));
        layer8Add->setGeometry(QRect(30, 390, 55, 27));
        layer8Add->setMaximum(512);
        readDMX = new QCheckBox(centralwidget);
        readDMX->setObjectName(QString::fromUtf8("readDMX"));
        readDMX->setGeometry(QRect(10, 470, 101, 23));
        universe = new QSpinBox(centralwidget);
        universe->setObjectName(QString::fromUtf8("universe"));
        universe->setGeometry(QRect(160, 470, 55, 27));
        winpositionx = new QSpinBox(centralwidget);
        winpositionx->setObjectName(QString::fromUtf8("winpositionx"));
        winpositionx->setGeometry(QRect(420, 470, 55, 27));
        winpositionx->setMaximum(5000);
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(140, 500, 91, 20));
        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(410, 500, 121, 20));
        winpositiony = new QSpinBox(centralwidget);
        winpositiony->setObjectName(QString::fromUtf8("winpositiony"));
        winpositiony->setGeometry(QRect(480, 470, 55, 27));
        winpositiony->setMaximum(5000);
        winsizey = new QSpinBox(centralwidget);
        winsizey->setObjectName(QString::fromUtf8("winsizey"));
        winsizey->setGeometry(QRect(330, 470, 55, 27));
        winsizey->setMaximum(5000);
        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(280, 500, 101, 17));
        winsizex = new QSpinBox(centralwidget);
        winsizex->setObjectName(QString::fromUtf8("winsizex"));
        winsizex->setGeometry(QRect(270, 470, 55, 27));
        winsizex->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
        winsizex->setMaximum(5000);
        layer2 = new QLabel(centralwidget);
        layer2->setObjectName(QString::fromUtf8("layer2"));
        layer2->setGeometry(QRect(100, 150, 441, 17));
        layer3 = new QLabel(centralwidget);
        layer3->setObjectName(QString::fromUtf8("layer3"));
        layer3->setGeometry(QRect(100, 190, 441, 17));
        layer4 = new QLabel(centralwidget);
        layer4->setObjectName(QString::fromUtf8("layer4"));
        layer4->setGeometry(QRect(100, 230, 441, 17));
        layer5 = new QLabel(centralwidget);
        layer5->setObjectName(QString::fromUtf8("layer5"));
        layer5->setGeometry(QRect(100, 270, 441, 17));
        layer6 = new QLabel(centralwidget);
        layer6->setObjectName(QString::fromUtf8("layer6"));
        layer6->setGeometry(QRect(100, 310, 451, 17));
        layer7 = new QLabel(centralwidget);
        layer7->setObjectName(QString::fromUtf8("layer7"));
        layer7->setGeometry(QRect(100, 350, 451, 17));
        layer8 = new QLabel(centralwidget);
        layer8->setObjectName(QString::fromUtf8("layer8"));
        layer8->setGeometry(QRect(100, 390, 441, 17));
        window = new QCheckBox(centralwidget);
        window->setObjectName(QString::fromUtf8("window"));
        window->setGeometry(QRect(440, 430, 87, 23));
        ChangePath = new QPushButton(centralwidget);
        ChangePath->setObjectName(QString::fromUtf8("ChangePath"));
        ChangePath->setGeometry(QRect(10, 430, 111, 28));
        ipAddress1 = new QSpinBox(centralwidget);
        ipAddress1->setObjectName(QString::fromUtf8("ipAddress1"));
        ipAddress1->setGeometry(QRect(10, 540, 55, 27));
        ipAddress1->setMaximum(254);
        ipAddress2 = new QSpinBox(centralwidget);
        ipAddress2->setObjectName(QString::fromUtf8("ipAddress2"));
        ipAddress2->setGeometry(QRect(70, 540, 55, 27));
        ipAddress2->setMaximum(254);
        ipAddress3 = new QSpinBox(centralwidget);
        ipAddress3->setObjectName(QString::fromUtf8("ipAddress3"));
        ipAddress3->setGeometry(QRect(130, 540, 55, 27));
        ipAddress3->setMaximum(254);
        ipAddress4 = new QSpinBox(centralwidget);
        ipAddress4->setObjectName(QString::fromUtf8("ipAddress4"));
        ipAddress4->setGeometry(QRect(190, 540, 55, 27));
        ipAddress4->setMaximum(254);
        layer1 = new QLabel(centralwidget);
        layer1->setObjectName(QString::fromUtf8("layer1"));
        layer1->setGeometry(QRect(100, 110, 441, 21));
        PureMediaServer->setCentralWidget(centralwidget);
        statusBar = new QStatusBar(PureMediaServer);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        PureMediaServer->setStatusBar(statusBar);
        menuBar = new QMenuBar(PureMediaServer);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 549, 29));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        PureMediaServer->setMenuBar(menuBar);

        menuBar->addAction(menuFile->menuAction());
        menuFile->addAction(actionOpen_conf);
        menuFile->addAction(actionSave_conf);

        retranslateUi(PureMediaServer);

        QMetaObject::connectSlotsByName(PureMediaServer);
    } // setupUi

    void retranslateUi(QMainWindow *PureMediaServer)
    {
        PureMediaServer->setWindowTitle(QApplication::translate("PureMediaServer", "Pure Media Server", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("PureMediaServer", "Exit", 0, QApplication::UnicodeUTF8));
        actionOpen_conf->setText(QApplication::translate("PureMediaServer", "Open conf", 0, QApplication::UnicodeUTF8));
        actionSave_conf->setText(QApplication::translate("PureMediaServer", "Save conf", 0, QApplication::UnicodeUTF8));
        textEdit->setPlainText(QApplication::translate("PureMediaServer", "Pure Media Server 0.0.5\n"
"Copyright (C) 2012 Santiago Nore\303\261a  puremediaserver@gmail.com\n"
"This program comes with ABSOLUTELY NO WARRANTY", 0, QApplication::UnicodeUTF8));
        updateButton->setText(QApplication::translate("PureMediaServer", "Update Media", 0, QApplication::UnicodeUTF8));
        restartPD->setText(QApplication::translate("PureMediaServer", "Restart PD", 0, QApplication::UnicodeUTF8));
        layer1Check->setText(QString());
        layer2Check->setText(QString());
        layer3Check->setText(QString());
        layer4Check->setText(QString());
        layer5Check->setText(QString());
        layer6Check->setText(QString());
        layer7Check->setText(QString());
        layer8Check->setText(QString());
        readDMX->setText(QApplication::translate("PureMediaServer", "Read DMX", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("PureMediaServer", "OLA Universe", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("PureMediaServer", "Window Position", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("PureMediaServer", "Window Size", 0, QApplication::UnicodeUTF8));
        layer2->setText(QApplication::translate("PureMediaServer", "Layer 2", 0, QApplication::UnicodeUTF8));
        layer3->setText(QApplication::translate("PureMediaServer", "Layer 3", 0, QApplication::UnicodeUTF8));
        layer4->setText(QApplication::translate("PureMediaServer", "Layer 4", 0, QApplication::UnicodeUTF8));
        layer5->setText(QApplication::translate("PureMediaServer", "Layer 5", 0, QApplication::UnicodeUTF8));
        layer6->setText(QApplication::translate("PureMediaServer", "Layer 6", 0, QApplication::UnicodeUTF8));
        layer7->setText(QApplication::translate("PureMediaServer", "Layer 7", 0, QApplication::UnicodeUTF8));
        layer8->setText(QApplication::translate("PureMediaServer", "Layer 8", 0, QApplication::UnicodeUTF8));
        window->setText(QApplication::translate("PureMediaServer", "Window", 0, QApplication::UnicodeUTF8));
        ChangePath->setText(QApplication::translate("PureMediaServer", "Change Path", 0, QApplication::UnicodeUTF8));
        layer1->setText(QApplication::translate("PureMediaServer", "Layer 1", 0, QApplication::UnicodeUTF8));
        menuFile->setTitle(QApplication::translate("PureMediaServer", "File", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class PureMediaServer: public Ui_PureMediaServer {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PUREMEDIASERVER_H
