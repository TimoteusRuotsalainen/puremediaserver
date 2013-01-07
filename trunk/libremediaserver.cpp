/*
   Pure Media Server - A Media Server Sotfware for stage and performing

   Copyright (C) 2012-2013  Santi Noreña libremediaserver@gmail.com

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

#include "libremediaserver.h"
#include "CITPDefines.h"
#include "MSEXDefines.h"
#include "msex.h"

#include <QObject>
#include <QApplication>
#include <QDesktopWidget>
#include <QtDebug>
#include <QtNetwork>
#include <QFileInfo>
#include <QFileDialog>
#include <QLocalServer>
//#include <sys/socket.h>
//#include <sys/types.h>
//#include <sys/un.h>

// Esto habrá que cambiarlo para poder utilizar varias instancias
#define PDPORTW 9195
//#define PDPORTR 9196 ya no hace falta

#define PDPORTW_AUDIO 9197
#define PDPORTR_AUDIO 9198

#define SOCKET "/tmp/pmspipe"

///////////////////////////////////////////////////////////////////
// Struct for the configuration files
///////////////////////////////////////////////////////////////////

struct conf
{
// Video configuration
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
bool videoCheck;

// Audio Configuration
quint16 layer1Add_audio;
bool layer1Check_audio;
quint16 layer2Add_audio;
bool layer2Check_audio;
quint16 layer3Add_audio;
bool layer3Check_audio;
quint16 layer4Add_audio;
bool layer4Check_audio;
quint16 layer5Add_audio;
bool layer5Check_audio;
quint16 layer6Add_audio;
bool layer6Check_audio;
quint16 layer7Add_audio;
bool layer7Check_audio;
quint16 layer8Add_audio;
bool layer8Check_audio;
bool dmx_audio;
quint8 universe_audio;
bool audioCheck;

// Path to Media Files
QString path;
};

// Constructor
///////////////////////////////////////////////////////////////////

libreMediaServer::libreMediaServer(QWidget *parent)
  : QMainWindow(parent)
{
     // Iniciamos el User Interface
     ui.setupUi(this);
     // Iniciamos los punteros NULL
     m_pd_write_video = NULL;
     m_pd_video = NULL;
     m_tcpsocket_audio = NULL;
     m_pd_read_audio = NULL;
     m_pd_write_audio = NULL;
     m_pd_audio = NULL;
    // Unix Local Sockets
     QFile socket(SOCKET);
     socket.remove();
     m_server_vid = new QLocalServer(this);
     Q_CHECK_PTR(m_server_vid);
     if (!m_server_vid->listen(SOCKET))
     {
         qErrnoWarning("Can not listen on unix local server");
     }
     connect(m_server_vid, SIGNAL(newConnection()),this, SLOT(newPeer()));
     // Start preview Timer
     m_preview = new QTimer(this);
     Q_CHECK_PTR(m_preview);
     m_preview->start(500);
     connect(m_preview, SIGNAL(timeout()) ,this, SLOT(previewMaster()));
     // The mediaserver object: CITP/MSEx
     m_msex = new msex(this);
     Q_CHECK_PTR(m_msex);
     // Iniciamos olad
     m_ola = new QProcess(this);
     olastart();
     // Conectamos los menus
    connect(ui.actionOpen_conf, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui.actionSave_conf, SIGNAL(triggered()), this, SLOT(save()));
    // Load the configuration
    open();
    connect(m_msex,SIGNAL(frameRequest()), this, SLOT(sendFrame()));
}

// Destructor
///////////////////////////////////////////////////////////////////

libreMediaServer::~libreMediaServer()
{
    save();
    QFile socket(SOCKET);
    socket.remove();
    if (m_pd_write_video != NULL) {
          m_pd_write_video->abort();
          delete m_pd_write_video;
        }
 //   if (m_read_vid->isOpen()) {
 //       m_read_vid->close();;
 //       delete m_read_vid;
 //   }
    if (m_server_vid->isListening()) {
        m_server_vid->close();
        delete m_server_vid;
    }
    if (m_pd_video != NULL)
    {
        disconnect(m_pd_video, SIGNAL(finished(int)), this, SLOT(pdrestart()));
        m_pd_video->close();
        delete m_pd_video;
    }
        m_ola->close();
        delete m_ola;
}

///////////////////////////////////////////////////////////////////
// File Configuration Stuff
///////////////////////////////////////////////////////////////////

// Load the last configuration from the file pms.conf

void libreMediaServer::open()
{
    QFile file("pms.conf");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
     {
        qDebug()<<("Can not find the conf file");
        return;
     }
    unsigned char * fileconf = new unsigned char[file.size()];
    memset(fileconf, 0, file.size());
    fileconf = file.map(0x00, file.size());
    if (fileconf == 0){
        qDebug()<<("Cannot map the file");
        return;
    }
    conf *packet = (conf *)fileconf;
    // Video configuration
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
    ui.video->setChecked(packet->videoCheck);

    // Audio Configuration
    ui.layer1Add_audio->setValue(packet->layer1Add_audio);
    ui.layer1Check_audio->setChecked(packet->layer1Check_audio);
    ui.layer2Add_audio->setValue(packet->layer2Add_audio);
    ui.layer2Check_audio->setChecked(packet->layer2Check_audio);
    ui.layer3Add_audio->setValue(packet->layer3Add_audio);
    ui.layer3Check_audio->setChecked(packet->layer3Check_audio);
    ui.layer4Add_audio->setValue(packet->layer4Add_audio);
    ui.layer4Check_audio->setChecked(packet->layer4Check_audio);
    ui.layer5Add_audio->setValue(packet->layer5Add_audio);
    ui.layer5Check_audio->setChecked(packet->layer5Check_audio);
    ui.layer6Add_audio->setValue(packet->layer6Add_audio);
    ui.layer6Check_audio->setChecked(packet->layer6Check_audio);
    ui.layer7Add_audio->setValue(packet->layer7Add_audio);
    ui.layer7Check_audio->setChecked(packet->layer7Check_audio);
    ui.layer8Add_audio->setValue(packet->layer8Add_audio);
    ui.layer8Check_audio->setChecked(packet->layer8Check_audio);
    ui.readDMX_audio->setChecked(packet->dmx_audio);
    ui.universe_audio->setValue(packet->universe_audio);
    ui.audio->setChecked(packet->audioCheck);

      // Path to media
    int offset = sizeof(struct conf) - 4;
    int size = file.size() - offset;
    char * buffer = new char[size];
    memset(buffer, 0, size);
    memcpy(buffer, fileconf+offset, size);
    m_pathmedia = buffer;
    QString desc = tr("Media Path Changed to: %1").arg(m_pathmedia);
    qDebug()<<(desc);
    file.close();
}

// Save the configuration to pms.conf file
void libreMediaServer::save()
{
    int bufferLen = sizeof(struct conf) + m_pathmedia.size();
    unsigned char *buffer = new unsigned char[bufferLen];
    memset(buffer, 0, bufferLen);
    conf *packet = (conf *)buffer;

    // Video Configuration

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
    packet->videoCheck = ui.video->checkState();

    // Audio Configuration

    packet->layer1Add_audio = ui.layer1Add_audio->value();
    packet->layer1Check_audio = ui.layer1Check_audio->checkState();
    packet->layer2Add_audio = ui.layer2Add_audio->value();
    packet->layer2Check_audio = ui.layer2Check_audio->checkState();
    packet->layer3Add_audio = ui.layer3Add_audio->value();
    packet->layer3Check_audio = ui.layer3Check_audio->checkState();
    packet->layer4Add_audio = ui.layer4Add_audio->value();
    packet->layer4Check_audio = ui.layer4Check_audio->checkState();
    packet->layer5Add_audio = ui.layer5Add_audio->value();
    packet->layer5Check_audio = ui.layer5Check_audio->checkState();
    packet->layer6Add_audio = ui.layer6Add_audio->value();
    packet->layer6Check_audio = ui.layer6Check_audio->checkState();
    packet->layer7Add_audio = ui.layer7Add_audio->value();
    packet->layer7Check_audio = ui.layer7Check_audio->checkState();
    packet->layer8Add_audio = ui.layer8Add_audio->value();
    packet->layer8Check_audio = ui.layer8Check_audio->checkState();
    packet->dmx_audio = ui.readDMX_audio->checkState();
    packet->universe_audio = ui.universe_audio->value();
    packet->audioCheck = ui.audio->checkState();

    // Path to media

    int offset = sizeof (struct conf) - 4;
    memcpy(buffer+offset, m_pathmedia.toAscii().constData(), m_pathmedia.size());
    QFile file("pms.conf");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug()<<("Can not open file pms.conf");
        return;
    }
    int error =  file.write((const char *)buffer, bufferLen);
    QString errorstring = tr("Bytes Write to file %1").arg(error);
    qDebug()<<"Saved file complete:"<<(errorstring);
    file.close();
    delete buffer;
}

///////////////////////////////////////////////////////////////////
// OLA Stuff
///////////////////////////////////////////////////////////////////

void libreMediaServer::olastart()
{
    qDebug()<<("Starting OLA");
    m_ola->start("olad", QStringList()<< "-l 3");
//    connect(ola, SIGNAL(finished(int)), this, SLOT(olastart()));
}

/*
 * User Interface Stuff
 */

