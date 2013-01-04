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

#include "PacketCreator.h"
#include "PureMediaServer.h"
#include "MediaServer.h"
#include "CITPDefines.h"
#include "MSEXDefines.h"
#include "ui_PureMediaServer.h"

#include <QtDebug>
#include <QFileInfo>
#include <QFile>
#include <QDir>

PacketCreator::PacketCreator(QObject *parent)
  : QObject(parent)
{
}

PacketCreator::~PacketCreator()
{
}

unsigned char * PacketCreator::createPLocPacket(
						const QString &name, 
						const QString &state, 
						int &bufferLen)
{
  qDebug() << "createPLOC" << name <<  state;
  QString plocType = "MediaServer";
  // figure out the packet size, all strings need to be NULL terminated
  bufferLen = sizeof(struct CITP_PINF_PLoc) + plocType.size() + 1 + name.size() + 1 + state.size() + 1;
  unsigned char *buffer = new unsigned char[bufferLen];
  memset(buffer, 0, bufferLen);
  CITP_PINF_PLoc *packet = (CITP_PINF_PLoc*)buffer;

  // CITP header
  packet->CITPPINFHeader.CITPHeader.Cookie = COOKIE_CITP;
  packet->CITPPINFHeader.CITPHeader.VersionMajor = 0x01;
  packet->CITPPINFHeader.CITPHeader.VersionMinor = 0x00;
  packet->CITPPINFHeader.CITPHeader.Reserved[0] = 0x00;
  packet->CITPPINFHeader.CITPHeader.Reserved[1] = 0x00; 
  packet->CITPPINFHeader.CITPHeader.MessageSize = bufferLen;
  packet->CITPPINFHeader.CITPHeader.MessagePartCount = 0x01;
  packet->CITPPINFHeader.CITPHeader.MessagePart = 0x00; // XXX - doc says 0-based?
  packet->CITPPINFHeader.CITPHeader.ContentType = COOKIE_PINF;

  // PINF header
  packet->CITPPINFHeader.ContentType = COOKIE_PINF_PLOC;

  // PLoc data
  packet->ListeningTCPPort = LISTENTCPPORT;
  // type
  int offset = sizeof(struct CITP_PINF_PLoc);
  memcpy(buffer + offset, plocType.toAscii().constData(), plocType.size());
  
  // name
  offset += plocType.size() + 1;
  memcpy(buffer + offset, name.toAscii().constData(), name.size());

  // state
  offset += name.size() + 1;
  memcpy(buffer + offset, state.toAscii().constData(), state.size());

  return buffer;
}

// Paquetes MSEX

unsigned char * PacketCreator::createSINFPacket(int &bufferLen)
{
  // figure out the packet size
  QString name = NAME;
  bufferLen = 2*name.size() + 3 + sizeof(struct CITP_MSEX_10_SINF);
  // create the buffer
  unsigned char *buffer = new unsigned char[bufferLen];
  memset(buffer, 0, bufferLen);
  CITP_MSEX_10_SINF *packet = (CITP_MSEX_10_SINF *)buffer;

  // CITP header
  packet->CITPMSEXHeader.CITPHeader.Cookie = COOKIE_CITP;
  packet->CITPMSEXHeader.CITPHeader.VersionMajor = 0x01;
  packet->CITPMSEXHeader.CITPHeader.VersionMinor = 0x00;
  packet->CITPMSEXHeader.CITPHeader.Reserved[0] = 0x00;
  packet->CITPMSEXHeader.CITPHeader.Reserved[1] = 0x00;
  packet->CITPMSEXHeader.CITPHeader.MessageSize = bufferLen;
  packet->CITPMSEXHeader.CITPHeader.MessagePartCount = 0x01;
  packet->CITPMSEXHeader.CITPHeader.MessagePart = 0x00; // XXX - doc says 0-based?
  packet->CITPMSEXHeader.CITPHeader.ContentType = COOKIE_MSEX;

  // MSEX header
  packet->CITPMSEXHeader.VersionMajor = 0x01;
  packet->CITPMSEXHeader.VersionMinor = 0x00;
  packet->CITPMSEXHeader.ContentType = COOKIE_MSEX_SINF;

  // SInf content
  int offset = sizeof(struct CITP_MSEX_Header);
  const ushort * namei = name.utf16();
  memcpy((buffer+offset), namei, 2*(name.size()));
  offset =  offset + 2*name.size() +2;
  memset ((buffer+offset), 0x01,1);
  offset++;
  memset((buffer+offset), 0x00, 3);
  return buffer;
}

