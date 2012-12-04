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

// Esto habrá que cambiarlo para poder utilizar varias instancias
#define PDPORTW 9195
#define PDPORTR 9196

#define PDPORTW_AUDIO 9197
#define PDPORTR_AUDIO 9198

#define PDPORTW_TEXT 9199
#define PDPORTR_TEXT 9200

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

// Text Configuration
bool window_text;
quint16 winpositionx_text;
quint16 winpositiony_text;
quint16 winsizex_text;
quint16 winsizey_text;
quint16 layer1Add_text;
bool layer1Check_text;
quint16 layer2Add_text;
bool layer2Check_text;
quint16 layer3Add_text;
bool layer3Check_text;
quint16 layer4Add_text;
bool layer4Check_text;
quint16 layer5Add_text;
bool layer5Check_text;
quint16 layer6Add_text;
bool layer6Check_text;
quint16 layer7Add_text;
bool layer7Check_text;
quint16 layer8Add_text;
bool layer8Check_text;
bool dmx_text;
quint8 universe_text;
bool textCheck;

// Path to Media Files
QString path;
};

///////////////////////////////////////////////////////////////////

PureMediaServer::PureMediaServer(QWidget *parent)
  : QMainWindow(parent)
{
    // Iniciamos el User Interface
     ui.setupUi(this);
     m_tcpsocket = NULL;
     m_pd_read = NULL;
     m_pd_write = NULL;
     pd = NULL;
     m_tcpsocket_audio = NULL;
     m_pd_read_audio = NULL;
     m_pd_write_audio = NULL;
     pd_audio = NULL;
     m_tcpsocket_text = NULL;
     m_pd_read_text = NULL;
     m_pd_write_text = NULL;
     pd_text = NULL;
     // Load the configuration
     open();
     // Iniciamos olad
    ola = new QProcess(this);
    olastart();
    // Creamos el mediaserver
    m_mediaserver = new MediaServer(this);
    Q_CHECK_PTR(m_mediaserver);
    // Conectamos los menus
    connect(ui.actionOpen_conf, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui.actionSave_conf, SIGNAL(triggered()), this, SLOT(save()));
}

///////////////////////////////////////////////////////////////////

PureMediaServer::~PureMediaServer()
{
    save();
    if (m_pd_write != NULL) {
          m_pd_write->abort();
        }
    if (m_pd_read != NULL) {
        m_pd_read->close();
    }
    if (m_tcpsocket != NULL) {
        m_tcpsocket->close();
    }
    if (pd != NULL)
    {
        disconnect(pd, SIGNAL(finished(int)), this, SLOT(pdrestart()));
        pd->close();
    }
        ola->close();
}

/*
 * File Configuration Stuff
 */

///////////////////////////////////////////////////////////////////
// Load the last configuration from the file pms.conf
///////////////////////////////////////////////////////////////////

void PureMediaServer::open()
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

    // Text Configuration

    ui.window_text->setChecked(packet->window_text);
    ui.winpositionx_text->setValue(packet->winpositionx_text);
    ui.winpositiony_text->setValue(packet->winpositiony_text);
    ui.winsizex_text->setValue(packet->winsizex_text);
    ui.winsizey_text->setValue(packet->winsizey_text);
    ui.layer1Add_text->setValue(packet->layer1Add_text);
    ui.layer1Check_text->setChecked(packet->layer1Check_text);
    ui.layer2Add_text->setValue(packet->layer2Add_text);
    ui.layer2Check_text->setChecked(packet->layer2Check_text);
    ui.layer3Add_text->setValue(packet->layer3Add_text);
    ui.layer3Check_text->setChecked(packet->layer3Check_text);
    ui.layer4Add_text->setValue(packet->layer4Add_text);
    ui.layer4Check_text->setChecked(packet->layer4Check_text);
    ui.layer5Add_text->setValue(packet->layer5Add_text);
    ui.layer5Check_text->setChecked(packet->layer5Check_text);
    ui.layer6Add_text->setValue(packet->layer6Add_text);
    ui.layer6Check_text->setChecked(packet->layer6Check_text);
    ui.layer7Add_text->setValue(packet->layer7Add_text);
    ui.layer7Check_text->setChecked(packet->layer7Check_text);
    ui.layer8Add_text->setValue(packet->layer8Add_text);
    ui.layer8Check_text->setChecked(packet->layer8Check_text);
    ui.readDMX_text->setChecked(packet->dmx_text);
    ui.universe_text->setValue(packet->universe_text);
    ui.text->setChecked(packet->textCheck);

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