///////////////////////////////////////////////////////////////////
// Global Controls
///////////////////////////////////////////////////////////////////

// Change Media path

void libreMediaServer::on_ChangePath_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    QStringList fileNames;
    if (!dialog.exec())
        return;
    fileNames = dialog.selectedFiles();
    QString file = fileNames.at(0);
    m_pathmedia = file;
    QString desc = tr("0000 0000 %1;").arg(file);
    if (ui.video->checkState())
    {
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
        {
            errorsending();
        }
    }
    if (ui.audio->checkState())
    {
        if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
        {
            errorsending_audio();
        }
    }
    desc = tr("Media Path Changed to: %1").arg(m_pathmedia);
    ui.textEdit->appendPlainText(desc.toAscii());

}

///////////////////////////////////////////////////////////////////
// Video Controls
///////////////////////////////////////////////////////////////////

// Init the CITP/MSEx protocol.
// Begins the CITP/MSEx protocol
// ToDo: Include thumbs generation here

void libreMediaServer::on_updateButton_clicked()
{
    // Chequeamos si existe el path a los medias
    QDir dir(m_pathmedia);
     if (!dir.exists())
     {
         qDebug()<<("Cannot find the media directory");
         ui.textEdit->appendPlainText("Can not find the media directory in the path given");
         return;
     }
     m_msex->setpath(m_pathmedia);
     if (!m_msex->updatemedia())
     {
         qDebug()<<("Cannot explore the media");
         ui.textEdit->appendPlainText("Can not explore the media in the path given");
         return;
     }


     // Creamos el objeto CITP y el peer information socket

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
     m_msex->startCitp(ipadd);

}

