#ifndef MEDIASERVER_H
#define MEDIASERVER_H

#include <QObject>
#include <QHostAddress>

#include "MSEXDefines.h"

class QTimer;
class QTcpSocket;
class QTcpServer;
class QDir;
class pdInterface;

class MediaServer : public QObject
{
    Q_OBJECT

public:
    MediaServer(QObject *parent);
    virtual ~MediaServer();
    virtual QString peerName() const;
    virtual QString peerState() const;
    virtual QString peerHost() const;
    virtual quint16 peerListeningPort() const;
    bool CreateMediaServerSocket();
    bool updatemedia();
    QList<LayerStatus> m_layers;
    QList<MediaLibrary> m_media;

    /*void setpath(QString path);
    void setpathu(const char *buffer);*/

    virtual void setpath(QString path);

protected:

    QString m_pathmedia;
    QTimer *n_timer;
    unsigned char * m_buffer;
    int m_bufferLen;
    QString m_peerState;
    QString m_peerName;
    QHostAddress m_host;
    quint16 m_listeningPort;
    QTcpServer *m_tcpServer;
    QTcpSocket *m_tcpSocket;


    bool sendPacket(const unsigned char *buffer, int bufferLen);

    bool sendNACK(quint32 header);

    // Parseo de paquetes MSEX
    void parsePacket(const QByteArray &byteArray);
    void parsePINFPacket(const QByteArray &byteArray);
    void parseMSEXPacket(const QByteArray &bytearray);
    void parseCINFPacket(const QByteArray &byteArray);
    void parseNACKPacket(const QByteArray &byteArray);
    void parseGELIPacket(const QByteArray &byteArray);
    void parseGEINPacket(const QByteArray &byteArray);
    void parseGLEIPacket(const QByteArray &byteArray);
    void parseGELTPacket(const QByteArray &byteArray);
    void parseGETHPacket(const QByteArray &byteArray);
    void parseGVSRPacket(const QByteArray &byteArray);
    void parseGELNPacket(const QByteArray &byteArray);
    void parseRQSTPacket(const QByteArray &byteArray);

private:

    QList<MediaInformation> getMediaInformation(QDir dir);

public slots:

protected slots:

    bool cinfprocess();
    void handleReadyRead();

private slots:

    bool transmitlsta();
    bool newPeer();

signals:

//  void newConnection();

  //MSEX Signals
 void cinfread();
 void geliread();
 void geltread();
 void gethread();
 void gvsrread();
 void rqstread();
 void gelnread();
 void geinread();
};

#endif // MEDIASERVER_H