unsigned char * PacketCreator::createLSTAPacket(layerList layerp,int &bufferLen)
{
    // figure out the packet size
    const quint8 layerCount = layerp.size();
    int layerslen = 0;
    LayerStatus layeri;
    for (int i=0; i<layerCount; i++) {
        layeri = layerp.at(i);
        layerslen = layerslen + 2*layeri.Name.size() -4 + sizeof(struct LayerStatus);
    }
    int offset = sizeof(struct CITP_MSEX_10_LSta);
    bufferLen = offset + layerslen;
    unsigned char *buffer = new unsigned char[bufferLen];
    memset(buffer, 0, bufferLen);
    CITP_MSEX_10_LSta *packet;
    packet = (CITP_MSEX_10_LSta *)buffer;
    // CITP header
    packet->CITPMSEXHeader.CITPHeader.Cookie = COOKIE_CITP;
    packet->CITPMSEXHeader.CITPHeader.VersionMajor = 0x01;
    packet->CITPMSEXHeader.CITPHeader.VersionMinor = 0x00;
    packet->CITPMSEXHeader.CITPHeader.Reserved[0] = 0x00;
    packet->CITPMSEXHeader.CITPHeader.Reserved[1] = 0x00;
    packet->CITPMSEXHeader.CITPHeader.MessageSize = bufferLen;
    packet->CITPMSEXHeader.CITPHeader.MessagePartCount = 0x01;
    packet->CITPMSEXHeader.CITPHeader.MessagePart = 0x00;
    packet->CITPMSEXHeader.CITPHeader.ContentType = COOKIE_MSEX;

    // MSEX header
    packet->CITPMSEXHeader.ContentType = COOKIE_MSEX_LSTA;
    packet->CITPMSEXHeader.VersionMajor = 0x01;
    packet->CITPMSEXHeader.VersionMinor = 0x00;

    //LSTA
    packet->LayerCount = layerCount;

    // Convertimos las estructuras a unsigned char
    unsigned char * buffer2 = new unsigned char[layerslen];
    LayerStatus packet2;
    for (int i=0; i<layerCount;i++)
    {
        int offset2 =0;
        memset(buffer2, 0x00, layerslen);
        packet2 = layerp.at(i);
        memcpy(buffer2, &packet2.LayerNumber,1);
        offset2++;
        memcpy((buffer2+offset2), &packet2.PhysicalOutput,1);
        offset2++;
        memcpy((buffer2+offset2), &packet2.MediaLibraryNumber,1);
        offset2++;
        memcpy((buffer2+offset2), &packet2.MediaNumber,1);
        offset2++;
        const ushort * name = packet2.Name.utf16();
        memcpy((buffer2+offset2), name, 2*packet2.Name.size());
        offset2 = offset2 + 2*packet2.Name.size() +2;
        memcpy((buffer2+offset2), &packet2.MediaPosition,4);
        offset2 = offset2 + 4;
        memcpy((buffer2+offset2), &packet2.MediaLength,4);
        offset2 = offset2 + 4;
        memcpy((buffer2+offset2), &packet2.MediaFPS,1);
        offset2 = offset2 + 1;
        memcpy((buffer2+offset2), &packet2.LayerStatusFlags,4);
        offset2 = offset2 + 4;
        memcpy((buffer+offset), buffer2, offset2);
        offset = offset + offset2;
    }
    return buffer;
}

