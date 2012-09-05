// -*- mode: C++ -*-
#ifndef _PACKETCREATOR_H_
#define _PACKETCREATOR_H_

#include "MSEXDefines.h"
#include <QObject>

typedef QList<LayerStatus> layerList;
class PacketCreator : public QObject
{
  Q_OBJECT
public:
  PacketCreator(QObject *parent=0);
  virtual ~PacketCreator();

// CITP

   // returns NULL on error. bufferLen is the length of the returned buffer.
  static unsigned char * createPLocPacket(const QString &name, const QString &state,
                      int &bufferLen);

  // returns NULL on error. bufferLen is the length of the returned buffer.
  static unsigned char * createUNamPacket(quint8 universeIndex, const QString &name,
					  int &bufferLen);

// MSEX

  static unsigned char * createSINFPacket(quint8 m_layers, int &bufferLen);
  static unsigned char * createLSTAPacket(layerList layerp, int &bufferLen);
  static unsigned char * createNACKPacket(quint32 cookie, int &bufferLen);
  static unsigned char * createELINPacket(QList<MediaLibrary> medialib, int &bufferLen);
  static unsigned char * createMEINPacket(MediaLibrary medialib, int &bufferLen);
  static unsigned char * createETHNPacket(MediaLibrary medialib, int elementnumber, int &bufferLen);
private:

};

#endif // _PACKETCREATOR_H_