// Window Configuration

void libreMediaServer::on_window_stateChanged(int state)
{
    if ((state == 2)) {
       QString desc("0001 0001;");
       if (!sendPacket(desc.toAscii().constData(),desc.size()))
       {
        errorsending();
       }
    }
    if ((state == 0)) {
          QString desc("0001 0000;");
          if (!sendPacket(desc.toAscii().constData(),desc.size()))
          {
           errorsending();
          }
   }
}

void libreMediaServer::on_winpositionx_valueChanged()
{
    int x = ui.winpositionx->value();
    QString desc = tr("0002 %1;").arg(x);
    if (!sendPacket(desc.toAscii().constData(),desc.size()))
            {
             errorsending();
            }
}

void libreMediaServer::on_winpositiony_valueChanged()
{
    int x = ui.winpositiony->value();
    QString desc = tr("3 %1;").arg(x);
    if (!sendPacket(desc.toAscii().constData(),desc.size()))
            {
             errorsending();
            }
}

void libreMediaServer::on_winsizex_valueChanged()
{
    int x = ui.winsizex->value();
    QString desc = tr("4 %1;").arg(x);
    if (!sendPacket(desc.toAscii().constData(),desc.size()))
            {
             errorsending();
            }
}

void libreMediaServer::on_winsizey_valueChanged()
{
    int x = ui.winsizey->value();
    QString desc = tr("5 %1;").arg(x);
    if (!sendPacket(desc.toAscii().constData(),desc.size()))
            {
             errorsending();
            }
}

// DMX address configuration

void libreMediaServer::on_layer1Check_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0011 0000;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 errorsending();
                }
         return;
    }
    if ((state == 2))
    {
        on_layer1Add_valueChanged();
    }
}

void libreMediaServer::on_layer1Add_valueChanged()
{
   if (ui.layer1Check->isChecked()){
        int x = ui.layer1Add->value();
        QString desc = tr("0011 %1;").arg(x);
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
           {
            errorsending();
           }
    }
}

void libreMediaServer::on_layer2Check_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0012 0000;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 errorsending();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer2Add_valueChanged();
    }
}

void libreMediaServer::on_layer2Add_valueChanged()
{
   if (ui.layer2Check->isChecked()){
   int x = ui.layer2Add->value();
   QString desc = tr("0012 %1;").arg(x);
   if (!sendPacket(desc.toAscii().constData(),desc.size()))
           {
            errorsending();
           }
   }
}

void libreMediaServer::on_layer3Check_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0013 0000;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 errorsending();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer3Add_valueChanged();
    }
}

void libreMediaServer::on_layer3Add_valueChanged()
{
   if (ui.layer3Check->isChecked()){
   int x = ui.layer3Add->value();
   QString desc = tr("0013 %1;").arg(x);
   if (!sendPacket(desc.toAscii().constData(),desc.size()))
           {
            errorsending();
           }
  }
}

void libreMediaServer::on_layer4Check_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0014 0000;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 errorsending();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer4Add_valueChanged();
    }
}

void libreMediaServer::on_layer4Add_valueChanged()
{
   if (ui.layer4Check->isChecked()){
   int x = ui.layer4Add->value();
   QString desc = tr("0014 %1;").arg(x);
   if (!sendPacket(desc.toAscii().constData(),desc.size()))
           {
            errorsending();
           }
   }
}

void libreMediaServer::on_layer5Check_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0015 0000;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 errorsending();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer5Add_valueChanged();
    }
}

void libreMediaServer::on_layer5Add_valueChanged()
{
   if (ui.layer5Check->isChecked()){
   int x = ui.layer5Add->value();
   QString desc = tr("0015 %1;").arg(x);
   if (!sendPacket(desc.toAscii().constData(),desc.size()))
           {
            errorsending();
           }
    }
}

void libreMediaServer::on_layer6Check_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0016 0000;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 errorsending();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer6Add_valueChanged();
    }
}

void libreMediaServer::on_layer6Add_valueChanged()
{
   if (ui.layer6Check->isChecked()){
   int x = ui.layer6Add->value();
   QString desc = tr("0016 %1;").arg(x);
   if (!sendPacket(desc.toAscii().constData(),desc.size()))
           {
            errorsending();
           }
   }
}

void libreMediaServer::on_layer7Check_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0017 0000;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 errorsending();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer7Add_valueChanged();
    }
}

void libreMediaServer::on_layer7Add_valueChanged()
{
   if (ui.layer7Check->isChecked()){
   int x = ui.layer7Add->value();
   QString desc = tr("0017 %1;").arg(x);
   if (!sendPacket(desc.toAscii().constData(),desc.size()))
           {
            errorsending();
           }
   }
}

