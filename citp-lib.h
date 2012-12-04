// -*- mode: C++ -*-
/*
The MIT License

Copyright (c) 2009 John Warwick

Copyright (c) 2012 Santi Noreña puremediaserver@gmail.com

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

#ifndef _CITPLIB_H_
#define _CITPLIB_H_

#include <qglobal.h>

#ifdef CITPLIB_LIB
# define CITPLIB_EXPORT Q_DECL_EXPORT
#else
# define CITPLIB_EXPORT Q_DECL_IMPORT
#endif

#include <QObject>
#include <QScriptable>
#include <QHostAddress>
#include <QNetworkInterface>

class PeerInformationSocket;
class MediaServer;

class CITPLIB_EXPORT CITPLib : public QObject, public QScriptable
{
  Q_OBJECT    
public:

  CITPLib(QObject *parent = 0);
  virtual ~CITPLib();

  bool createPeerInformationSocket(const QString &name, const QString &state, quint32 ipadd);

  PeerInformationSocket *m_peerSocket;

private:
  


signals:


};

#endif // _CITPLIB_H_
