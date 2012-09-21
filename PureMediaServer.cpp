/*
   Pure Media Server - A Media Server Sotfware for stage and performing
   Copyright (C) 2012  Santiago Noreña
   belfegor <AT> gmail <DOT> com

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "PureMediaServer.h"
#include "CITPDefines.h"
#include "MSEXDefines.h"
#include "citp-lib.h"
#include "MediaServer.h"

#include <QtDebug>
#include <QtNetwork>
#include <QFileInfo>
#include <QFileDialog>
#include <QtTest/QTest>

#define PDPORTW 9195
#define PDPORTR 9196
struct conf
{
bool window;
quint16 winpositionx;
quint16 winpositiony;
quint16 winsizex;
quint16 winsizey;
quint16 layer1Add;
bool layer1Check;
quint16 layer2Add;
bool layer2Check;
quint16 layer3Add;
bool layer3Check;
quint16 layer4Add;
bool layer4Check;
quint16 layer5Add;
bool layer5Check;
quint16 layer6Add;
bool layer6Check;
quint16 layer7Add;
bool layer7Check;
quint16 layer8Add;
bool layer8Check;
bool dmx;
quint8 universe;
quint8 ipadd1;
quint8 ipadd2;
quint8 ipadd3;
quint8 ipadd4;
QString path;
};

PureMediaServer::PureMediaServer(QWidget *parent)
  : QMainWindow(parent)
{
    // Iniciamos el User Interface
     ui.setupUi(this);
    // Cargamos la configuración del fichero
     open();
    // Iniciamos olad
    ola = new QProcess(this);
    olastart();
    // Iniciamos Pure Data
    m_pd_read = NULL;
    m_pd_write = NULL;
    pd = new QProcess(this);
    pdstart();
  // Creamos el mediaserver
  m_mediaserver = new MediaServer(this);
  Q_CHECK_PTR(m_mediaserver);
  // Conectamos los menus
  connect(ui.actionOpen_conf, SIGNAL(triggered()), this, SLOT(open()));
  connect(ui.actionSave_conf, SIGNAL(triggered()), this, SLOT(save()));
}

PureMediaServer::~PureMediaServer()
{
    close();
}

void PureMediaServer::close()
{
    ui.textEdit->appendPlainText("Closing...");
    qDebug() << "Closing...";
    m_pd_write->abort();
    m_pd_read->close();
    if (tcpsocket->isOpen()){
        tcpsocket->close();
    }
    save();
    pd->close();
    ola->close();
}

/*
 *
 * User Interface Stuff
 *
 */

void PureMediaServer::on_window_stateChanged(int state)
{
    if ((state == 2)) {
       QString desc("0001 0001;");
       if (!sendPacket(desc.toAscii().constData(),desc.size()))
       {
        ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
       }
    }
    if ((state == 0)) {
          QString desc("0001 0000;");
          if (!sendPacket(desc.toAscii().constData(),desc.size()))
          {
           ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
          }
   }
}

void PureMediaServer::on_winpositionx_valueChanged()
{
    int x = ui.winpositionx->value();
    QString desc = tr("0002 %1;").arg(x);
    if (!sendPacket(desc.toAscii().constData(),desc.size()))
            {
             ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
            }

}

void PureMediaServer::on_winpositiony_valueChanged()
{
    int x = ui.winpositiony->value();
    QString desc = tr("3 %1;").arg(x);
    if (!sendPacket(desc.toAscii().constData(),desc.size()))
            {
             ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
            }

}

void PureMediaServer::on_winsizex_valueChanged()
{
    int x = ui.winsizex->value();
    QString desc = tr("4 %1;").arg(x);
    if (!sendPacket(desc.toAscii().constData(),desc.size()))
            {
             ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
            }

}

void PureMediaServer::on_winsizey_valueChanged()
{
    int x = ui.winsizey->value();
    QString desc = tr("5 %1;").arg(x);
    if (!sendPacket(desc.toAscii().constData(),desc.size()))
            {
             ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
            }
}

void PureMediaServer::on_layer1Check_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0011 0000;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
                }
        return;
    }
    if ((state == 2))
    {
        on_layer1Add_valueChanged();
    }
}

void PureMediaServer::on_layer1Add_valueChanged()
{
   if (ui.layer1Check->isChecked()){
        int x = ui.layer1Add->value();
        QString desc = tr("0011 %1;").arg(x);
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
           {
            ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
           }
    }
}