void libreMediaServer::on_layer8Check_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0018 0000;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 errorsending();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer8Add_valueChanged();
    }
}

void libreMediaServer::on_layer8Add_valueChanged()
{
   if (ui.layer8Check->isChecked()){
   int x = ui.layer8Add->value();
   QString desc = tr("0018 %1;").arg(x);
   if (!sendPacket(desc.toAscii().constData(),desc.size()))
           {
            errorsending();
           }
  }
}

// Open the connection with OLA and start reading DMX

void libreMediaServer::on_readDMX_stateChanged(int state)
{
    if ((state == 0)) {
        QString desc("0020 0000;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 errorsending();
                }
        }
    if ((state == 2))
    {
        int x = ui.universe->value();
        QString desc = tr("0021 %1;").arg(x);
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 errorsending();
                }
        desc = tr("0020 0001;");
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 errorsending();
                }
        }
 }

// Open the video process

void libreMediaServer::on_video_stateChanged(int state)
{
    if ((state == 0))
         {
        disconnect(m_pd_video, SIGNAL(readyReadStandardError()), this, SLOT(stdout()));
        disconnect(m_pd_video, SIGNAL(finished(int)), this, SLOT(pdrestart()));
        m_pd_video->terminate();
        m_pd_video = NULL;
        delete m_pd_video;
    }
    if ((state == 2))
    {
   // Iniciamos Pure Data
   m_pd_video = new QProcess(this);
   pdstart();
   }
}

///////////////////////////////////////////////////////////////////
/*
 * Pure Data Video Stuff
 */
///////////////////////////////////////////////////////////////////

// Start the PD Process, open the ports and connects stdout de Pure Data.

void libreMediaServer::pdstart()
{
    if (m_pd_video->state() != 0)
    {
        return;
    }
    // Creamos los sockets para la conexión a Pure Data
    m_pd_write_video = new QTcpSocket(this);
    Q_CHECK_PTR(m_pd_write_video);
    connect(m_pd_write_video, SIGNAL(connected()),this, SLOT(newconexion()));
    // Arrancamos el proceso Pure Data
    m_pd_video->start("pd -lib Gem -d 4 -stderr pms-video.pd");
    if (m_pd_video->waitForStarted(3000)){
        ui.textEdit->appendPlainText("Video Engine started.");
    }
    else
    {
        ui.textEdit->appendPlainText("Video Engine can not start!");
        return;
    }
    // Connect the output fropm PD Video to stdout slot to process it
    connect(m_pd_video, SIGNAL(readyReadStandardError()), this, SLOT(stdout()));
//    connect(m_pd_video, SIGNAL(readyRead()),this,SLOT(stdout()));
    // Restart PD Video if crash
    connect(m_pd_video, SIGNAL(finished(int)), this, SLOT(pdrestart()));
}

// Sacamos la salida de Pure Data en la terminal

void libreMediaServer::stdout() {
    QByteArray out = m_pd_video->readAllStandardError();
    if (out.size() < 7) {return;}
    out.chop(1);
    qDebug() << out;
    if (out.indexOf("ola2pd: Can not read DMX!",0) != -1)
        {
            ui.textEdit->appendPlainText("Can not read DMX data.");
        }
    if (out.indexOf("watchdog",0) != -1)
        {
            ui.textEdit->appendPlainText("PD video watchdog detected.");
        }
    int j = out.indexOf("togui",0);
    if (j >= 0)
         {
         int i = m_pathmedia.size() + 16;
         out.remove(0,j);
         switch (out.at(7)) {
             case '0':
                qDebug()<<"Loadbang Video";
                ui.textEdit->appendPlainText("LoadBang Video received.");
                newconexion();
                break;
             case '1':
                out.remove(0, i);
                out.prepend("Layer 1 playing:");
                ui.textEdit->appendPlainText(out);
                break;
             case '2':
                out.remove(0, i);
                out.prepend("Layer 2 playing:");
                ui.textEdit->appendPlainText(out);
                break;
             case '3':
                out.remove(0,i);
                out.prepend("Layer 3 playing:");
                ui.textEdit->appendPlainText(out);
                break;
             case '4':
                out.remove(0,i);
                out.prepend("Layer 4 playing:");
                ui.textEdit->appendPlainText(out);
                break;
             case '5':
                out.remove(0,i);
                out.prepend("Layer 5 playing:");
                ui.textEdit->appendPlainText(out);
                break;
             case '6':
                out.remove(0,i);
                out.prepend("Layer 6 playing:");
                ui.textEdit->appendPlainText(out);
                break;
             case '7':
                out.remove(0,i);
                out.prepend("Layer 7 playing:");
                ui.textEdit->appendPlainText(out);
                break;
             case '8':
                out.remove(0,i);
                out.prepend("Layer 8 playing:");
                ui.textEdit->appendPlainText(out);
                break;
             default:
                qDebug()<<"stdout:Invalid cooki received"<<out;
                break;
             }
       }
}
// Restart the Pure Data process if crash. Connected wit signal finished of QProcess