///////////////////////////////////////////////////////////////////
// Save the configuration to pms.conf file
///////////////////////////////////////////////////////////////////

void PureMediaServer::save()
{
    qDebug()<<("Saving conf file");
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

    // Text Configuration

    packet->window_text = ui.window_text->checkState();
    packet->winpositionx_text = ui.winpositionx_text->value();
    packet->winpositiony_text = ui.winpositiony_text->value();
    packet->winsizex_text = ui.winsizex_text->value();
    packet->winsizey_text = ui.winsizey_text->value();
    packet->layer1Add_text = ui.layer1Add_text->value();
    packet->layer1Check_text = ui.layer1Check_text->checkState();
    packet->layer2Add_text = ui.layer2Add_text->value();
    packet->layer2Check_text = ui.layer2Check_text->checkState();
    packet->layer3Add_text = ui.layer3Add_text->value();
    packet->layer3Check_text = ui.layer3Check_text->checkState();
    packet->layer4Add_text = ui.layer4Add_text->value();
    packet->layer4Check_text = ui.layer4Check_text->checkState();
    packet->layer5Add_text = ui.layer5Add_text->value();
    packet->layer5Check_text = ui.layer5Check_text->checkState();
    packet->layer6Add_text = ui.layer6Add_text->value();
    packet->layer6Check_text = ui.layer6Check_text->checkState();
    packet->layer7Add_text = ui.layer7Add_text->value();
    packet->layer7Check_text = ui.layer7Check_text->checkState();
    packet->layer8Add_text = ui.layer8Add_text->value();
    packet->layer8Check_text = ui.layer8Check_text->checkState();
    packet->dmx_text = ui.readDMX_text->checkState();
    packet->universe_text = ui.universe_text->value();
    packet->textCheck = ui.text->checkState();

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
    qDebug()<<(errorstring);
    file.close();
}

///////////////////////////////////////////////////////////////////
// OLA Stuff
///////////////////////////////////////////////////////////////////

void PureMediaServer::olastart()
{
    qDebug()<<("Starting OLA");
    ola->start("olad", QStringList()<< "-l 3");
//    connect(ola, SIGNAL(finished(int)), this, SLOT(olastart()));
}

/*
 * User Interface Stuff
 */

///////////////////////////////////////////////////////////////////
// Global Controls
///////////////////////////////////////////////////////////////////

// Change Media path