void PureMediaServer::on_layer2Check_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0012 0000;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
                }
        return;
    }
    if ((state == 2))
    {
        on_layer2Add_valueChanged();
    }
}

void PureMediaServer::on_layer2Add_valueChanged()
{
   if (ui.layer2Check->isChecked()){
   int x = ui.layer2Add->value();
   QString desc = tr("0012 %1;").arg(x);
   if (!sendPacket(desc.toAscii().constData(),desc.size()))
           {
            ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
           }
   }
}

void PureMediaServer::on_layer3Check_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0013 0000;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
                }
        return;
    }
    if ((state == 2))
    {
        on_layer3Add_valueChanged();
    }
}

void PureMediaServer::on_layer3Add_valueChanged()
{
   if (ui.layer3Check->isChecked()){
   int x = ui.layer3Add->value();
   QString desc = tr("0013 %1;").arg(x);
   if (!sendPacket(desc.toAscii().constData(),desc.size()))
           {
            ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
           }
  }
}

void PureMediaServer::on_layer4Check_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0014 0000;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
                }
        return;
    }
    if ((state == 2))
    {
        on_layer4Add_valueChanged();
    }
}

void PureMediaServer::on_layer4Add_valueChanged()
{
   if (ui.layer4Check->isChecked()){
   int x = ui.layer4Add->value();
   QString desc = tr("0014 %1;").arg(x);
   if (!sendPacket(desc.toAscii().constData(),desc.size()))
           {
            ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
           }
   }
}

void PureMediaServer::on_layer5Check_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0015 0000;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
                }
        return;
    }
    if ((state == 2))
    {
        on_layer5Add_valueChanged();
    }
}

void PureMediaServer::on_layer5Add_valueChanged()
{
   if (ui.layer5Check->isChecked()){
   int x = ui.layer5Add->value();
   QString desc = tr("0015 %1;").arg(x);
   if (!sendPacket(desc.toAscii().constData(),desc.size()))
           {
            ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
           }
    }
}

void PureMediaServer::on_layer6Check_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0016 0000;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
                }
        return;
    }
    if ((state == 2))
    {
        on_layer6Add_valueChanged();
    }
}

void PureMediaServer::on_layer6Add_valueChanged()
{
   if (ui.layer6Check->isChecked()){
   int x = ui.layer6Add->value();
   QString desc = tr("0016 %1;").arg(x);
   if (!sendPacket(desc.toAscii().constData(),desc.size()))
           {
            ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
           }
   }
}

void PureMediaServer::on_layer7Check_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0017 0000;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
                }
        return;
    }
    if ((state == 2))
    {
        on_layer7Add_valueChanged();
    }
}

void PureMediaServer::on_layer7Add_valueChanged()
{
   if (ui.layer7Check->isChecked()){
   int x = ui.layer7Add->value();
   QString desc = tr("0017 %1;").arg(x);
   if (!sendPacket(desc.toAscii().constData(),desc.size()))
           {
            ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
           }
   }
}

void PureMediaServer::on_layer8Check_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0018 0000;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
                }
        return;
    }
    if ((state == 2))
    {
        on_layer8Add_valueChanged();
    }
}

void PureMediaServer::on_layer8Add_valueChanged()
{
   if (ui.layer8Check->isChecked()){
   int x = ui.layer8Add->value();
   QString desc = tr("0018 %1;").arg(x);
   if (!sendPacket(desc.toAscii().constData(),desc.size()))
           {
            ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
           }
  }
}

void PureMediaServer::on_readDMX_stateChanged(int state)
{
    if ((state == 0)) {
        QString desc("0020 0000;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
                }
        }
    if ((state == 2))
    {
        int x = ui.universe->value();
        QString desc = tr("0021 %1;").arg(x);
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
                }
        desc = tr("0020 0001;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
                }
        }
 }

/*
 *
 * MSEX Stuff
 *
 */