unsigned char * PacketCreator::createELINPacket(QList <MediaLibrary> medialib, int&bufferLen)
{
    // Vemos el tamaño total de la estructurta
    MediaLibrary mediai;
    int medialen = 0;
    int size = 0;
    int i;
    for (i=0; i<medialib.size(); i++)
    {
        mediai = medialib.at(i);
        size = sizeof(mediai) + 2*mediai.m_Name.size() +2 -sizeof(mediai.m_Name) -sizeof(mediai.m_MediaInformation);
        medialen = medialen + size;
    }
    int offset = sizeof(struct CITP_MSEX_ELIn);
    bufferLen = offset + medialen;
    unsigned char *buffer = new unsigned char[bufferLen];
    memset(buffer, 0, bufferLen);
    CITP_MSEX_ELIn *packet = (CITP_MSEX_ELIn *)buffer;

    // CITP header
    packet->CITPMSEXHeader.CITPHeader.Cookie = COOKIE_CITP;
    packet->CITPMSEXHeader.CITPHeader.VersionMajor = 0x01;
    packet->CITPMSEXHeader.CITPHeader.VersionMinor = 0x00;
    packet->CITPMSEXHeader.CITPHeader.Reserved[0] = 0x00;
    packet->CITPMSEXHeader.CITPHeader.Reserved[1] = 0x00;
    packet->CITPMSEXHeader.CITPHeader.MessageSize = bufferLen;
    packet->CITPMSEXHeader.CITPHeader.MessagePartCount = 0x01;
    packet->CITPMSEXHeader.CITPHeader.MessagePart = 0x00; // XXX - doc says 0-based?
    packet->CITPMSEXHeader.CITPHeader.ContentType = COOKIE_MSEX;

    // MSEX header
    packet->CITPMSEXHeader.ContentType = COOKIE_MSEX_ELIN;
    packet->CITPMSEXHeader.VersionMajor = 0x01;
    packet->CITPMSEXHeader.VersionMinor = 0x00;

    // ELin Content
    packet->LibraryType = 0x01;
    packet->LibraryCount = medialib.size();
    // Element Library Information
   unsigned char *buffer2 = new unsigned char[medialen];
   for (i=0; i<medialib.size(); i++)
    {
        memset(buffer2, 0, medialen);
        int offset2 = 0;
        mediai = medialib.at(i);
        memcpy (buffer2, &mediai.m_Id, 1);
        offset2++;
        memcpy ((buffer2+offset2), &mediai.m_DMXRangeMin,1);
        offset2++;
        memcpy((buffer2 +offset2), &mediai.m_DMXRangeMax,1);
        offset2++;
        const ushort * name = mediai.m_Name.utf16();
        memcpy((buffer2+offset2), name, (2*mediai.m_Name.size()));
        offset2 = 2+ offset2 + 2*mediai.m_Name.size();
        memcpy((buffer2+offset2), &mediai.m_ElementCount, 1);
        offset2++;
        memcpy((buffer+offset), buffer2, offset2);
        offset = offset + offset2;
    }
    return buffer;
}

