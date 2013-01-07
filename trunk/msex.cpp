/*
   Pure Media Server - A Media Server Sotfware for stage and performing
   Copyright (C) 2012-2013  Santiago Noreña puremediaserver@gmail.com

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

#include "msex.h"
//#include "libremediaserver.h"
#include "MSEXDefines.h"
#include "PacketCreator.h"
#include "citp-lib.h"

#include <QtDebug>
#include <QTimer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDir>
#include <QDateTime>

msex::msex(QObject *parent)
   : QObject(parent),
    m_peerState(STATE),
    m_peerName(NAME),
    m_listeningPort(LISTENTCPPORT)

{
   // Iniciamos la lista de estatus de layer
   LayerStatus layermodel;
   m_layers.clear();
   int i;
    for (i=0; i < LAYER_NUMBER; i++)
    {
    layermodel.LayerNumber = i;
    layermodel.PhysicalOutput = 0x01;
    layermodel.MediaLibraryNumber = 0x00;
    layermodel.MediaNumber = i;
    layermodel.Name = "medianame";
    layermodel.MediaPosition = 0x1111;
    layermodel.MediaLength = 0x2222;
    layermodel.MediaFPS = 25;
    m_layers.append(layermodel);
    }
  // Variables para el buffer de LSTA
  m_bufferLen = 0x00;
  m_buffer = NULL;
  m_pathmedia.clear();
 // Inicio del TCP Server
  m_tcpServer = new QTcpServer(this);
  Q_CHECK_PTR(m_tcpServer);
  if (!m_tcpServer)
      qWarning("error TCP Server no creado");
  if (!m_tcpServer->listen(QHostAddress::Any, LISTENTCPPORT))
      {
      qWarning("error creando tcpServer");
      }
  connect(m_tcpServer, SIGNAL(newConnection()),
         this, SLOT(newPeer()));
  // Timer for frame preview
  n_timer = new QTimer(this);
  Q_CHECK_PTR(n_timer);
}

// Propiedades de clase

msex::~msex()
{
   if (m_tcpServer)
     m_tcpServer->close();

}
/*
QString msex::peerName() const
{
  return m_peerName;
}

QString msex::peerState() const
{
  return m_peerState;
}

QString msex::peerHost() const
{
  return m_host.toString();
}

quint16 msex::peerListeningPort() const
{
  return m_listeningPort;
}
*/
bool msex::newPeer()
{
    m_tcpSocket = m_tcpServer->nextPendingConnection();
    connect(m_tcpSocket, SIGNAL(readyRead()),
              this, SLOT(handleReadyRead()));
    // Ahora tenemos que mandar un Sinf para MSEX 1.1 y 1.0
    //Creamos un paquete SINf
     int bufferLen = sizeof(struct CITP_MSEX_10_SINF);
     unsigned char * buffer = PacketCreator::createSINFPacket(bufferLen);
     if (!buffer)
     {
       qDebug() << "msex::new peer:createSINFPacket() failed";
       return false;
     }
     // Mandamos el paquete
     if (!msex::sendPacket(buffer, bufferLen))
         {
         qDebug() << "msex::new peer: Send SInf Message failed";
         return false;
         }
     qDebug() << "msex::new peer: SInf Sent...";
     return true;
    /* Iniciamos el layer Status
     // Creamos un  paquete LSTA
      m_bufferLen = sizeof(struct CITP_MSEX_12_LSta);
      unsigned char *buffer2 = PacketCreator::createLSTAPacket(m_layers, m_bufferLen);
      if (!buffer2)
      {
        qDebug() << "createLSTAPacket() failed";
        return false;
      }
      m_buffer = buffer2;
      // Iniciamos el temporizador para mandar LSTA
    //  n_timer->start();
    //    qDebug("Start sending LSta");
       transmitlsta();
   return true;*/
}
/*
bool msex::transmitlsta()
{
   if (!msex::sendPacket(m_buffer, m_bufferLen))
        {
        qDebug() << "msex::LSta Message failed";
        qDebug() << "Peer disconnected?";
        n_timer->stop();
        m_tcpSocket->close();
        return false;
        }
    return true;
}*/

