/*
The MIT License

Copyright (c) 2009 John Warwick

Copyright (c) 2012 Santi Noreña

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#include "PeerInformationSocket.h"

#include <QTimer>
#include <QtDebug>

#include "citp-lib.h"

#include "CITPDefines.h"
#include "PacketCreator.h"
#include "MediaServer.h"

#include <QNetworkInterface>

#ifdef Q_OS_WIN
  #include <winsock2.h>
  #include <ws2tcpip.h>
#else
  #include <sys/types.h>
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
#endif



PeerInformationSocket::PeerInformationSocket(QObject *parent)
  : QUdpSocket(parent),
    m_name(tr("Unknown name")),
    m_state(tr("Unknown state")),
    m_packetBuffer(NULL),
    m_packetBufferLen(0)
{
  m_timer = new QTimer(this);
  Q_CHECK_PTR(m_timer);
  connect(m_timer, SIGNAL(timeout()),
	  this, SLOT(transmitPLoc()));
  m_timer->setInterval(TRANSMIT_INTERVAL_MS);
  setSocketState(QUdpSocket::BoundState);
}

PeerInformationSocket::~PeerInformationSocket()
{
  if (m_timer)
    {
      m_timer->stop();
    }
}

bool PeerInformationSocket::init(const QString &name, const QString &state, quint32 ipadd)
{
  if (m_timer)
    {
      m_timer->stop();
    }
  qDebug() << "PeerInformationSocket" << name << state;
  m_name = name;
  m_state = state;


  // create multicast socket, bind to port
  if (!bind(CITP_PINF_MULTICAST_PORT, ShareAddress | ReuseAddressHint))
    {
      qDebug() << "Multicast bind failed";
      return false;
    }

/////////////////////////
/*

  QList<QHostAddress> networklist = networkinterface.allAddresses();
  int k = 0xFFFF;
  int l;
  QHostAddress address;
  for (int i = 0; i < networklist.size(); i++)
  {
      address = networklist.at(i);
      qDebug()<<"Network IP: "<<address.toString();

      int j = address.toIPv4Address();
      qDebug()<<j;
      if (0 << j << 0x7f000002)
      {
          k=j;
          l=i;
          networkinterface =
      }
  }
  address = networklist.at(l);

  networkinterface =
  if (!joinMulticastGroup(CITP_PINF_MULTICAST_IP, iface)
  {
          qDebug() << "Failed joining multicast";
  }
*/
///////////////////////

  qDebug()<<"I have choose " << ipadd;
  struct ip_mreq mreq;
  mreq.imr_multiaddr.s_addr = inet_addr(CITP_PINF_MULTICAST_IP);
  mreq.imr_interface.s_addr = ipadd;
  int r = ::setsockopt(socketDescriptor(), IPPROTO_IP, IP_ADD_MEMBERSHIP,
		       (const char *)&mreq, sizeof(struct ip_mreq));
  if (0 != r)
    {
      qDebug() << "setsockopt failed, r:" << r;
      qDebug("setsockopt failed");
      return false;
    }

  delete m_packetBuffer;
  m_packetBuffer = PacketCreator::createPLocPacket(name, state, m_packetBufferLen);
  if (!m_packetBuffer)
    {
      m_packetBufferLen = 0;
      return false;
    }

//  transmitPLoc();

  // XXX - don't connect this more than once..
  connect(this, SIGNAL(readyRead()),
  	  this, SLOT(handleReadReady()));
  
  if (m_timer)
    {
       m_timer->start();
    }

  return true;
}

void PeerInformationSocket::transmitPLoc()
{
  if (m_packetBuffer && m_packetBufferLen > 0)
    {
      QHostAddress addr(CITP_PINF_MULTICAST_IP);
      qint64 ret = writeDatagram((const char*)m_packetBuffer, m_packetBufferLen, 
				 addr, CITP_PINF_MULTICAST_PORT);
      if (-1 == ret)
        {
          qDebug() << "Failed to send multicast packet:" << error();
        }
    }
}

void PeerInformationSocket::handleReadReady()
{
  while (hasPendingDatagrams()) 
    {
      QByteArray datagram;
      datagram.resize(pendingDatagramSize());
      QHostAddress sender;
      quint16 senderPort;

      if (-1 != readDatagram(datagram.data(), datagram.size(),
			     &sender, &senderPort))
        {
        processPacket(sender, datagram);
        }
    }
}

void PeerInformationSocket::processPacket(const QHostAddress &address, const QByteArray &packetArray)
{
  const char *data = packetArray.constData();
  CITP_PINF_PLoc *packet = (CITP_PINF_PLoc*)data;

  // CITP header
  if (packet->CITPPINFHeader.CITPHeader.Cookie != COOKIE_CITP)
    {
      qDebug() << "No CITP";
      return;
    }
/*
  if (packet->CITPPINFHeader.CITPHeader.VersionMajor != 0x01)
    {
      qDebug() << "Invalid VersionMajor value:" << packet->CITPPINFHeader.CITPHeader.VersionMajor;
      return;
    }

  if (packet->CITPPINFHeader.CITPHeader.VersionMinor != 0x00)
    {
      qDebug() << "Invalid VersionMinor value:" << packet->CITPPINFHeader.CITPHeader.VersionMinor;
      return;
    }
*/
  if (packet->CITPPINFHeader.CITPHeader.ContentType != COOKIE_PINF)
    {
      qDebug() << "NO PINF";
      return;
    }
    // PINF header
//  if (packet->CITPPINFHeader.ContentType = COOKIE_PINF_PLOC)
//       {
//       qDebug() << "PLOC found" << packet->Name;
//       return;
//       }

  // PLoc data
// quint16 listeningPort = packet->ListeningTCPPort;

}

