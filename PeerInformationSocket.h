// -*- mode: C++ -*-
/*
The MIT License

Copyright (c) 2009 John Warwick

Copyright c() 2012 Santi Noreña

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

  bool init(const QString &name, const QString &state, quint32 ipadd);

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