// Lectura de paquetes

void msex::handleReadyRead()
{
 while(m_tcpSocket->bytesAvailable())
       {
       QByteArray peekArray = m_tcpSocket->peek(12);
       if (12 != peekArray.size())
           {
           return;
            }
       if (peekArray[0] != 'C' &&
           peekArray[1] != 'I' &&
           peekArray[2] != 'T' &&
           peekArray[3] != 'P')
           {
           // remove the first byte and try again
           QByteArray b = m_tcpSocket->read(1);
           qDebug() << "Discarding byte:" << b[0];
           continue;
           }
       quint32 messageSize = 0;
       messageSize = peekArray[11];
       messageSize <<= 8;
       messageSize |= peekArray[10];
       messageSize <<= 8;
       messageSize |= peekArray[9];
       messageSize <<= 8;
       messageSize |= peekArray[8];
       if (messageSize > m_tcpSocket->bytesAvailable())
             {
             qDebug() << "Not enough bytes available, only have:" << m_tcpSocket->bytesAvailable();
             return;
              }
       QByteArray byteArray = m_tcpSocket->read(messageSize);
       parsePacket(byteArray);
                 }
}


// Mandar paquetes

bool msex::sendPacket(const unsigned char *buffer, int bufferLen)
{
  if (!m_tcpSocket)
    {
      return false;
    }
  if (QAbstractSocket::ConnectedState != m_tcpSocket->state())
    {
      qDebug() << "msex::sendPacket() - Socket not connected";
      return false;
    }
  if (bufferLen != m_tcpSocket->write((const char*)buffer, bufferLen))
    {
      qDebug() << "msex::sendPacket() write failed:" << m_tcpSocket->error();
      return false;
    }
  delete buffer;
  return true;
}

// Overload Send Packet

bool msex::sendPacket(const char *buffer, int bufferLen)
{
  if (!m_tcpSocket)
    {
      return false;
    }
  if (QAbstractSocket::ConnectedState != m_tcpSocket->state())
    {
      qDebug() << "msex::sendPacket() - Socket not connected";
      return false;
    }
  if (bufferLen != m_tcpSocket->write(buffer, bufferLen))
    {
      qDebug() << "msex::sendPacket() write failed:" << m_tcpSocket->error();
      return false;
    }
  return true;
}

// Determina qué tipo de paquete es
void msex::parsePacket(const QByteArray &byteArray)
{
  const char *data = byteArray.constData();
  struct CITP_Header *citpHeader = (struct CITP_Header*)data;

  // CITP header
  if (citpHeader->Cookie != COOKIE_CITP)
    {
      qDebug() << "parsePacket: invalid Cookie" << citpHeader->Cookie;
      return;
    }
/*
  if (citpHeader->VersionMajor != 0x01)
    {
      qDebug() << "parsePacket: invalid VersionMajor:" << citpHeader->VersionMajor;
      return;
    }

   if (citpHeader->VersionMinor != 0x00)
    {
      qDebug() << "parsePacket: invalid VersionMinor:" << citpHeader->VersionMinor;
      return;
    }
  */
  switch (citpHeader->ContentType)
    {
    case COOKIE_PINF:
      qDebug() << "parsePacket: got COOKIE_PINF packet on TCP Socket...";
      parsePINFPacket(byteArray);
      break;
    case COOKIE_MSEX:
      parseMSEXPacket(byteArray);
      break;
    default:
      qDebug() << "parsePacket: unknown ContentType:" << citpHeader->ContentType;
      return;
    }
}