void PureMediaServer::on_ChangePath_clicked()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    QStringList fileNames;
    if (dialog.exec())
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
    if (ui.text->checkState())
    {
        if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
        {
            errorsending_text();
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
// ToDo: Include the thums generation here

void PureMediaServer::on_updateButton_clicked()
{
    // Chequeamos si existe el path a los medias
    QDir dir(m_pathmedia);
     if (!dir.exists())
     {
         qDebug()<<("Cannot find the media directory");
         ui.textEdit->appendPlainText("Can not find the media directory in the path given");
         return;
     }
     m_mediaserver->setpath(m_pathmedia);
     if (!m_mediaserver->updatemedia())
     {
         qDebug()<<("Cannot explore the media");
         ui.textEdit->appendPlainText("Can not explore the media in the path given");
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
           qDebug()<<("CreatePeerInformationSocket failed");
           ui.textEdit->appendPlainText("CITP/MSEx error. No interface up?");
            }
}

// Window Configuration

void PureMediaServer::on_window_stateChanged(int state)
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

void PureMediaServer::on_winpositionx_valueChanged()
{
    int x = ui.winpositionx->value();
    QString desc = tr("0002 %1;").arg(x);
    if (!sendPacket(desc.toAscii().constData(),desc.size()))
            {
             errorsending();
            }

}

void PureMediaServer::on_winpositiony_valueChanged()
{
    int x = ui.winpositiony->value();
    QString desc = tr("3 %1;").arg(x);
    if (!sendPacket(desc.toAscii().constData(),desc.size()))
            {
             errorsending();
            }

}

void PureMediaServer::on_winsizex_valueChanged()
{
    int x = ui.winsizex->value();
    QString desc = tr("4 %1;").arg(x);
    if (!sendPacket(desc.toAscii().constData(),desc.size()))
            {
             errorsending();
            }

}

void PureMediaServer::on_winsizey_valueChanged()
{
    int x = ui.winsizey->value();
    QString desc = tr("5 %1;").arg(x);
    if (!sendPacket(desc.toAscii().constData(),desc.size()))
            {
             errorsending();
            }
}

// DMX address configuration

void PureMediaServer::on_layer1Check_stateChanged (int state)
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

void PureMediaServer::on_layer1Add_valueChanged()
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

void PureMediaServer::on_layer2Check_stateChanged (int state)
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

void PureMediaServer::on_layer2Add_valueChanged()
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

void PureMediaServer::on_layer3Check_stateChanged (int state)
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

void PureMediaServer::on_layer3Add_valueChanged()
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

void PureMediaServer::on_layer4Check_stateChanged (int state)
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

void PureMediaServer::on_layer4Add_valueChanged()
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

void PureMediaServer::on_layer5Check_stateChanged (int state)
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

void PureMediaServer::on_layer5Add_valueChanged()
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

void PureMediaServer::on_layer6Check_stateChanged (int state)
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

void PureMediaServer::on_layer6Add_valueChanged()
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

void PureMediaServer::on_layer7Check_stateChanged (int state)
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

void PureMediaServer::on_layer7Add_valueChanged()
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

void PureMediaServer::on_layer8Check_stateChanged (int state)
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

void PureMediaServer::on_layer8Add_valueChanged()
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

void PureMediaServer::on_readDMX_stateChanged(int state)
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

void PureMediaServer::on_video_stateChanged(int state)
{
    if ((state == 0))
         {
        if (m_pd_write != NULL)
        {
            m_pd_write->close();
            disconnect(m_pd_write, SIGNAL(connected()),this, SLOT(newconexion()));
            m_pd_write == NULL;
        }
        if (m_pd_read != NULL)
        {
            disconnect(m_pd_read, SIGNAL(newConnection()),this, SLOT(newPeer()));
            m_pd_read->close();
            m_pd_read == NULL;
        }
        if (m_tcpsocket != NULL){
            m_tcpsocket->close();
            m_tcpsocket == NULL;
        }

        disconnect(pd, SIGNAL(finished(int)), this, SLOT(pdrestart()));
        pd->terminate();
        pd = NULL;
    }
    if ((state == 2))
    {
   // Iniciamos Pure Data
   m_pd_read = NULL;
   m_pd_write = NULL;
   pd = new QProcess(this);
   pdstart();
   }
}

///////////////////////////////////////////////////////////////////
/*
 * Pure Data Video Stuff
 */
///////////////////////////////////////////////////////////////////

// Start the PD Process, open the ports and connects stdout de Pure Data.

void PureMediaServer::pdstart()
{
    if (pd->state() != 0)
    {
        return;
    }
    // Creamos los sockets para la conexión a Pure Data
    m_pd_write = new QTcpSocket(this);
    Q_CHECK_PTR(m_pd_write);
    connect(m_pd_write, SIGNAL(connected()),this, SLOT(newconexion()));
    m_pd_read = new QTcpServer(this);
    Q_CHECK_PTR(m_pd_read);
    connect(m_pd_read, SIGNAL(newConnection()),this, SLOT(newPeer()));
    if (!m_pd_read)
     {
         qDebug()<<("error TCP Server no creado");
     }
    if (!m_pd_read->listen(QHostAddress::LocalHost, PDPORTR))
    {
    qDebug()<<"error listening tcpServer";
    }
    // Arrancamos el proceso Pure Data
    pd->start("pd -path /usr/lib/pd/extra/cyclone -lib Gem pms-video.pd");
    if (pd->waitForStarted(3000)){
        ui.textEdit->appendPlainText("PD-Video started.");
    }
    else
    {
        ui.textEdit->appendPlainText("PD_Video not started!");
        return;
    }
    connect(pd, SIGNAL(readyReadStandardError()), this, SLOT(stdout()));
}

// Sacamos la salida de Pure Data en la terminal

void PureMediaServer::stdout() {
    QString out = pd->readAllStandardError();
    out.chop(1);
    if (!out.isEmpty())
    {
        qDebug() << out;
//        ui.textEdit->appendPlainText(out);
    }
}

// Restart the Pure Data process if crash. Connected wit signal finished of QProcess

void PureMediaServer::pdrestart()
{
    save();
    qDebug()<<"Restarting PD";
    ui.textEdit->appendPlainText("PD Restarting...");
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

// New conexion on TCP Server

void PureMediaServer::newPeer()
{
   m_tcpsocket = m_pd_read->nextPendingConnection();
   connect(m_tcpsocket, SIGNAL(readyRead()),
                this, SLOT(newmessage()));
}

// New message in a TCP socket stablished connection

void PureMediaServer::newmessage()
{
    if (m_tcpsocket == NULL)
    {
        qDebug()<<("tcpsocket not created");
        newPeer();
        return;
    }
    QByteArray byteArray = m_tcpsocket->readAll();
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
        ui.textEdit->appendPlainText("Loadbang received...");
        // Conectamos a Pure Data para escribir
        m_pd_write->connectToHost(QHostAddress::LocalHost, PDPORTW);
        // Conectamos para reiniciar si PD crash
        connect(pd, SIGNAL(finished(int)), this, SLOT(pdrestart()));
        // Mandamos Configuración
        m_pd_write->waitForConnected(30000);
        newconexion();
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

// Send the configuration to PD
void PureMediaServer::newconexion()
{
    if (!(m_pd_write->isOpen())){
        errorsending();
        return;
     }
    qDebug() << "Sending conf tu PD";
    QString desc = tr("0000 0000 %1;").arg(m_pathmedia);
    if (!sendPacket(desc.toAscii().constData(),desc.size()))
    {
      errorsending();
      return;
    }
    on_layer1Check_stateChanged (ui.layer1Check->checkState());
    on_layer2Check_stateChanged (ui.layer2Check->checkState());
    on_layer3Check_stateChanged (ui.layer3Check->checkState());
    on_layer4Check_stateChanged (ui.layer4Check->checkState());
    on_layer5Check_stateChanged (ui.layer5Check->checkState());
    on_layer6Check_stateChanged (ui.layer6Check->checkState());
    on_layer7Check_stateChanged (ui.layer7Check->checkState());
    on_layer8Check_stateChanged (ui.layer8Check->checkState());
    on_readDMX_stateChanged(ui.readDMX->checkState());
    on_winpositionx_valueChanged();
    on_winpositiony_valueChanged();
    on_winsizex_valueChanged();
    on_winsizey_valueChanged();
    on_window_stateChanged(ui.window->checkState());
}

// Sends packets to Pure Data video

bool PureMediaServer::sendPacket(const char *buffer, int bufferLen)
{
 if (m_pd_write == NULL) {
    return false;
 }
 if (QAbstractSocket::ConnectedState != m_pd_write->state())
 {
    return false;
 }
 if (bufferLen != m_pd_write->write((const char*)buffer, bufferLen))
 {
    return false;
 }
 return true;
}

// Function error sending packets to PD video

void PureMediaServer::errorsending() {
    if (ui.video->checkState())
    {
    qDebug() << "Can not talk to Pure Data Video!";
    ui.textEdit->appendPlainText("Can not send packets to PD Video");
    }
}

///////////////////////////////////////////////////////////////////
// Audio Controls
///////////////////////////////////////////////////////////////////

void PureMediaServer::on_layer1Check_audio_stateChanged (int state)
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

void PureMediaServer::on_layer1Add_audio_valueChanged()
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

void PureMediaServer::on_layer2Check_audio_stateChanged (int state)
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

void PureMediaServer::on_layer2Add_audio_valueChanged()
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

void PureMediaServer::on_layer3Check_audio_stateChanged (int state)
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

void PureMediaServer::on_layer3Add_audio_valueChanged()
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

void PureMediaServer::on_layer4Check_audio_stateChanged (int state)
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

void PureMediaServer::on_layer4Add_audio_valueChanged()
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

void PureMediaServer::on_layer5Check_audio_stateChanged (int state)
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

void PureMediaServer::on_layer5Add_audio_valueChanged()
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

void PureMediaServer::on_layer6Check_audio_stateChanged (int state)
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

void PureMediaServer::on_layer6Add_audio_valueChanged()
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

void PureMediaServer::on_layer7Check_audio_stateChanged (int state)
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

void PureMediaServer::on_layer7Add_audio_valueChanged()
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

void PureMediaServer::on_layer8Check_audio_stateChanged (int state)
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

void PureMediaServer::on_layer8Add_audio_valueChanged()
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

void PureMediaServer::on_readDMX_audio_stateChanged(int state)
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

void PureMediaServer::on_audio_stateChanged(int state)
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

        disconnect(pd_audio, SIGNAL(finished(int)), this, SLOT(pdrestart_audio()));
        pd_audio->terminate();
        pd_audio = NULL;
    }
    if ((state == 2))
    {
   // Iniciamos Pure Data
   m_pd_read_audio = NULL;
   m_pd_write_audio = NULL;
   pd_audio = new QProcess(this);
   pdstart_audio();
   }
}
///////////////////////////////////////////////////////////////////
/*
 * Pure Data Audio Stuff
 */
///////////////////////////////////////////////////////////////////

// Start the PD Process, open the ports and connects stdout de Pure Data.

void PureMediaServer::pdstart_audio()
{
    if (pd_audio->state() != 0)
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
    pd_audio->start("pd -path /usr/lib/pd/extra/pdogg -path /usr/lib/pd/extra/pan pms-audio.pd");
    if (pd_audio->waitForStarted(3000)){
        ui.textEdit->appendPlainText("PD Audio started.");
    }
    else
    {
        ui.textEdit->appendPlainText("PD Audio not started!");
        return;
    }
    connect(pd_audio, SIGNAL(readyReadStandardError()), this, SLOT(stdout_audio()));
}

// Sacamos la salida de Pure Data en la terminal

void PureMediaServer::stdout_audio() {
    QString out = pd_audio->readAllStandardError();
    out.chop(1);
    if (!out.isEmpty())
    {
        qDebug() << out;
//        ui.textEdit->appendPlainText(out);
    }
}

// Restart the Pure Data process if crash. Connected wit signal finished of QProcess

void PureMediaServer::pdrestart_audio()
{
    save();
    qDebug()<<"Restarting PD audio";
    ui.textEdit->appendPlainText("PD audio Restarting...");
    int state = pd_audio->state();
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
    disconnect(pd_audio, SIGNAL(finished(int)), this, SLOT(pdrestart_audio()));
    pdstart_audio();
}

// New conexion on TCP Server

void PureMediaServer::newPeer_audio()
{
   m_tcpsocket_audio = m_pd_read_audio->nextPendingConnection();
   connect(m_tcpsocket_audio, SIGNAL(readyRead()),
                this, SLOT(newmessage_audio()));
}

// New message in a TCP socket stablished connection

void PureMediaServer::newmessage_audio()
{
    if (m_tcpsocket_audio == NULL)
    {
        qDebug()<<("tcpsocket not created");
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
        connect(pd_audio, SIGNAL(finished(int)), this, SLOT(pdrestart_audio()));
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
void PureMediaServer::newconexion_audio()
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

bool PureMediaServer::sendPacket_audio(const char *buffer, int bufferLen)
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

void PureMediaServer::errorsending_audio() {
    if (ui.audio->checkState())
    {
    qDebug() << "Can not talk to Pure Data Video!";
    ui.textEdit->appendPlainText("Can not send packets to PD Video");
    }
}

///////////////////////////////////////////////////////////////////
// Text Controls
///////////////////////////////////////////////////////////////////

// Window Configuration

void PureMediaServer::on_window_text_stateChanged(int state)
{
    if ((state == 2)) {
       QString desc("0001 0001;");
       if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
       {
        errorsending_text();
       }
    }
    if ((state == 0)) {
          QString desc("0001 0000;");
          if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
          {
           errorsending_text();
          }
   }
}

void PureMediaServer::on_winpositionx_text_valueChanged()
{
    int x = ui.winpositionx_text->value();
    QString desc = tr("0002 %1;").arg(x);
    if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
            {
             errorsending_text();
            }
}

void PureMediaServer::on_winpositiony_text_valueChanged()
{
    int x = ui.winpositiony_text->value();
    QString desc = tr("3 %1;").arg(x);
    if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
            {
             errorsending_text();
            }
}

void PureMediaServer::on_winsizex_text_valueChanged()
{
    int x = ui.winsizex_text->value();
    QString desc = tr("4 %1;").arg(x);
    if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
            {
             errorsending_text();
            }
}

void PureMediaServer::on_winsizey_text_valueChanged()
{
    int x = ui.winsizey_text->value();
    QString desc = tr("5 %1;").arg(x);
    if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
            {
             errorsending_text();
            }
}

// DMX address configuration

void PureMediaServer::on_layer1Check_text_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0011 0000;");
        if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_text();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer1Add_text_valueChanged();
    }
}

void PureMediaServer::on_layer1Add_text_valueChanged()
{
   if (ui.layer1Check_text->isChecked()){
        int x = ui.layer1Add_text->value();
        QString desc = tr("0011 %1;").arg(x);
        if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
           {
            errorsending_text();
           }
    }
}

void PureMediaServer::on_layer2Check_text_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0012 0000;");
        if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_text();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer2Add_text_valueChanged();
    }
}

