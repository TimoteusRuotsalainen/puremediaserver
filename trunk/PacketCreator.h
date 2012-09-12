// -*- mode: C++ -*-
/*
   Pure Media Server - A Media Server Sotfware for stage and performing
   Copyright (C) 2012  Santiago Noreña
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

  static unsigned char * createSINFPacket(int &bufferLen);
  static unsigned char * createLSTAPacket(layerList layerp, int &bufferLen);
  static unsigned char * createNACKPacket(quint32 cookie, int &bufferLen);
  static unsigned char * createELINPacket(QList<MediaLibrary> medialib, int &bufferLen);
  static unsigned char * createMEINPacket(MediaLibrary medialib, int &bufferLen);
  static unsigned char * createETHNPacket(QString path, MediaLibrary medialib, int elementnumber, int &bufferLen);
private:

};

#endif // _PACKETCREATOR_H_
