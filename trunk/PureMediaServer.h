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

    QString m_pathmedia; // Path to Medias
    MediaServer *m_mediaserver; // MSEx
    CITPLib *m_citp;    // CITP Peer. PLOc
    QProcess *ola; // OLA daemon process

    // Video TCP Sockets

    QTcpSocket *m_pd_write;
    QTcpServer *m_pd_read;
    QTcpSocket *m_tcpsocket;
    QProcess *pd; // Pure Data process for video

    // Audio TCP Sockets

    QTcpSocket *m_pd_write_audio;
    QTcpServer *m_pd_read_audio;
    QTcpSocket *m_tcpsocket_audio;
    QProcess *pd_audio; // Pure Data process for audio

    QTimer *m_preview;
//    void contextMenuEvent(QContextMenuEvent *event);

private:

    Ui::PureMediaServer ui;

    bool sendPacket(const char *buffer, int bufferLen);

    void pdstart();

    void errorsending();

    bool sendPacket_audio(const char *buffer, int bufferLen);

    void pdstart_audio();

    void errorsending_audio();

public slots:

private slots:

    void olastart(); // Init the OLA daemon

    void on_ChangePath_clicked();// Change the path to medias

// Video

    void on_updateButton_clicked();  // Init the CITP/MSEx protocol
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
    void pdrestart();
    void stdout();
    void on_video_stateChanged(int state);

    //Audio

    void newPeer_audio();
    void newmessage_audio();
    void newconexion_audio();
    void on_readDMX_audio_stateChanged(int state);
    void on_layer1Add_audio_valueChanged();
    void on_layer1Check_audio_stateChanged (int state);
    void on_layer2Add_audio_valueChanged();
    void on_layer2Check_audio_stateChanged (int state);
    void on_layer3Add_audio_valueChanged();
    void on_layer3Check_audio_stateChanged (int state);
    void on_layer4Add_audio_valueChanged();
    void on_layer4Check_audio_stateChanged (int state);
    void on_layer5Add_audio_valueChanged();
    void on_layer5Check_audio_stateChanged (int state);
    void on_layer6Add_audio_valueChanged();
    void on_layer6Check_audio_stateChanged (int state);
    void on_layer7Add_audio_valueChanged();
    void on_layer7Check_audio_stateChanged (int state);
    void on_layer8Add_audio_valueChanged();
    void on_layer8Check_audio_stateChanged (int state);
    void pdrestart_audio();
    void stdout_audio();
    void on_audio_stateChanged(int state);

    // File configuration

    void open();
    void save();

    // Previews

    void previewLayer1();
    void previewLayer2();
    void previewLayer3();
    void previewLayer4();
    void previewLayer5();
    void previewLayer6();
    void previewLayer7();
    void previewLayer8();
    void previewMaster();
};

#endif // PUREMEDIASERVER_H