void PureMediaServer::on_layer2Add_text_valueChanged()
{
   if (ui.layer2Check_text->isChecked()){
   int x = ui.layer2Add_text->value();
   QString desc = tr("0012 %1;").arg(x);
   if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
           {
            errorsending_text();
           }
   }
}

void PureMediaServer::on_layer3Check_text_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0013 0000;");
        if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_text();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer3Add_text_valueChanged();
    }
}

void PureMediaServer::on_layer3Add_text_valueChanged()
{
   if (ui.layer3Check_text->isChecked()){
   int x = ui.layer3Add_text->value();
   QString desc = tr("0013 %1;").arg(x);
   if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
           {
            errorsending_text();
           }
  }
}

void PureMediaServer::on_layer4Check_text_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0014 0000;");
        if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_text();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer4Add_text_valueChanged();
    }
}

void PureMediaServer::on_layer4Add_text_valueChanged()
{
   if (ui.layer4Check_text->isChecked()){
   int x = ui.layer4Add_text->value();
   QString desc = tr("0014 %1;").arg(x);
   if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
           {
            errorsending_text();
           }
   }
}

void PureMediaServer::on_layer5Check_text_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0015 0000;");
        if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_text();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer5Add_text_valueChanged();
    }
}

void PureMediaServer::on_layer5Add_text_valueChanged()
{
   if (ui.layer5Check_text->isChecked()){
   int x = ui.layer5Add_text->value();
   QString desc = tr("0015 %1;").arg(x);
   if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
           {
            errorsending_text();
           }
    }
}