void PureMediaServer::on_updateButton_clicked()
{
    // Chequeamos si existe el path a los medias
    QDir dir(pathmedia);
     if (!dir.exists())
     {
         ui.textEdit->appendPlainText("Cannot find the media directory");
         return;
     }
     m_mediaserver->setpath(pathmedia);
     if (!m_mediaserver->updatemedia())
     {
         ui.textEdit->appendPlainText("Cannot explore the media");
         return;
     }

     // Creamos el objeto CITP y el peer information socket
         m_citp = new CITPLib(this);
         Q_CHECK_PTR(m_citp);
         quint32 ipadd = 0x00000000;
         quint32 i;
         i = ui.ipAddress1->value();
         ipadd = ipadd + (i * 0x1000000);
         i =ui.ipAddress2->value();
         ipadd = ipadd + (i * 0x10000);
         i = ui.ipAddress3->value();
         ipadd = ipadd + (i * 0x100);
         i = ui.ipAddress4->value();
         ipadd = ipadd + i;
         if (!m_citp->createPeerInformationSocket(NAME, STATE, ipadd))
           {
           ui.textEdit->appendPlainText("CreatePeerInformationSocket failed");
           }
}

/*
 *
 * Change Media path
 *
 */

void PureMediaServer::on_ChangePath_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    QStringList fileNames;
    if (dialog.exec())
        fileNames = dialog.selectedFiles();
    QString file = fileNames.at(0);
    pathmedia = file;
    QString desc = tr("0000 0000 %1;").arg(file);
    if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");

    }
    on_updateButton_clicked();
    desc = tr("Media Path Changed to: %1").arg(pathmedia);
    ui.textEdit->appendPlainText(desc);
}
/*
 *
 * File Configuration Stuff
 *
 */

void PureMediaServer::open()
{
    ui.textEdit->appendPlainText("Opening conf file");
    QFile file("pms.conf");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
     {
        ui.textEdit->appendPlainText("Can not find the conf file");
        return;
     }
    unsigned char * fileconf = new unsigned char[file.size()];
    memset(fileconf, 0, file.size());
    fileconf = file.map(0x00, file.size());
    if (fileconf == 0){
        ui.textEdit->appendPlainText("Cannot map the file");
        return;
    }
    conf *packet = (conf *)fileconf;
    ui.window->setChecked(packet->window);
    ui.winpositionx->setValue(packet->winpositionx);
    ui.winpositiony->setValue(packet->winpositiony);
    ui.winsizex->setValue(packet->winsizex);
    ui.winsizey->setValue(packet->winsizey);
    ui.layer1Add->setValue(packet->layer1Add);
    ui.layer1Check->setChecked(packet->layer1Check);
    ui.layer2Add->setValue(packet->layer2Add);
    ui.layer2Check->setChecked(packet->layer2Check);
    ui.layer3Add->setValue(packet->layer3Add);
    ui.layer3Check->setChecked(packet->layer3Check);
    ui.layer4Add->setValue(packet->layer4Add);
    ui.layer4Check->setChecked(packet->layer4Check);
    ui.layer5Add->setValue(packet->layer5Add);
    ui.layer5Check->setChecked(packet->layer5Check);
    ui.layer6Add->setValue(packet->layer6Add);
    ui.layer6Check->setChecked(packet->layer6Check);
    ui.layer7Add->setValue(packet->layer7Add);
    ui.layer7Check->setChecked(packet->layer7Check);
    ui.layer8Add->setValue(packet->layer8Add);
    ui.layer8Check->setChecked(packet->layer8Check);
    ui.readDMX->setChecked(packet->dmx);
    ui.universe->setValue(packet->universe);
    ui.ipAddress1->setValue(packet->ipadd1);
    ui.ipAddress2->setValue(packet->ipadd2);
    ui.ipAddress3->setValue(packet->ipadd3);
    ui.ipAddress4->setValue(packet->ipadd4);
    int offset = sizeof(struct conf) - 4;
    int size = file.size() - offset;
    char * buffer = new char[size];
    memset(buffer, 0, size);
    memcpy(buffer, fileconf+offset, size);
    pathmedia = buffer;
    QString desc = tr("Media Path Changed to: %1").arg(pathmedia);
    ui.textEdit->appendPlainText(desc);
    file.close();
}

