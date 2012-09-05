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

bool CITPLib::createPeerInformationSocket(const QString &name, const QString &state)
{
  if (!m_peerSocket)
    {
      return false;
    }
  qDebug() << "about to call peer socket init";
  return m_peerSocket->init(name, state);
}