void PureMediaServer::on_layer6Check_text_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0016 0000;");
        if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_text();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer6Add_text_valueChanged();
    }
}

void PureMediaServer::on_layer6Add_text_valueChanged()
{
   if (ui.layer6Check_text->isChecked()){
   int x = ui.layer6Add_text->value();
   QString desc = tr("0016 %1;").arg(x);
   if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
           {
            errorsending_text();
           }
   }
}

void PureMediaServer::on_layer7Check_text_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0017 0000;");
        if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_text();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer7Add_text_valueChanged();
    }
}

void PureMediaServer::on_layer7Add_text_valueChanged()
{
   if (ui.layer7Check_text->isChecked()){
   int x = ui.layer7Add_text->value();
   QString desc = tr("0017 %1;").arg(x);
   if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
           {
            errorsending_text();
           }
   }
}

void PureMediaServer::on_layer8Check_text_stateChanged (int state)
{
    if ((state == 0))
    {
        QString desc("0018 0000;");
        if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_text();
                }
        return;
    }
    if ((state == 2))
    {
        on_layer8Add_text_valueChanged();
    }
}

void PureMediaServer::on_layer8Add_text_valueChanged()
{
   if (ui.layer8Check_text->isChecked()){
   int x = ui.layer8Add_text->value();
   QString desc = tr("0018 %1;").arg(x);
   if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
           {
            errorsending_text();
           }
  }
}