//Process PINF Packets
void msex::parsePINFPacket(const QByteArray &byteArray)
{
    const char *data = byteArray.constData();
    struct CITP_PINF_Header *pinfHeader = (struct CITP_PINF_Header*)data;
    switch (pinfHeader->ContentType)
    {
        case COOKIE_PINF_PNAM:
        {
        int offset = sizeof(struct CITP_PINF_Header);
        int size = byteArray.size();
        char * name = new char[size];
        memcpy(name,(data+offset),size);
        QString namei = name;
        qDebug() << "Ploc arrives. NAME:" << namei;
        }
    }
}

// Procesa paquete MSEX
void msex::parseMSEXPacket(const QByteArray &byteArray)
{
  const char *data = byteArray.constData();
  struct CITP_MSEX_Header *msexHeader = (struct CITP_MSEX_Header*)data;
  switch (msexHeader->ContentType)
  {
  case COOKIE_MSEX_CINF:
      parseCINFPacket(byteArray);
      break;
/* case COOKIE_MSEX_NACK:
      parseNACKPacket(byteArray);
      break;*/
 case COOKIE_MSEX_GELI:
      parseGELIPacket(byteArray);
      break;
 case COOKIE_MSEX_GEIN:
      parseGEINPacket(byteArray);
      break;
  case COOKIE_MSEX_GELT:
      parseGELTPacket();
      break;
  case COOKIE_MSEX_GETH:
      parseGETHPacket(byteArray);
      break;
  case COOKIE_MSEX_GVSR:
      parseGVSRPacket();
      break;
  case COOKIE_MSEX_RQST:
      parseRQSTPacket(byteArray);
      break;
  default:
      qDebug() << "parseMSEXPacket: unknown ContentType:" << msexHeader->ContentType ;
      msex::sendNACK(msexHeader->ContentType);
  }
}

void msex::parseCINFPacket(const QByteArray &byteArray)
{
    const char *data = byteArray.constData();
    struct CITP_MSEX_CInf *cinf = (struct CITP_MSEX_CInf*)data;
    int size = 2 * cinf->SupportedMSEXVersionsCount;
    unsigned short * versions = new unsigned short[size];
    memcpy(versions, &cinf->SupportedMSEXVersions, size);
    for (int i=0; i < cinf->SupportedMSEXVersionsCount;i++){
    qDebug() << "CInf arrives. Suported Versions: " <<versions[i];
    }
}

void msex::parseGELIPacket(const QByteArray &byteArray)
{
  const char *data = byteArray.constData();
  struct CITP_MSEX_11_GELI *geliPacket = (struct CITP_MSEX_11_GELI*)data;
  if (geliPacket->LibraryType != 0x01)
    {
      qDebug() << "parseGELIPacket:Not library type supported, only MEDIA now 0x01";
      return;
  }
  // Creamos un paquete ELin
  int bufferLen = sizeof(struct CITP_MSEX_ELIn);
  unsigned char * buffer = PacketCreator::createELINPacket(m_media, bufferLen);
  if (!buffer)
  {
    qDebug() << "parseGELIPacket:createELINPacket() failed";
    return;
  }

  // Mandamos el paquete
  if (!msex::sendPacket(buffer, bufferLen))
      {
      qDebug() << "parseGELIPacket: Send ELin Message failed";
      return;
      }
  qDebug() << "parseGELIPacket finish ok. ELin Sent...";
}

