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
#include "citp-lib.h"

#include <QtDebug>

#include "CITPDefines.h"
#include "PeerInformationSocket.h"

CITPLib::CITPLib(QObject *parent)
  : QObject(parent),
    m_peerSocket(NULL)
{
  m_peerSocket = new PeerInformationSocket(this);
  Q_CHECK_PTR(m_peerSocket);
}

CITPLib::~CITPLib()
{
}

bool CITPLib::createPeerInformationSocket(const QString &name, const QString &state, quint32 ipadd)
{
  if (!m_peerSocket)
    {
      return false;
    }
  return m_peerSocket->init(name, state, ipadd);
}
