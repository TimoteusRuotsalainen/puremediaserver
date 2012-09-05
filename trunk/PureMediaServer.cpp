#include "PureMediaServer.h"
#include "CITPDefines.h"
#include "MSEXDefines.h"
#include "citp-lib.h"
#include "MediaServer.h"

#include <QtDebug>
#include <QtNetwork>
#include <QFileInfo>

#define PDPORT 9191

PureMediaServer::PureMediaServer(QWidget *parent)
  : QMainWindow(parent)
{
  // Creamos el objeto CITP
  ui.setupUi(this);
  m_citp = new CITPLib(this);
  Q_CHECK_PTR(m_citp);
  if (!m_citp->createPeerInformationSocket(NAME, STATE))
      {
      qDebug("CreatePeerInformationSocket failed");
  }

  // Creamos el mediaserver
  m_mediaserver = new MediaServer(this);
  Q_CHECK_PTR(m_mediaserver);

  // Creamos el socket para la conexión a Pure Data
  m_pd_socket = new QTcpSocket(this);
  Q_CHECK_PTR(m_pd_socket);
  connect(m_pd_socket, SIGNAL(connected()),this, SLOT(newconexion()));
  connect(m_pd_socket, SIGNAL(readyRead()), this, SLOT(handleReadyRead()));
}

PureMediaServer::~PureMediaServer()
{
    qDebug() << "Exit";
}

void PureMediaServer::close()
{
qDebug() << "bye!";
}

void PureMediaServer::on_updateButton_clicked()
{
    // Chequeamos si existe el path a los medias
    QDir dir(PATHMEDIA);
     if (!dir.exists())
     {
         qWarning("Cannot find the media directory");
         return;
     }
     ui.textEdit->appendPlainText("Actualizando biblioteca de medias. Esto puede llevar un rato");
     if (!m_mediaserver->updatemedia())
     {
         qWarning("Cannot explore the media");
         return;
     }
}

void PureMediaServer::on_connectPDButton_clicked(){

    QString desc = tr("Conectando con Pure Data on port %1").arg(PDPORT);
    ui.textEdit->appendPlainText(desc);
    qDebug()<<"Conectando a pure data on "<<PDPORT;
    m_pd_socket->connectToHost(QHostAddress::LocalHost, PDPORT);
}

void PureMediaServer::handleReadyRead()
{
    qDebug() << "Info arrives...";
}

bool PureMediaServer::sendPacket(const char *buffer, int bufferLen)
{
 if (!m_pd_socket) {
    return false;
 }
 if (QAbstractSocket::ConnectedState != m_pd_socket->state())
 {
    qDebug() << "pdInterface::sendPacket() - Socket not connected";
    return false;
 }
 if (bufferLen != m_pd_socket->write((const char*)buffer, bufferLen))
 {
    qDebug() << "pdInterface::sendPacket() write failed:" << m_pd_socket->error();
    return false;
 }
 return true;
}

void PureMediaServer::newconexion()
    {
    ui.textEdit->appendPlainText("Conectado a Pure Data");
    qDebug() << "Connected to PD";
    }

void PureMediaServer::on_window_stateChanged(int state)
{
    qDebug() << "state" << state;
    if ((state == 2)) {
       ui.textEdit->appendPlainText("Creando Ventana");
       QString desc("0001 0001;");
       if (!sendPacket(desc.toAscii().constData(),desc.size()))
       {
        ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
       }
    }
    if ((state == 0)) {
          ui.textEdit->appendPlainText("Destruyendo Ventana");
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
    QString desc = tr("0003 %1;").arg(x);
    if (!sendPacket(desc.toAscii().constData(),desc.size()))
            {
             ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
            }

}

void PureMediaServer::on_winsizex_valueChanged()
{
    int x = ui.winsizex->value();
    QString desc = tr("0004 %1;").arg(x);
    if (!sendPacket(desc.toAscii().constData(),desc.size()))
            {
             ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
            }

}

void PureMediaServer::on_winsizey_valueChanged()
{
    int x = ui.winsizey->value();
    QString desc = tr("0005 %1;").arg(x);
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
        return;
    }
    if ((state == 2))
    {
        int x = ui.universe->value();
        QString desc = tr("0020 %1;").arg(x);
        if (!sendPacket(desc.toAscii().constData(),desc.size()))
                {
                 ui.textEdit->appendPlainText("No puedo mandar mensaje a Pure Data");
                }
        return;
    }
 }
