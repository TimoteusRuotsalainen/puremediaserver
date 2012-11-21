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

#ifndef PUREMEDIASERVER_H
#define PUREMEDIASERVER_H

#include <QMainWindow>
#include "MediaServer.h"
#include "ui_PureMediaServer.h"

class CITPLib;
class MediaServer;
class QMenu;
class QProcess;

class PureMediaServer : public QMainWindow
{
    Q_OBJECT

public:

    PureMediaServer (QWidget *parent = 0);
    virtual ~PureMediaServer();

protected:

    QString m_pathmedia;
    MediaServer *m_mediaserver;
    CITPLib *m_citp;    
    QTcpSocket *m_pd_write;
    QTcpServer *m_pd_read;
    QTcpSocket *m_tcpsocket;
//    void contextMenuEvent(QContextMenuEvent *event);
    QProcess *ola;
    QProcess *pd;

private:

    Ui::PureMediaServer ui;

    bool sendPacket(const char *buffer, int bufferLen);

    void pdstart();

    void errorsending();

public slots:

private slots:

    void on_updateButton_clicked();

//    void on_restartPD_clicked();

    void newPeer();
    void newmessage();
    void newconexion();

    void on_window_stateChanged(int state);

    void on_winpositionx_valueChanged();
    void on_winpositiony_valueChanged();
    void on_winsizex_valueChanged();
    void on_winsizey_valueChanged();

    void on_readDMX_stateChanged(int state);

    void on_layer1Add_valueChanged();
    void on_layer1Check_stateChanged (int state);
    void on_layer2Add_valueChanged();
    void on_layer2Check_stateChanged (int state);
    void on_layer3Add_valueChanged();
    void on_layer3Check_stateChanged (int state);
    void on_layer4Add_valueChanged();
    void on_layer4Check_stateChanged (int state);
    void on_layer5Add_valueChanged();
    void on_layer5Check_stateChanged (int state);
    void on_layer6Add_valueChanged();
    void on_layer6Check_stateChanged (int state);
    void on_layer7Add_valueChanged();
    void on_layer7Check_stateChanged (int state);
    void on_layer8Add_valueChanged();
    void on_layer8Check_stateChanged (int state);
    void on_ChangePath_clicked();

    void open();
    void save();

    void olastart();
    void pdrestart();

    void stdout();

    void on_video_stateChanged(int state);
};

#endif // PUREMEDIASERVER_H