// Open the connection with OLA and start reading DMX

void PureMediaServer::on_readDMX_text_stateChanged(int state)
{
    if ((state == 0)) {
        QString desc("0020 0000;");
        if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_text();
                }
        }
    if ((state == 2))
    {
        int x = ui.universe_text->value();
        QString desc = tr("0021 %1;").arg(x);
        if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_text();
                }
        desc = tr("0020 0001;");
        if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
                {
                 errorsending_text();
                }
        }
 }

// Open the text process

void PureMediaServer::on_text_stateChanged(int state)
{
    if ((state == 0))
         {
        if (m_pd_write_text != NULL)
        {
            m_pd_write_text->close();
            disconnect(m_pd_write_text, SIGNAL(connected()),this, SLOT(newconexion_text()));
            m_pd_write_text == NULL;
        }
        if (m_pd_read_text != NULL)
        {
            disconnect(m_pd_read_text, SIGNAL(newConnection()),this, SLOT(newPeer_text()));
            m_pd_read_text->close();
            m_pd_read_text == NULL;
        }
        if (m_tcpsocket_text != NULL){
            m_tcpsocket_text->close();
            m_tcpsocket_text == NULL;
        }

        disconnect(pd_text, SIGNAL(finished(int)), this, SLOT(pdrestart_text()));
        pd_text->terminate();
        pd_text = NULL;
    }
    if ((state == 2))
    {
   // Iniciamos Pure Data
   m_pd_read_text = NULL;
   m_pd_write_text = NULL;
   pd_text = new QProcess(this);
   pdstart_text();
   }
}