void PureMediaServer::save()
{
    ui.textEdit->appendPlainText("Saving conf file");
    int bufferLen = sizeof(struct conf) + pathmedia.size();
    unsigned char *buffer = new unsigned char[bufferLen];
    memset(buffer, 0, bufferLen);
    conf *packet = (conf *)buffer;
    packet->window = ui.window->checkState();
    packet->winpositionx = ui.winpositionx->value();
    packet->winpositiony = ui.winpositiony->value();
    packet->winsizex = ui.winsizex->value();
    packet->winsizey = ui.winsizey->value();
    packet->layer1Add = ui.layer1Add->value();
    packet->layer1Check = ui.layer1Check->checkState();
    packet->layer2Add = ui.layer2Add->value();
    packet->layer2Check = ui.layer2Check->checkState();
    packet->layer3Add = ui.layer3Add->value();
    packet->layer3Check = ui.layer3Check->checkState();
    packet->layer4Add = ui.layer4Add->value();
    packet->layer4Check = ui.layer4Check->checkState();
    packet->layer5Add = ui.layer5Add->value();
    packet->layer5Check = ui.layer5Check->checkState();
    packet->layer6Add = ui.layer6Add->value();
    packet->layer6Check = ui.layer6Check->checkState();
    packet->layer7Add = ui.layer7Add->value();
    packet->layer7Check = ui.layer7Check->checkState();
    packet->layer8Add = ui.layer8Add->value();
    packet->layer8Check = ui.layer8Check->checkState();
    packet->dmx = ui.readDMX->checkState();
    packet->universe = ui.universe->value();
    packet->ipadd1 = ui.ipAddress1->value();
    packet->ipadd2 = ui.ipAddress2->value();
    packet->ipadd3 = ui.ipAddress3->value();
    packet->ipadd4 = ui.ipAddress4->value();
    int offset = sizeof (struct conf) - 4;
    memcpy(buffer+offset, pathmedia.toAscii().constData(), pathmedia.size());
    QFile file("pms.conf");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        ui.textEdit->appendPlainText("Can not open file pms.conf");
        return;
    }
    int error =  file.write((const char *)buffer, bufferLen);
    QString errorstring = tr("Bytes Write to file %1").arg(error);
    ui.textEdit->appendPlainText(errorstring);
    file.close();
}

/*
 *
 * Pure Data Stuff
 *
 */


void PureMediaServer::pdstart()
{
    int state = pd->state();
    if (state != 0)
    {
        return;
    }
    qDebug()<< "starting pd";
    // Creamos los sockets para la conexión a Pure Data
    m_pd_write = new QTcpSocket(this);
    Q_CHECK_PTR(m_pd_write);
    m_pd_read = new QTcpServer(this);
    Q_CHECK_PTR(m_pd_read);
    if (!m_pd_read)
     {
         ui.textEdit->appendPlainText("error TCP Server no creado");
     }
    if (!m_pd_read->listen(QHostAddress::LocalHost, PDPORTR))
         {
         ui.textEdit->appendPlainText("error creando tcpServer");
         }
    connect(m_pd_read, SIGNAL(newConnection()),this, SLOT(newPeer()));
    ui.textEdit->appendPlainText("Starting Pure Data");
    pd->start("pd", QStringList()<<"pms-video.pd");
    pd->waitForStarted();
    m_pd_write->connectToHost(QHostAddress::LocalHost, PDPORTW);
    while (!m_pd_write->waitForConnected(3000))
    {
            m_pd_write->connectToHost(QHostAddress::LocalHost, PDPORTW);
    }

    connect(m_pd_write, SIGNAL(connected()),this, SLOT(newconexion()));
    connect(pd, SIGNAL(finished(int)), this, SLOT(pdrestart()));
}

void PureMediaServer::pdrestart()
{
    qDebug()<<"restarting pd";
    int state = pd->state();
    if (state != 0)
    {
        return;
    }
    if (m_pd_write != NULL)
    {
        m_pd_write->close();
        disconnect(m_pd_write, SIGNAL(connected()),this, SLOT(newconexion()));
        delete m_pd_write;
    }
    if (m_pd_read != NULL)
    {
        disconnect(m_pd_read, SIGNAL(newConnection()),this, SLOT(newPeer()));
        m_pd_read->close();
        delete m_pd_read;
    }
    disconnect(pd, SIGNAL(finished(int)), this, SLOT(pdrestart()));
    pdstart();
}

void PureMediaServer::newPeer()
{
   tcpsocket = m_pd_read->nextPendingConnection();
   connect(tcpsocket, SIGNAL(readyRead()),
                this, SLOT(newmessage()));
}