unsigned char * PacketCreator::createMEINPacket(MediaLibrary medialib, int &bufferLen)
{
    // Vemos el tamaño total de la estructurta
    MediaLibrary mediai = medialib;
    QList<MediaInformation> mediainflist = mediai.m_MediaInformation;
    MediaInformation mediainf;
    int medialen = 0;
    int size = 0;
    int i;
    for (i=0; i<mediainflist.size(); i++)
    {
        mediainf = mediainflist.at(i);
        size = 2 + sizeof(mediainf) +2*mediainf.MediaName.size() - sizeof(mediainf.MediaName);
        medialen = medialen + size;
    }

    int offset = sizeof(struct CITP_MSEX_10_MEIn);
    bufferLen = offset + medialen;
    unsigned char *buffer = new unsigned char[bufferLen];
    memset(buffer, 0, bufferLen);
    CITP_MSEX_10_MEIn *packet = (CITP_MSEX_10_MEIn *)buffer;

    // CITP header
    packet->CITPMSEXHeader.CITPHeader.Cookie = COOKIE_CITP;
    packet->CITPMSEXHeader.CITPHeader.VersionMajor = 0x01;
    packet->CITPMSEXHeader.CITPHeader.VersionMinor = 0x00;
    packet->CITPMSEXHeader.CITPHeader.Reserved[0] = 0x00;
    packet->CITPMSEXHeader.CITPHeader.Reserved[1] = 0x00;
    packet->CITPMSEXHeader.CITPHeader.MessageSize = bufferLen;
    packet->CITPMSEXHeader.CITPHeader.MessagePartCount = 0x01;
    packet->CITPMSEXHeader.CITPHeader.MessagePart = 0x00; // XXX - doc says 0-based?
    packet->CITPMSEXHeader.CITPHeader.ContentType = COOKIE_MSEX;

    // MSEX header
    packet->CITPMSEXHeader.ContentType = COOKIE_MSEX_MEIN;
    packet->CITPMSEXHeader.VersionMajor = 0x01;
    packet->CITPMSEXHeader.VersionMinor = 0x00;

    // MEIn Header
    packet->LibraryId = mediai.m_Id;
    packet->ElementCount = mediainflist.size();
    // MEIn Data
    unsigned char * buffer2 = new unsigned char[medialen];
    for (i=0; i<mediainflist.size();i++){
        memset(buffer2, 0,medialen);
        int offset2 = 0;
        mediainf = mediainflist.at(i);
        memcpy(buffer2, &mediainf.Number, 1);
        offset2= offset2 + 1;
        memcpy((buffer2 + offset2),&mediainf.DMXRangeMin,1);
        offset2= offset2 + 1;
        memcpy((buffer2+offset2), &mediainf.DMXRangeMax ,1 );
        offset2= offset2 + 1;
        const ushort * name = mediainf.MediaName.utf16();
        memcpy((buffer2+offset2), name, 2*mediainf.MediaName.size());
        offset2 = 2 + offset2 + 2*mediainf.MediaName.size();
        memcpy ((buffer2 + offset2), &mediainf.MediaVersionTimestamp, 8);
        offset2= offset2 + 8;
        memcpy((buffer2+offset2), &mediainf.MediaWidth,2);
        offset2= offset2 + 2;
        memcpy((buffer2+offset2), &mediainf.MediaHeight,2);
        offset2= offset2 + 2;
        memcpy((buffer2+offset2), &mediainf.MediaLength,4);
        offset2= offset2 + 4;
        memcpy((buffer2+offset2), &mediainf.MediaFPS,1);
        offset2= offset2 + 1;
        memcpy((buffer+offset), buffer2, offset2);
        offset = offset + offset2;
    }
   return buffer;
}

unsigned char * PacketCreator::createETHNPacket(QString path,MediaLibrary medialib, int elementnumber, int &bufferLen)
{
    QDir dir(path);
    QFileInfo fileInfo;
    QFileInfoList filelist;
    if (medialib.m_Name == "image"){
        dir.setFilter(QDir::Files);
        dir.cd("image");
    }
   else {
        dir.cd("video");
        dir.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
        filelist = dir.entryInfoList();
        fileInfo = filelist.at(medialib.m_Id);
        dir.cd(fileInfo.baseName());
        dir.setFilter(QDir::Files);
    }
  dir.cd("thumbs");
  filelist = dir.entryInfoList();
  if (filelist.size() > elementnumber){
    fileInfo = filelist.at(elementnumber);
  }
  else {
      return NULL;
       }
  QFile file(fileInfo.filePath());
  if (!file.open(QIODevice::ReadOnly)){
        qDebug() << "Cannot open the file"<<fileInfo.filePath();
        return NULL;
  }
  unsigned char * bufferthumb = new unsigned char[file.size()];
  memset(bufferthumb, 0, file.size());
  bufferthumb = file.map(0x00, file.size());
  if (bufferthumb == 0){
      qDebug() << "Cannot map the file";
  }
  bufferLen = sizeof(struct CITP_MSEX_10_ETHN) + file.size();
  unsigned char * buffer = new unsigned char[bufferLen];
  memset(buffer, 0, bufferLen);
  CITP_MSEX_10_ETHN *packet = (CITP_MSEX_10_ETHN *)buffer;
  // CITP header
    packet->CITPMSEXHeader.CITPHeader.Cookie = COOKIE_CITP;
    packet->CITPMSEXHeader.CITPHeader.VersionMajor = 0x01;
    packet->CITPMSEXHeader.CITPHeader.VersionMinor = 0x00;
    packet->CITPMSEXHeader.CITPHeader.Reserved[0] = 0x00;
    packet->CITPMSEXHeader.CITPHeader.Reserved[1] = 0x00;
    packet->CITPMSEXHeader.CITPHeader.MessageSize = bufferLen;
    packet->CITPMSEXHeader.CITPHeader.MessagePartCount = 0x01;
    packet->CITPMSEXHeader.CITPHeader.MessagePart = 0x00; // XXX - doc says 0-based?
    packet->CITPMSEXHeader.CITPHeader.ContentType = COOKIE_MSEX;
    // Mandamos el paquete

    // MSEX header
    packet->CITPMSEXHeader.ContentType = COOKIE_MSEX_ETHN;
    packet->CITPMSEXHeader.VersionMajor = 0x01;
    packet->CITPMSEXHeader.VersionMinor = 0x00;

    //ETHN Content
    packet->LibraryType = 0x01;
    packet->LibraryId = medialib.m_Id;
    packet->ElementNumber = elementnumber;
    packet->ThumbnailFormat = 0x4745504A; //  JPEG
    packet->ThumbnailWidth = 60;
    packet->ThumbnailHeight = 46;
    packet->ThumbnailBufferSize = file.size();

    // Thumbnail
    int offset = sizeof(struct CITP_MSEX_10_ETHN);
    memcpy((buffer+offset), bufferthumb, file.size());
    file.close();
    return buffer;
}