///////////////////////////////////////////////////////////////////
/*
 * Pure Data Text
 */
///////////////////////////////////////////////////////////////////

// Start the PD Process, open the ports and connects stdout de Pure Data.

void PureMediaServer::pdstart_text()
{
    if (pd_text->state() != 0)
    {
        return;
    }
    // Creamos los sockets para la conexión a Pure Data
    m_pd_write_text = new QTcpSocket(this);
    Q_CHECK_PTR(m_pd_write_text);
    connect(m_pd_write_text, SIGNAL(connected()),this, SLOT(newconexion_text()));
    m_pd_read_text = new QTcpServer(this);
    Q_CHECK_PTR(m_pd_read_text);
    connect(m_pd_read_text, SIGNAL(newConnection()),this, SLOT(newPeer_text()));
    if (!m_pd_read_text)
     {
         qDebug()<<("error TCP Server no creado");
     }
    if (!m_pd_read_text->listen(QHostAddress::LocalHost, PDPORTR_TEXT))
    {
    qDebug()<<"error listening tcpServer";
    }
    // Arrancamos el proceso Pure Data
    pd_text->start("pd -path /usr/lib/pd/extra/cyclone -lib Gem pms-text.pd");
    if (pd_text->waitForStarted(3000)){
        ui.textEdit->appendPlainText("PD Text started.");
    }
    else
    {
        ui.textEdit->appendPlainText("PD Text not started!");
        return;
    }
    connect(pd_text, SIGNAL(readyReadStandardError()), this, SLOT(stdout_text()));
}

// Sacamos la salida de Pure Data en la terminal

void PureMediaServer::stdout_text() {
    QString out = pd_text->readAllStandardError();
    out.chop(1);
    if (!out.isEmpty())
    {
        qDebug() << out;
//        ui.textEdit->appendPlainText(out);
    }
}

// Restart the Pure Data process if crash. Connected wit signal finished of QProcess

