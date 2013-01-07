/*
   Libre Media Server - A Media Server Sotfware for stage and performing
   Copyright (C) 2012-2013  Santiago Noreña
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

/* This class do all the MSEX work */

#ifndef MSEX_H
#define MSEX_H

#include <QObject>
#include <QHostAddress>

#include "MSEXDefines.h"

class QTimer;
class QTcpSocket;
class QTcpServer;
class QDir;
class CITPLib;

class msex : public QObject
{
    Q_OBJECT

public:
    msex(QObject *parent);
    virtual ~msex();
//    virtual QString peerName() const;
//    virtual QString peerState() const;
//    virtual QString peerHost() const;
//    virtual quint16 peerListeningPort() const;
//    void previewLayer1();

    bool CreateMSEXSocket();
    bool updatemedia();
    virtual void setpath(QString path); // path to media dir
    void startCitp(quint32 ipadd); // Start the Peer Informatio Socket
    bool sendPacket(const unsigned char *buffer, int bufferLen);

    QTimer *n_timer; // Frame Request
    QList<LayerStatus> m_layers;
    QList<MediaLibrary> m_media;

protected:

    CITPLib *m_citp;    // CITP Peer. PLOc and frame transmit
    QString m_pathmedia;
    unsigned char * m_buffer;
    int m_bufferLen;
    QString m_peerState;
    QString m_peerName;
    QHostAddress m_host;
    quint16 m_listeningPort;
    QTcpServer *m_tcpServer;
    QTcpSocket *m_tcpSocket;

    bool sendPacket(const char *buffer, int bufferLen);
    bool sendNACK(quint32 header);

    // Procesado de paquetes MSEX
    void parsePacket(const QByteArray &byteArray);
    void parsePINFPacket(const QByteArray &byteArray);
    void parseMSEXPacket(const QByteArray &bytearray);
    void parseCINFPacket(const QByteArray &byteArray);
    void parseNACKPacket(const QByteArray &byteArray);
    void parseGELIPacket(const QByteArray &byteArray);
    void parseGEINPacket(const QByteArray &byteArray);
    void parseGLEIPacket(const QByteArray &byteArray);
    void parseGELTPacket();
    void parseGETHPacket(const QByteArray &byteArray);
    void parseGVSRPacket();
    void parseGELNPacket(const QByteArray &byteArray);
    void parseRQSTPacket(const QByteArray &byteArray);

private:

    QList<MediaInformation> getMediaInformation(QDir dir);

public slots:

protected slots:

    void handleReadyRead();

private slots:

    bool newPeer();
    void sendFrame(); // Slot temporizado para mandar una señal a libremediaserver::sendframe() para mandar un frame

signals:
    void frameRequest();
};

#endif // MEDIASERVER_H