void libreMediaServer::pdrestart()
{
    if (m_pd_video->state())
    {
        return;
    }
    save();
    qDebug()<<"Restarting PD";
    ui.textEdit->appendPlainText("PD Restarting...");
    disconnect(m_pd_video, SIGNAL(finished(int)), this, SLOT(pdrestart()));
    pdstart();
}

// New conexion on TCP Server

void libreMediaServer::newPeer()
{
   m_read_vid = m_server_vid->nextPendingConnection();
   connect(m_read_vid, SIGNAL(readyRead()),
           this, SLOT(newmessage()));
}

// New message in a TCP socket stablished connection

void libreMediaServer::newmessage()
{
    if (m_read_vid == NULL)
    {
        qDebug()<<("Local Socket not created");
        newPeer();
        return;
    }
    QByteArray byteArray;
    byteArray.resize(m_read_vid->bytesAvailable());
    byteArray = m_read_vid->readAll();
    if (byteArray == NULL)
    {
        return;
    }
    QPixmap frame;
    switch (byteArray.at(0)) {
    case 11:
        byteArray.remove(0,2);
        if (!frame.loadFromData((byteArray))) {
            qDebug()<<"Layer 1 Convert byte Array to frame failed ";
        }
        ui.layer1Preview->setPixmap(frame);
        break;

    case 12:
        byteArray.remove(0,2);
        if (!frame.loadFromData((byteArray))) {
            qDebug()<<"Layer 2 Convert byte Array to frame failed ";
        }
        ui.layer2Preview->setPixmap(frame);
        break;
    case 13:
        byteArray.remove(0,2);
        if (!frame.loadFromData((byteArray))) {
            qDebug()<<"Layer 3 Convert byte Array to frame failed ";
            break;
        }
        ui.layer3Preview->setPixmap(frame);
        break;
    case 14:
        byteArray.remove(0,2);
        if (!frame.loadFromData(byteArray)) {
            qDebug()<<"Layer 4 Convert byte Array to frame failed ";
            break;
        }
        ui.layer4Preview->setPixmap(frame);
        break;
    case 15:
        byteArray.remove(0,2);
        if (!frame.loadFromData(byteArray)) {
                qDebug()<<"Layer 5 Convert byte Array to frame failed ";
                break;
        }
        ui.layer5Preview->setPixmap(frame);
        break;
    case 16:
        byteArray.remove(0,2);
        if (!frame.loadFromData(byteArray)) {
                qDebug()<<"Layer 6 Convert byte Array to frame failed ";
                break;
        }
                ui.layer6Preview->setPixmap(frame);
        break;
    case 17:
        byteArray.remove(0,2);
        if (!frame.loadFromData(byteArray)) {
                qDebug()<<"Layer 7 Convert byte Array to frame failed ";
                break;
        }
        ui.layer7Preview->setPixmap(frame);
        break;
    case 18:
        byteArray.remove(0,2);
        if (!frame.loadFromData(byteArray)) {
                qDebug()<<"Layer 8 Convert byte Array to frame failed ";
                break;
        }
        ui.layer8Preview->setPixmap(frame);
        break;
    default:
        qDebug()<<"newmessage: Message received but can not identify the cooki";
        break;
    }
}

// Send the configuration to PD
void libreMediaServer::newconexion()
{
    // Iniciamos el socket
    m_pd_write_video->connectToHost(QHostAddress::LocalHost, PDPORTW);
    m_pd_write_video->waitForConnected(300000);
    if (!(m_pd_write_video->isOpen())){
        qErrnoWarning("newconexion:pd write socket not open!:");
        return;
     }
    //Mandamos el path
    QString desc = tr("0000 0000 %1;").arg(m_pathmedia);
    if (!sendPacket(desc.toAscii().constData(),desc.size()))
    {
      ui.textEdit->appendPlainText("newconexion: Can not send the path to PD Video");
      return;
    }
    // send the layer adress
    on_layer1Check_stateChanged (ui.layer1Check->checkState());
    on_layer2Check_stateChanged (ui.layer2Check->checkState());
    on_layer3Check_stateChanged (ui.layer3Check->checkState());
    on_layer4Check_stateChanged (ui.layer4Check->checkState());
    on_layer5Check_stateChanged (ui.layer5Check->checkState());
    on_layer6Check_stateChanged (ui.layer6Check->checkState());
    on_layer7Check_stateChanged (ui.layer7Check->checkState());
    on_layer8Check_stateChanged (ui.layer8Check->checkState());
    // send the dmx on/off
    on_readDMX_stateChanged(ui.readDMX->checkState());
    // send the windows info
    on_winpositionx_valueChanged();
    on_winpositiony_valueChanged();
    on_winsizex_valueChanged();
    on_winsizey_valueChanged();
    on_window_stateChanged(ui.window->checkState());
}

// Sends packets to Pure Data video

bool libreMediaServer::sendPacket(const char *buffer, int bufferLen)
{
 if (m_pd_write_video == NULL) {
     qErrnoWarning("sendPacket:Socket not initialized:");
     return false;
 }
 if (QAbstractSocket::ConnectedState != m_pd_write_video->state())
 {
     qErrnoWarning("sendPacket:Socket not conected:");
     return false;
 }
 if (bufferLen != m_pd_write_video->write((const char*)buffer, bufferLen))
 {
     qErrnoWarning("sendPacket:Can not write to socket::");
     return false;
 }
 return true;
}

