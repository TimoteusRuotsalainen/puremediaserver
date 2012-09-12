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

#include "MediaServer.h"

#include "MSEXDefines.h"
#include "PacketCreator.h"

#include <QtDebug>
#include <QTimer>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDir>
#include <QDateTime>

MediaServer::MediaServer(QObject *parent)
   : QObject(parent),
    m_peerState(STATE),
    m_peerName(NAME),
//  m_host(host),
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
  m_pathmedia =  NULL;
  // Variables para el buffer de LSTA
  m_bufferLen = 0x00;
  m_buffer = NULL;
  m_tcpServer = new QTcpServer(this);
  Q_CHECK_PTR(m_tcpServer);

 // Inicio del TCP Server; empieza a escuchar
  if (!m_tcpServer)
      qWarning("error TCP Server no creado");
  if (!m_tcpServer->listen(QHostAddress::Any, LISTENTCPPORT))
      {
      qWarning("error creando tcpServer");
      }
  connect(m_tcpServer, SIGNAL(newConnection()),
         this, SLOT(newPeer()));

  // Iniciamos el timer de LSTA
  n_timer = new QTimer(this);
  Q_CHECK_PTR(n_timer);
  n_timer->setInterval(TRANSMIT_INTERVAL_LSTA);

  // Iniciamos las conexiones
  connect(n_timer, SIGNAL(timeout()),this, SLOT(transmitlsta()));
  connect(this, SIGNAL(cinfread()), this, SLOT(cinfprocess()));
}

// Propiedades de clase

MediaServer::~MediaServer()
{
   if (m_tcpServer)
     m_tcpServer->close();

}

QString MediaServer::peerName() const
{
  return m_peerName;
}

QString MediaServer::peerState() const
{
  return m_peerState;
}

QString MediaServer::peerHost() const
{
  return m_host.toString();
}

quint16 MediaServer::peerListeningPort() const
{
  return m_listeningPort;
}

bool MediaServer::newPeer()
{
    qDebug() << "newPeer init";
    m_tcpSocket = m_tcpServer->nextPendingConnection();

    /*
     QHostAddress ipadd;
     quint32 buff2 = 0;
     quint32 buff = 0;

     if (!m_peerList.size())
     {
         ipadd = (m_tcpSocket->localAddress());
         buff2 = ipadd.toIPv4Address();
         m_peerList.append(buff2);
      }
     ipadd = m_tcpSocket->peerAddress();
     buff2 = ipadd.toIPv4Address();
     int i;
     for(i = 0; i< m_peerList.size(); i++)
     {
         buff = m_peerList.at(i);
         if (buff == buff2)
         {
             qDebug() << "TCP Socket ya creado para ese cliente";
             return false;
         }
      }
     m_peerList.append(buff2);*/
     connect(m_tcpSocket, SIGNAL(readyRead()),
              this, SLOT(handleReadyRead()));
     return true;
}

// Lectura de paquetes

void MediaServer::handleReadyRead()
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
//       qDebug() << "Found message size:" << messageSize;
       if (messageSize > m_tcpSocket->bytesAvailable())
             {
             qDebug() << "Not enough bytes available, only have:" << m_tcpSocket->bytesAvailable();
             return;
              }
       QByteArray byteArray = m_tcpSocket->read(messageSize);
//       qDebug() << "Read Data:" << byteArray[0] << byteArray[1] << byteArray[2] << byteArray[3];
//       qDebug() << "byteArray size:" << byteArray.size();
       parsePacket(byteArray);
                 }
}

// Gestión LSta y SInf

bool MediaServer::cinfprocess()
{
   //Creamos un paquete SINf
    int bufferLen = sizeof(struct CITP_MSEX_10_SINF);
    unsigned char * buffer = PacketCreator::createSINFPacket(bufferLen);
    if (!buffer)
    {
      qDebug() << "parseCINFPacket:createSINFPacket() failed";
      return false;
    }
    // Mandamos el paquete
    if (!MediaServer::sendPacket(buffer, bufferLen))
        {
        qDebug() << "parseCINFPacket: Send SInf Message failed";
        return false;
        }
    qDebug() << "parseCINFPacket finish ok. SInf Sent...";

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
    return true;
}