void msex::parseGEINPacket(const QByteArray &byteArray)
{
  const char *data = byteArray.constData();
  struct CITP_MSEX_10_GEIn *geinPacket = (struct CITP_MSEX_10_GEIn*)data;
  qDebug() << "GEIn arrives: Elemet count:" << geinPacket->ElementCount << "Libray Id:" << geinPacket->LibraryId << "Library Type:" << geinPacket->LibraryType;
  int bufferLen = sizeof (struct CITP_MSEX_10_MEIn);
  if (!(geinPacket->LibraryId < m_media.size())) {
          qDebug() << "Library ID exceeds size list";
          /*unsigned char * buffer = PacketCreator::createMEINPacket(m_media.at(geinPacket->LibraryId),bufferLen);
          if (!buffer)
            {
            qDebug() << "parseGEINPacket:createMEINPacket() failed";
            return;
          }
          // Mandamos el paquete
          if (!msex::sendPacket(buffer, bufferLen))
              {
              qDebug() << "parseGEINPacket: Send MEIn Message failed";
              return;
              }
          qDebug() << "parseGEINPacket finish ok. MEIn Sent...";*/
  }

  else {
          unsigned char * buffer = PacketCreator::createMEINPacket(m_media.at(geinPacket->LibraryId),bufferLen);
          if (!buffer)
            {
            qDebug() << "parseGEINPacket:createMEINPacket() failed";
            return;
          }
      // Mandamos el paquete
        if (!msex::sendPacket(buffer, bufferLen))
        {
            qDebug() << "parseGEINPacket: Send MEIn Message failed";
            return;
        }
 }
}

void msex::parseGELTPacket()
{
//  const char *data = byteArray.constData();
//  struct CITP_MSEX_12_GELT *geltPacket = (struct CITP_MSEX_12_GELT*)data;
    qDebug() << "parseGELTPacket: GELT arrives...";
}

void msex::parseGETHPacket(const QByteArray &byteArray)
{
  const char *data = byteArray.constData();
  struct CITP_MSEX_10_GETh *gethPacket = (struct CITP_MSEX_10_GETh*)data;
  qDebug() << "GETh arrives. Format:"<< gethPacket->ThumbnailFormat<<"Width:"<<gethPacket->ThumbnailWidth<<"Height:"<<gethPacket->ThumbnailHeight;
  qDebug() << "Library Number:"<<gethPacket->LibraryNumber<<"Element COunt:"<<gethPacket->ElementCount<<"ElementNumber:"<<gethPacket->ElementNumber;
  int bufferLen = sizeof (struct CITP_MSEX_10_ETHN);
  if (!(gethPacket->LibraryNumber < m_media.size())) {
          qDebug() << "Library ID exceeds size list";
          return;
  }
  MediaLibrary mediai = m_media.at(gethPacket->LibraryNumber);
  if (mediai.m_ElementCount <= gethPacket->ElementNumber){
      qDebug() <<  "Element Number is greater than the elements in library";
      return;
  }
  unsigned char * buffer = PacketCreator::createETHNPacket(m_pathmedia, mediai, gethPacket->ElementNumber, bufferLen);
  if (!buffer)
  {
    qDebug() << "parseGETHPacket:createETHNPacket() failed";
    return;
  }
  // Mandamos el paquete
  if (!msex::sendPacket(buffer, bufferLen))
      {
      qDebug() << "parseGETHPacket: Send ETHN Message failed";
      return;
      }  
}

void msex::parseGVSRPacket()
{
  int bufferLen;
  // Create a VSrc message
  const char * buffer = PacketCreator::createVSRCPacket(bufferLen);
  if (!buffer)
  {
    qDebug() << "parseGVSRacket:createVSRCPacket() failed";
    return;
  }
  // Mandamos el paquete
  if (!msex::sendPacket(buffer, bufferLen))
      {
      qDebug() << "parseGVSRPacket: Send VRSC Message failed";
      return;
      }
    delete buffer;
}

void msex::parseRQSTPacket(const QByteArray &byteArray)
{
    const char *data = byteArray.constData();
    struct CITP_MSEX_RqSt *Packet = (struct CITP_MSEX_RqSt*)data;
    // Here we need start the streaming MagicQ 88x64 RGB8 1 FPS
    // Iniciamos el timer
    if (!n_timer->isActive()) {
            n_timer->setInterval(1000 * Packet->fps);
            n_timer->start();
            connect(n_timer, SIGNAL(timeout()), this, SLOT(sendFrame()));
    }
}

bool msex::sendNACK(quint32 header)
{
    int bufferLen;
    unsigned char *buffer = PacketCreator::createNACKPacket(header, bufferLen);
    if (!buffer)
      {
        qDebug() << "createNACKPacket() failed";
        return false;
      }

    if (!sendPacket(buffer, bufferLen))
      {
        qDebug() << "sendNACK failed";
        return false;
      }
  return true;
}

