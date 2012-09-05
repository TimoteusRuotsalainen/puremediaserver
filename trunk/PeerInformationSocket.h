// -*- mode: C++ -*-
#ifndef _PEERINFORMATIONSOCKET_H_
#define _PEERINFORMATIONSOCKET_H_

#include <QUdpSocket>

class QTimer;
class MediaServer;

struct PeerDescripton;

class PeerInformationSocket : public QUdpSocket
{
  Q_OBJECT
public:
  PeerInformationSocket(QObject *parent=0);
  virtual ~PeerInformationSocket();

  bool init(const QString &name, const QString &state);

private slots:

  void transmitPLoc();
  void handleReadReady();

private:
  QString m_name;
  QString m_state;
  QTimer *m_timer;
  unsigned char *m_packetBuffer;
  int m_packetBufferLen;

  void processPacket(const QHostAddress &sender, const QByteArray &packet);

signals:

  void peersUpdated();
};

#endif // _PEERINFORMATIONSOCKET_H_