// Function error sending packets to PD video

void libreMediaServer::errorsending() {
    if (ui.video->checkState())
    {
    qErrnoWarning("errorsending: Can not talk to Pure Data Video!");
    ui.textEdit->appendPlainText("errorsending: Can not send packets to PD Video");
    }
}

///////////////////////////////////////////////////////////////////
// Audio Controls
///////////////////////////////////////////////////////////////////

void libreMediaServer::on_layer1Check_audio_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0011 0000;");
        if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_audio();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer1Add_audio_valueChanged();
    }
}

void libreMediaServer::on_layer1Add_audio_valueChanged()
{
   if (ui.layer1Check_audio->isChecked()){
        int x = ui.layer1Add_audio->value();
        QString desc = tr("0011 %1;").arg(x);
        if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
           {
            errorsending_audio();
           }
    }
}

void libreMediaServer::on_layer2Check_audio_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0012 0000;");
        if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_audio();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer2Add_audio_valueChanged();
    }
}

void libreMediaServer::on_layer2Add_audio_valueChanged()
{
   if (ui.layer2Check_audio->isChecked()){
   int x = ui.layer2Add_audio->value();
   QString desc = tr("0012 %1;").arg(x);
   if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
           {
            errorsending_audio();
           }
   }
}

void libreMediaServer::on_layer3Check_audio_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0013 0000;");
        if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_audio();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer3Add_audio_valueChanged();
    }
}

void libreMediaServer::on_layer3Add_audio_valueChanged()
{
   if (ui.layer3Check_audio->isChecked()){
   int x = ui.layer3Add_audio->value();
   QString desc = tr("0013 %1;").arg(x);
   if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
           {
            errorsending_audio();
           }
  }
}

void libreMediaServer::on_layer4Check_audio_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0014 0000;");
        if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_audio();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer4Add_audio_valueChanged();
    }
}

void libreMediaServer::on_layer4Add_audio_valueChanged()
{
   if (ui.layer4Check_audio->isChecked()){
   int x = ui.layer4Add_audio->value();
   QString desc = tr("0014 %1;").arg(x);
   if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
           {
            errorsending_audio();
           }
   }
}

void libreMediaServer::on_layer5Check_audio_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0015 0000;");
        if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_audio();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer5Add_audio_valueChanged();
    }
}

void libreMediaServer::on_layer5Add_audio_valueChanged()
{
   if (ui.layer5Check_audio->isChecked()){
   int x = ui.layer5Add_audio->value();
   QString desc = tr("0015 %1;").arg(x);
   if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
           {
            errorsending_audio();
           }
    }
}

void libreMediaServer::on_layer6Check_audio_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0016 0000;");
        if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_audio();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer6Add_audio_valueChanged();
    }
}

void libreMediaServer::on_layer6Add_audio_valueChanged()
{
   if (ui.layer6Check_audio->isChecked()){
   int x = ui.layer6Add_audio->value();
   QString desc = tr("0016 %1;").arg(x);
   if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
           {
            errorsending_audio();
           }
   }
}

void libreMediaServer::on_layer7Check_audio_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0017 0000;");
        if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_audio();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer7Add_audio_valueChanged();
    }
}

void libreMediaServer::on_layer7Add_audio_valueChanged()
{
   if (ui.layer7Check_audio->isChecked()){
   int x = ui.layer7Add_audio->value();
   QString desc = tr("0017 %1;").arg(x);
   if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
           {
            errorsending_audio();
           }
   }
}

void libreMediaServer::on_layer8Check_audio_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0018 0000;");
        if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_audio();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer8Add_audio_valueChanged();
    }
}

void libreMediaServer::on_layer8Add_audio_valueChanged()
{
   if (ui.layer8Check_audio->isChecked()){
   int x = ui.layer8Add_audio->value();
   QString desc = tr("0018 %1;").arg(x);
   if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
           {
            errorsending_audio();
           }
  }
}

// Open the connection with OLA and start reading DMX

void libreMediaServer::on_readDMX_audio_stateChanged(int state)
{
    if ((state == 0)) {
        QString desc("0020 0000;");
        if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_audio();
                }
        }
    if ((state == 2))
    {
        int x = ui.universe_audio->value();
        QString desc = tr("0021 %1;").arg(x);
        if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_audio();
                }
        desc = tr("0020 0001;");
        if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_audio();
                }
        }
 }

// Open the audio process

void libreMediaServer::on_audio_stateChanged(int state)
{
    if ((state == 0))
         {
        if (m_pd_write_audio != NULL)
        {
            m_pd_write_audio->close();
            disconnect(m_pd_write_audio, SIGNAL(connected()),this, SLOT(newconexion_audio()));
            m_pd_write_audio == NULL;
        }
        if (m_pd_read_audio != NULL)
        {
            disconnect(m_pd_read_audio, SIGNAL(newConnection()),this, SLOT(newPeer_audio()));
            m_pd_read_audio->close();
            m_pd_read_audio == NULL;
        }
        if (m_tcpsocket_audio != NULL)
        {
            m_tcpsocket_audio->close();
            m_tcpsocket_audio == NULL;
        }

        disconnect(m_pd_audio, SIGNAL(finished(int)), this, SLOT(pdrestart_audio()));
        m_pd_audio->terminate();
        m_pd_audio = NULL;
    }
    if ((state == 2))
    {
   // Iniciamos Pure Data
   m_pd_read_audio = NULL;
   m_pd_write_audio = NULL;
   m_pd_audio = new QProcess(this);
   pdstart_audio();
   }
}
///////////////////////////////////////////////////////////////////
/*
 * Pure Data Audio Stuff
 */