bool MediaServer::transmitlsta()
{
   if (!MediaServer::sendPacket(m_buffer, m_bufferLen))
        {
        qDebug() << "MediaServer::LSta Message failed";
        qDebug() << "Peer disconnected?";
        n_timer->stop();
        m_tcpSocket->close();
        return false;
        }
    return true;
}

// Mandar paquetes

bool MediaServer::sendPacket(const unsigned char *buffer, int bufferLen)
{
  if (!m_tcpSocket)
    {
      return false;
    }
  if (QAbstractSocket::ConnectedState != m_tcpSocket->state())
    {
      qDebug() << "MediaServer::sendPacket() - Socket not connected";
      return false;
    }
  if (bufferLen != m_tcpSocket->write((const char*)buffer, bufferLen))
    {
      qDebug() << "MediaServer::sendPacket() write failed:" << m_tcpSocket->error();
      return false;
    }
  return true;
}

// Determina qué tipo de paquete es
void MediaServer::parsePacket(const QByteArray &byteArray)
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

  // XXX - this coming back as 0x01 during patch operations?

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
void MediaServer::parsePINFPacket(const QByteArray &byteArray)
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
void MediaServer::parseMSEXPacket(const QByteArray &byteArray)
{
  const char *data = byteArray.constData();
  struct CITP_MSEX_Header *msexHeader = (struct CITP_MSEX_Header*)data;
  qDebug() << "MSEX Version mayor:"<<msexHeader->VersionMajor<<"."<<msexHeader->VersionMinor<<"Content type:"<<msexHeader->ContentType;
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
      parseGELTPacket(byteArray);
      break;
  case COOKIE_MSEX_GETH:
      parseGETHPacket(byteArray);
      break;
  case COOKIE_MSEX_GVSR:
      parseGVSRPacket(byteArray);
      break;
  case COOKIE_MSEX_RQST:
      parseRQSTPacket(byteArray);
      break;
  default:
      qDebug() << "parseMSEXPacket: unknown ContentType:" << msexHeader->ContentType ;
      MediaServer::sendNACK(msexHeader->ContentType);
  }
}

void MediaServer::parseCINFPacket(const QByteArray &byteArray)
{
    const char *data = byteArray.constData();
    struct CITP_MSEX_CInf *cinf = (struct CITP_MSEX_CInf*)data;
    int size = 2 * cinf->SupportedMSEXVersionsCount;
    unsigned short * versions = new unsigned short[size];
    memcpy(versions, &cinf->SupportedMSEXVersions, size);
    for (int i=0; i < cinf->SupportedMSEXVersionsCount;i++){
    qDebug() << "CInf arrives. Suported Versions: " <<versions[i];
    }
//    emit cinfread();
}