void PureMediaServer::newmessage()
{
    if (tcpsocket == NULL)
    {
        ui.textEdit->appendPlainText("tcpsocket not created");
        newPeer();
        return;
    }
    QByteArray byteArray = tcpsocket->readAll();
    QString string(byteArray);
    if (byteArray.at(0) == NULL)
    {
        return;
    }
    QChar layer = string.at(0);
    int i = 9 + pathmedia.size();
    string.remove(0,i);
    string.chop(2);
    int val = layer.digitValue();
    switch (val) {
    case 1:
       ui.layer1->setText(string);
       break;
    case 2:
        ui.layer2->setText(string);
        break;
    case 3:
        ui.layer3->setText(string);
        break;
    case 4:
        ui.layer4->setText(string);
        break;
    case 5:
        ui.layer5->setText(string);
        break;
    case 6:
        ui.layer6->setText(string);
        break;
    case 7:
        ui.layer7->setText(string);
        break;
    case 8:
        ui.layer8->setText(string);
        break;
    }
}

bool PureMediaServer::sendPacket(const char *buffer, int bufferLen)
{
 if (!m_pd_write) {
    return false;
 }
 if (QAbstractSocket::ConnectedState != m_pd_write->state())
 {
    ui.textEdit->appendPlainText("sendPacket() - PD Socket not connected");
    return false;
 }
 if (bufferLen != m_pd_write->write((const char*)buffer, bufferLen))
 {
    ui.textEdit->appendPlainText("sendPacket() - write to PD failed");
    return false;
 }
 return true;
}

void PureMediaServer::newconexion()
{
    ui.textEdit->appendPlainText("Connected to Pure Data - Sending configuration");
    if (!(m_pd_write->isOpen())){
        ui.textEdit->appendPlainText("Socket not open. Can not send conf to PD");
        return;
     }
    QString desc = tr("0000 0000 %1;").arg(pathmedia);
    if (!sendPacket(desc.toAscii().constData(),desc.size()))
    {
      ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
      return;
    }
    QTest::qWait(50);
    on_window_stateChanged(ui.window->checkState());
     QTest::qWait(50);
     on_winpositionx_valueChanged();
     QTest::qWait(50);
     on_winpositiony_valueChanged();
     QTest::qWait(50);
     on_winsizex_valueChanged();
     QTest::qWait(50);
     on_winsizey_valueChanged();
     QTest::qWait(50);
     on_readDMX_stateChanged(ui.readDMX->checkState());
     QTest::qWait(50);
     on_layer1Check_stateChanged (ui.layer1Check->checkState());
     QTest::qWait(50);
     on_layer2Check_stateChanged (ui.layer2Check->checkState());
     QTest::qWait(50);
     on_layer3Check_stateChanged (ui.layer3Check->checkState());
     QTest::qWait(50);
     on_layer4Check_stateChanged (ui.layer4Check->checkState());
     QTest::qWait(50);
     on_layer5Check_stateChanged (ui.layer5Check->checkState());
     QTest::qWait(50);
     on_layer6Check_stateChanged (ui.layer6Check->checkState());
     QTest::qWait(50);
     on_layer7Check_stateChanged (ui.layer7Check->checkState());
     QTest::qWait(50);
     on_layer8Check_stateChanged (ui.layer8Check->checkState());
}

/*
 *
 * OLA Stuff
 *
 */

void PureMediaServer::olastart()
{
    ui.textEdit->appendPlainText("Starting OLA");
    ola->start("olad", QStringList()<< "-l 3");
//    connect(ola, SIGNAL(finished(int)), this, SLOT(olastart()));
}



/*
void PureMediaServer::on_connectPDButton_clicked(){
    if (m_pd_write->isOpen()){
            m_pd_write->close();
            m_pd_read->close();
    }
    m_pd_write->connectToHost(QHostAddress::LocalHost, PDPORTW);
    connect(m_pd_write, SIGNAL(connected()),this, SLOT(newconexion()));
    connect(m_pd_write, SIGNAL(disconnected()), this, SLOT(pdDisconnected()));
    // Inicio del TCP Server; empieza a escuchar
     if (!m_pd_read)
     {
         ui.textEdit->appendPlainText("error TCP Server no creado");
     }
     if (!m_pd_read->listen(QHostAddress::LocalHost, PDPORTR))
         {
         ui.textEdit->appendPlainText("error creando tcpServer");
         }
     connect(m_pd_read, SIGNAL(newConnection()),
             this, SLOT(newPeer()));
}
*/
/*
void PureMediaServer::pdDisconnected()
{
    ui.textEdit->appendPlainText("Pure Data Disconnected");
    m_pd_write->close();
    m_pd_read->close();
}
*/