///////////////////////////////////////////////////////////////////

// Start the PD Process, open the ports and connects stdout de Pure Data.

void libreMediaServer::pdstart_audio()
{
    if (m_pd_audio->state() != 0)
    {
        return;
    }
    // Creamos los sockets para la conexión a Pure Data
    m_pd_write_audio = new QTcpSocket(this);
    Q_CHECK_PTR(m_pd_write_audio);
    connect(m_pd_write_audio, SIGNAL(connected()),this, SLOT(newconexion_audio()));
    m_pd_read_audio = new QTcpServer(this);
    Q_CHECK_PTR(m_pd_read_audio);
    connect(m_pd_read_audio, SIGNAL(newConnection()),this, SLOT(newPeer_audio()));
    if (!m_pd_read_audio)
     {
         qDebug()<<("error TCP Server no creado");
     }
    if (!m_pd_read_audio->listen(QHostAddress::LocalHost, PDPORTR_AUDIO))
    {
    qDebug()<<"error listening tcpServer";
    }
    // Arrancamos el proceso Pure Data
    m_pd_audio->start("pd -path /usr/lib/pd/extra/pdogg -path /usr/lib/pd/extra/pan pms-audio.pd");
    if (m_pd_audio->waitForStarted(3000)){
        ui.textEdit->appendPlainText("PD Audio started.");
    }
    else
    {
        ui.textEdit->appendPlainText("PD Audio not started!");
        return;
    }
    connect(m_pd_audio, SIGNAL(readyReadStandardError()), this, SLOT(stdout_audio()));
}

// Sacamos la salida de Pure Data en la terminal

void libreMediaServer::stdout_audio() {
    QString out = m_pd_audio->readAllStandardError();
    out.chop(1);
    if (!out.isEmpty())
    {
        qDebug() << out;
//        ui.textEdit->appendPlainText(out);
    }
}

// Restart the Pure Data process if crash. Connected wit signal finished of QProcess

void libreMediaServer::pdrestart_audio()
{
    save();
    qDebug()<<"Restarting PD audio";
    ui.textEdit->appendPlainText("PD audio Restarting...");
    int state = m_pd_audio->state();
    if (state != 0)
    {
        return;
    }
    if (m_pd_write_audio != NULL)
    {
        m_pd_write_audio->close();
        disconnect(m_pd_write_audio, SIGNAL(connected()),this, SLOT(newconexion_audio()));
        delete m_pd_write_audio;
    }
    if (m_pd_read_audio != NULL)
    {
        disconnect(m_pd_read_audio, SIGNAL(newConnection()),this, SLOT(newPeer_audio()));
        m_pd_read_audio->close();
        delete m_pd_read_audio;
    }
    disconnect(m_pd_audio, SIGNAL(finished(int)), this, SLOT(pdrestart_audio()));
    pdstart_audio();
}

// New conexion on TCP Server

void libreMediaServer::newPeer_audio()
{
   m_tcpsocket_audio = m_pd_read_audio->nextPendingConnection();
   connect(m_tcpsocket_audio, SIGNAL(readyRead()),
                this, SLOT(newmessage_audio()));
}

// New message in a TCP socket stablished connection

void libreMediaServer::newmessage_audio()
{
    if (m_tcpsocket_audio == NULL)
    {
        qDebug()<<("tcpsocket audio not created");
        newPeer_audio();
        return;
    }
    QByteArray byteArray = m_tcpsocket_audio->readAll();
    QString string(byteArray);
    if (byteArray.at(0) == 0)
    {
        return;
    }
    QChar layer = string.at(0);
    int i = 9 + m_pathmedia.size();
    string.remove(0,i);
    string.chop(2);
    int val = layer.digitValue();
    switch (val) {
    case 0:
        ui.textEdit->appendPlainText("Loadbang from PD Audio received...");
        // Conectamos a Pure Data para escribir
        m_pd_write_audio->connectToHost(QHostAddress::LocalHost, PDPORTW_AUDIO);
        // Conectamos para reiniciar si PD crash
        connect(m_pd_audio, SIGNAL(finished(int)), this, SLOT(pdrestart_audio()));
        // Mandamos Configuración
        m_pd_write_audio->waitForConnected(30000);
        newconexion_audio();
    case 1:
       ui.layer1_audio->setText(string);
       break;
    case 2:
        ui.layer2_audio->setText(string);
        break;
    case 3:
        ui.layer3_audio->setText(string);
        break;
    case 4:
        ui.layer4_audio->setText(string);
        break;
    case 5:
        ui.layer5_audio->setText(string);
        break;
    case 6:
        ui.layer6_audio->setText(string);
        break;
    case 7:
        ui.layer7_audio->setText(string);
        break;
    case 8:
        ui.layer8_audio->setText(string);
        break;
    }
}