// Fin de mensajes MSEX

bool msex::updatemedia()
{
    qDebug()<<"Actualizando biblioteca de medias en " << m_pathmedia;
    QDir dir;
    dir.cd(m_pathmedia);
    m_media.clear();
    // Miramos cuantas librerías tenemos en /video
    if (!dir.cd("video"))
    {   qWarning("Cannot cd to the video directory");
        return false;
    }
    dir.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QFileInfoList filelist = dir.entryInfoList();
    dir.setFilter(QDir::Files);
    QFileInfo fileInfo;
    struct MediaLibrary mediai;
    int i;
    for (i = 0; i < filelist.size(); ++i) {
        fileInfo = filelist.at(i);
        QString name = fileInfo.baseName();
        dir.cd(fileInfo.baseName());
        mediai.m_Id = i;
        mediai.m_DMXRangeMax = i;
        mediai.m_DMXRangeMin = i;
        mediai.m_Name = name;
//        mediai.m_LibraryCount = 0; msex 1.2
        mediai.m_ElementCount = dir.count();
        mediai.m_MediaInformation = getMediaInformation(dir);
        dir.cdUp();
        m_media.append(mediai);
    }
/*
// Miramos las librerías en /imagenes
    i++;
    dir.cd(m_pathmedia);
    if (!dir.cd("image"))
       {
        qWarning("Can not find images dir");
        return false;
        }
    mediai.m_Id = i;
    mediai.m_DMXRangeMax = i;
    mediai.m_DMXRangeMin = i;
    mediai.m_Name = "Images";
//    mediai.m_LibraryCount = 0; msex 1.2
    mediai.m_ElementCount = dir.count();
    mediai.m_MediaInformation = getMediaInformation(dir);
    dir.cdUp();
    m_media.append(mediai);

// Creamos una vacía
    i++;
    mediai.m_Id = i;
    mediai.m_DMXRangeMax = i;
    mediai.m_DMXRangeMin = i;
    mediai.m_Name = "empty";
//    mediai.m_LibraryCount = 0; msex 1.2
    mediai.m_ElementCount = 0x00;
//    mediai.m_MediaInformation = NULL;
    m_media.append(mediai);
*/
// Lanzar aquí la creación de thumbs?

// Mandamos las señal ELup (Msex 1.2)

return true;
}

QList<MediaInformation> msex::getMediaInformation(QDir dir)
{
    QList<MediaInformation> mediaList;
    MediaInformation mediainf;
    dir.setFilter(QDir::Files);
    QFileInfoList filelist = dir.entryInfoList();
    QFileInfo fileInfo;
    for (int i = 0; i < filelist.size(); ++i) {
        fileInfo = filelist.at(i);
        QDateTime date = fileInfo.created();
        mediainf.Number = i;
        mediainf.DMXRangeMin = i;
        mediainf.DMXRangeMax = i;
        mediainf.MediaName = fileInfo.baseName();
//        mediainf.MediaVersionTimestamp = date.toMSecsSinceEpoch();
        mediainf.MediaWidth = 320;
        mediainf.MediaHeight = 200;
        mediainf.MediaLength = 1000;
        mediainf.MediaFPS = 25;
        mediaList.append(mediainf);
        }
    return mediaList;
}

void msex::setpath(QString path)
{
    //m_pathmedia = NULL;
    m_pathmedia.clear();
    m_pathmedia.append(path);
}

void msex::startCitp(quint32 ipadd)
{
    m_citp = new CITPLib(this);
    Q_CHECK_PTR(m_citp);
    if (!m_citp->createPeerInformationSocket(NAME, STATE, ipadd))
    {
        qDebug()<<("CreatePeerInformationSocket failed");
    }
}

void msex::sendFrame()
{
    emit frameRequest();
}