void PureMediaServer::pdrestart_text()
{
    save();
    qDebug()<<"Restarting PD Text";
    ui.textEdit->appendPlainText("PD Text Restarting...");
    int state = pd_text->state();
    if (state != 0)
    {
        return;
    }
    if (m_pd_write_text != NULL)
    {
        m_pd_write_text->close();
        disconnect(m_pd_write_text, SIGNAL(connected()),this, SLOT(newconexion_text()));
        delete m_pd_write_text;
    }
    if (m_pd_read_text != NULL)
    {
        disconnect(m_pd_read_text, SIGNAL(newConnection()),this, SLOT(newPeer_text()));
        m_pd_read_text->close();
        delete m_pd_read_text;
    }
    disconnect(pd_text, SIGNAL(finished(int)), this, SLOT(pdrestart_text()));
    pdstart_text();
}

// New conexion on TCP Server

void PureMediaServer::newPeer_text()
{
   m_tcpsocket_text = m_pd_read_text->nextPendingConnection();
   connect(m_tcpsocket_text, SIGNAL(readyRead()),
                this, SLOT(newmessage_text()));
}

// New message in a TCP socket stablished connection

void PureMediaServer::newmessage_text()
{
    if (m_tcpsocket_text == NULL)
    {
        qDebug()<<("tcpsocket not created");
        newPeer_text();
        return;
    }
    QByteArray byteArray = m_tcpsocket_text->readAll();
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
        ui.textEdit->appendPlainText("Loadbang from PD Text received...");
        // Conectamos a Pure Data para escribir
        m_pd_write_text->connectToHost(QHostAddress::LocalHost, PDPORTW_TEXT);
        // Conectamos para reiniciar si PD crash
        connect(pd_text, SIGNAL(finished(int)), this, SLOT(pdrestart_text()));
        // Mandamos Configuración
        m_pd_write_text->waitForConnected(30000);
        newconexion_text();
    case 1:
       ui.layer1_text->setText(string);
       break;
    case 2:
        ui.layer2_text->setText(string);
        break;
    case 3:
        ui.layer3_text->setText(string);
        break;
    case 4:
        ui.layer4_text->setText(string);
        break;
    case 5:
        ui.layer5_text->setText(string);
        break;
    case 6:
        ui.layer6_text->setText(string);
        break;
    case 7:
        ui.layer7_text->setText(string);
        break;
    case 8:
        ui.layer8_text->setText(string);
        break;
    }
}

// Send the configuration to PD
void PureMediaServer::newconexion_text()
{
    if (!(m_pd_write_text->isOpen())){
        errorsending_text();
        return;
     }
    qDebug() << "Sending conf to PD";
    QString desc = tr("0000 0000 %1;").arg(m_pathmedia);
    if (!sendPacket_text(desc.toAscii().constData(),desc.size()))
    {
      errorsending_text();
      return;
    }
    on_layer1Check_text_stateChanged (ui.layer1Check_text->checkState());
    on_layer2Check_text_stateChanged (ui.layer2Check_text->checkState());
    on_layer3Check_text_stateChanged (ui.layer3Check_text->checkState());
    on_layer4Check_text_stateChanged (ui.layer4Check_text->checkState());
    on_layer5Check_text_stateChanged (ui.layer5Check_text->checkState());
    on_layer6Check_text_stateChanged (ui.layer6Check_text->checkState());
    on_layer7Check_text_stateChanged (ui.layer7Check_text->checkState());
    on_layer8Check_text_stateChanged (ui.layer8Check_text->checkState());
    on_readDMX_text_stateChanged(ui.readDMX_text->checkState());
    on_winpositionx_text_valueChanged();
    on_winpositiony_text_valueChanged();
    on_winsizex_text_valueChanged();
    on_winsizey_text_valueChanged();
    on_window_text_stateChanged(ui.window_text->checkState());
}

// Sends packets to Pure Data video

bool PureMediaServer::sendPacket_text(const char *buffer, int bufferLen)
{
 if (m_pd_write_text == NULL) {
    return false;
 }
 if (QAbstractSocket::ConnectedState != m_pd_write_text->state())
 {
    return false;
 }
 if (bufferLen != m_pd_write_text->write((const char*)buffer, bufferLen))
 {
    return false;
 }
 return true;
}

// Function error sending packets to PD video

void PureMediaServer::errorsending_text() {
    if (ui.text->checkState())
    {
    qDebug() << "Can not talk to Pure Data Text!";
    ui.textEdit->appendPlainText("Can not send packets to PD Text");
    }
}