unsigned char * PacketCreator::createNACKPacket(quint32 cookie, int &bufferLen)
{
    bufferLen = sizeof(struct CITP_MSEX_Nack);
    unsigned char *buffer = new unsigned char[bufferLen];
    memset(buffer, 0, bufferLen);
    CITP_MSEX_Nack *packet;
    packet = (CITP_MSEX_Nack *)buffer;
    packet->ReceivedContentType = cookie;
    return buffer;
}

// Video Stream

const char * PacketCreator::createVSRCPacket(int &bufferLen)
{
    QString name("Output 1");
    bufferLen = sizeof(struct CITP_MSEX_VSRC) + 2*name.size() + 10;
    // create the buffer
    char *buffer = new char[bufferLen];
    memset(buffer, 0, bufferLen);
    CITP_MSEX_VSRC *packet = (CITP_MSEX_VSRC *)buffer;

    // CITP header
    packet->CITPMSEXHeader.CITPHeader.Cookie = COOKIE_CITP;
    packet->CITPMSEXHeader.CITPHeader.VersionMajor = 0x01;
    packet->CITPMSEXHeader.CITPHeader.VersionMinor = 0x00;
    packet->CITPMSEXHeader.CITPHeader.Reserved[0] = 0x00;
    packet->CITPMSEXHeader.CITPHeader.Reserved[1] = 0x00;
    packet->CITPMSEXHeader.CITPHeader.MessageSize = bufferLen;
    packet->CITPMSEXHeader.CITPHeader.MessagePartCount = 0x01;
    packet->CITPMSEXHeader.CITPHeader.MessagePart = 0x00; // XXX - doc says 0-based?
    packet->CITPMSEXHeader.CITPHeader.ContentType = COOKIE_MSEX;
    // MSEX header
    packet->CITPMSEXHeader.ContentType = COOKIE_MSEX_VSRC;
    packet->CITPMSEXHeader.VersionMajor = 0x01;
    packet->CITPMSEXHeader.VersionMinor = 0x00;
    packet->SourceCount = 0x01;
    // VSRC Information
    packet->SourceIdentifier = 0x01;
    int offset = sizeof(struct CITP_MSEX_VSRC);
    const ushort * namei = name.utf16();
    memcpy((buffer+offset), namei, 2*(name.size()));
    offset =  offset + 2*name.size() +2;
    QByteArray post;
    post[0] = 0xFF;          // Physical Ouput uint8
    post[1] = 0xFF;          // Layer Number uint8
    post[2] = 0x00;          // Flags uint16
    post[3] = 0x00;
    post[4] = 0x00;          // Width uint16
    post[5] = 0x40;
    post[6] = 0x00;          // Height uint16
    post[7] = 0x2E;
    memcpy((buffer+offset), post, post.size());
    return buffer;
}

const char * PacketCreator::createFrame(uchar *frame, int &bufferLen)
{
    int bufferLenTot = sizeof(struct CITP_MSEX_10_StFr ) + bufferLen;
    char * buffer = new char[bufferLen];
    memset(buffer, 0, bufferLen);
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
    memcpy((buffer+offset), frame, bufferLen);
    bufferLen = bufferLenTot;
    return buffer;
}