void MediaServer::parseGELIPacket(const QByteArray &byteArray)
{
  const char *data = byteArray.constData();
  struct CITP_MSEX_11_GELI *geliPacket = (struct CITP_MSEX_11_GELI*)data;
  if (geliPacket->LibraryType != 0x01)
    {
      qDebug() << "Not library type supported, only MEDIA now 0x01";
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
  if (!MediaServer::sendPacket(buffer, bufferLen))
      {
      qDebug() << "parseGELIPacket: Send ELin Message failed";
      return;
      }
  qDebug() << "parseGELIPacket finish ok. ELin Sent...";
}

void MediaServer::parseGEINPacket(const QByteArray &byteArray)
{
  const char *data = byteArray.constData();
  struct CITP_MSEX_10_GEIn *geinPacket = (struct CITP_MSEX_10_GEIn*)data;
  qDebug() << "parseGEINPacket: GEIn arrives...";
  qDebug() << "eLEMENT coUNT:" << geinPacket->ElementCount << "Libray Id:" << geinPacket->LibraryId << "Library Type:" << geinPacket->LibraryType;
  int bufferLen = sizeof (struct CITP_MSEX_10_MEIn);
  if (!(geinPacket->LibraryId < m_media.size())) {
          qDebug() << "Library ID exceeds size list";
          unsigned char * buffer = PacketCreator::createMEINPacket(m_media.last(),bufferLen);
          if (!buffer)
            {
            qDebug() << "parseGEINPacket:createMEINPacket() failed";
            return;
          }
          // Mandamos el paquete
          if (!MediaServer::sendPacket(buffer, bufferLen))
              {
              qDebug() << "parseGEINPacket: Send MEIn Message failed";
              return;
              }
          qDebug() << "parseGEINPacket finish ok. MEIn Sent...";
  }

  else {
          unsigned char * buffer = PacketCreator::createMEINPacket(m_media.at(geinPacket->LibraryId),bufferLen);
          if (!buffer)
            {
            qDebug() << "parseGEINPacket:createMEINPacket() failed";
            return;
          }
      // Mandamos el paquete
        if (!MediaServer::sendPacket(buffer, bufferLen))
        {
            qDebug() << "parseGEINPacket: Send MEIn Message failed";
            return;
        }
 }
 qDebug() << "parseGEINPacket finish ok. MEIn Sent...";
}

void MediaServer::parseGELTPacket(const QByteArray &byteArray)
{
//  const char *data = byteArray.constData();
//  struct CITP_MSEX_12_GELT *geltPacket = (struct CITP_MSEX_12_GELT*)data;
    qDebug() << "parseGELTPacket: GELT arrives...";
//    emit geltread();
}

void MediaServer::parseGETHPacket(const QByteArray &byteArray)
{
  const char *data = byteArray.constData();
  struct CITP_MSEX_10_GETh *gethPacket = (struct CITP_MSEX_10_GETh*)data;
  qDebug() << "parseGEThPacket: GETh arrives...";
  qDebug() << "format:"<< gethPacket->ThumbnailFormat<<"Width:"<<gethPacket->ThumbnailWidth<<"Height:"<<gethPacket->ThumbnailHeight;
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
  unsigned char * buffer = PacketCreator::createETHNPacket(getpath(), mediai, gethPacket->ElementNumber, bufferLen);
  if (!buffer)
  {
    qDebug() << "parseGETHPacket:createETHNPacket() failed";
    return;
  }
  // Mandamos el paquete
  if (!MediaServer::sendPacket(buffer, bufferLen))
      {
      qDebug() << "parseGETHPacket: Send ETHN Message failed";
      return;
      }
  qDebug() << "parseGETHPacket finish ok. ETHN Sent...";
}

void MediaServer::parseGVSRPacket(const QByteArray &byteArray)
{
//  const char *data = byteArray.constData();
//  struct CITP_MSEX_GVSr *gvsrPacket = (struct CITP_MSEX_GVSr*)data;
  qDebug() << "parseGVSRPacket: GVSR arrives...";
//  emit gvsrread();
}

void MediaServer::parseRQSTPacket(const QByteArray &byteArray)
{
//  const char *data = byteArray.constData();
//  struct CITP_MSEX_RqSt *gelnPacket = (struct CITP_MSEX_RqSt*)data;
    qDebug() << "parseRQSTPacket: RQST arrives...";
//    emit rqstread();
}

bool MediaServer::sendNACK(quint32 header)
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
        qDebug() << "Visualizer::sendNACK failed";
        return false;
      }
  return true;
}

// Fin de mensajes MSEX

bool MediaServer::updatemedia()
{
    QDir dir(getpath());
    m_media.clear();
    // Miramos cuantas librerías tenenmos en /video
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

// Miramos las librerías en /imagenes
    i++;
    dir.cd(getpath());
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

// Lanzar aquí la creación de thumbs?

// Mandamos las señal ELup (Msex 1.2)

return true;
}

QList<MediaInformation> MediaServer::getMediaInformation(QDir dir)
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
        mediainf.MediaVersionTimestamp = date.toMSecsSinceEpoch();
        mediainf.MediaWidth = 320;
        mediainf.MediaHeight = 200;
        mediainf.MediaLength = 1000;
        mediainf.MediaFPS = 25;
        mediaList.append(mediainf);
        }
    return mediaList;
}

void MediaServer::setpath(QString path)
{
    m_pathmedia = NULL;
    m_pathmedia = path.toAscii().constData();
}

void MediaServer::setpathu(const char *buffer)
{
  if (!buffer)
    {
      qDebug() << "No buffer to set path";
      return;
    }
    m_pathmedia = buffer;
}

QString MediaServer::getpath()
{
    QString path(m_pathmedia);
    return path;
}