// Send the configuration to PD
void libreMediaServer::newconexion_audio()
{
    if (!(m_pd_write_audio->isOpen())){
        errorsending_audio();
        return;
     }
    qDebug() << "Sending conf to PD Audio";
    QString desc = tr("0000 0000 %1;").arg(m_pathmedia);
    if (!sendPacket_audio(desc.toAscii().constData(),desc.size()))
    {
      errorsending_audio();
      return;
    }
    on_layer1Check_audio_stateChanged (ui.layer1Check_audio->checkState());
    on_layer2Check_audio_stateChanged (ui.layer2Check_audio->checkState());
    on_layer3Check_audio_stateChanged (ui.layer3Check_audio->checkState());
    on_layer4Check_audio_stateChanged (ui.layer4Check_audio->checkState());
    on_layer5Check_audio_stateChanged (ui.layer5Check_audio->checkState());
    on_layer6Check_audio_stateChanged (ui.layer6Check_audio->checkState());
    on_layer7Check_audio_stateChanged (ui.layer7Check_audio->checkState());
    on_layer8Check_audio_stateChanged (ui.layer8Check_audio->checkState());
    on_readDMX_audio_stateChanged(ui.readDMX_audio->checkState());

}

// Sends packets to Pure Data audio

bool libreMediaServer::sendPacket_audio(const char *buffer, int bufferLen)
{
 if (m_pd_write_audio == NULL) {
    return false;
 }
 if (QAbstractSocket::ConnectedState != m_pd_write_audio->state())
 {
    return false;
 }
 if (bufferLen != m_pd_write_audio->write((const char*)buffer, bufferLen))
 {
    return false;
 }
 return true;
}

// Function error sending packets to PD audio

void libreMediaServer::errorsending_audio() {
    if (ui.audio->checkState())
    {
    ui.textEdit->appendPlainText("Can not send packets to PD Video");
    }
}

///////////////////////////////////////////////////////////////////
//
// Previews
//
///////////////////////////////////////////////////////////////////
// GUI
void libreMediaServer::previewMaster()
{
    QPixmap preview = QPixmap::grabWindow(QApplication::desktop()->winId(), ui.winpositionx->value() , ui.winpositiony->value(),ui.winsizex->value(),ui.winsizey->value());
    ui.masterPreview->setPixmap(preview);
}
// CITP/MSEx 1.0
void libreMediaServer::sendFrame()
{
    m_msex->n_timer->stop();
    QPixmap frame = QPixmap::grabWindow(QApplication::desktop()->winId(), ui.winpositionx->value() , ui.winpositiony->value(),ui.winsizex->value(),ui.winsizey->value());
    if (!frame) {
        qDebug()<<"sendFrame: Can not take frame";
        return;
    }
    QImage image = QImage(frame.toImage());
    if (!image.byteCount()) {
        qDebug()<<"sendFrame: Can not convert screen capture to image";
        return;
    }
    image = image.scaledToWidth(88);
    image = image.convertToFormat(QImage::Format_RGB888);
    int bufferLen = image.byteCount();
    int bufferLenTot = sizeof(struct CITP_MSEX_10_StFr ) + bufferLen;
    uchar * buffer = new uchar[bufferLenTot];
    memset(buffer, 0, bufferLenTot);
    CITP_MSEX_10_StFr *packet = (CITP_MSEX_10_StFr *)buffer;
    // CITP header
    packet->CITPMSEXHeader.CITPHeader.Cookie = COOKIE_CITP;
    packet->CITPMSEXHeader.CITPHeader.VersionMajor = 0x01;
    packet->CITPMSEXHeader.CITPHeader.VersionMinor = 0x00;
    packet->CITPMSEXHeader.CITPHeader.Reserved[0] = 0x00;
    packet->CITPMSEXHeader.CITPHeader.Reserved[1] = 0x00;
    packet->CITPMSEXHeader.CITPHeader.MessageSize = bufferLenTot;
    packet->CITPMSEXHeader.CITPHeader.MessagePartCount = 0x01;
    packet->CITPMSEXHeader.CITPHeader.MessagePart = 0x00; // XXX - doc says 0-based?
    packet->CITPMSEXHeader.CITPHeader.ContentType = COOKIE_MSEX;
    // MSEX header
    packet->CITPMSEXHeader.ContentType = COOKIE_MSEX_STFR;
    packet->CITPMSEXHeader.VersionMajor = 0x01;
    packet->CITPMSEXHeader.VersionMinor = 0x00;
    //STFR Content
    packet->FrameBufferSize = bufferLen;
    packet->FrameFormat = 943867730;
    packet->FrameHeight = 64;
    packet->FrameWidth = 88;
    packet->SourceIdentifier = 0x0001;
    // Copiamos los datos del thumbnail
    int offset = sizeof(struct CITP_MSEX_10_StFr);
    memcpy((buffer+offset), image.bits(), bufferLen);
    if (!buffer)
    {
      qDebug() << "sendFrame:create Frame failed";
      return;
    }
    // Mandamos el paquete
    if (!m_msex->sendPacket(buffer, bufferLenTot))
        {
        qDebug() << "sendFrame: transmitFrame failed";
        return;
        }
    m_msex->n_timer->start();
}
