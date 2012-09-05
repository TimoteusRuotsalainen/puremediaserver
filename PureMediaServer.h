#ifndef PUREMEDIASERVER_H
#define PUREMEDIASERVER_H

#include <QMainWindow>
#include "MediaServer.h"
#include "ui_PureMediaServer.h"

class CITPLib;
class MediaServer;

class PureMediaServer : public QMainWindow
{
    Q_OBJECT

public:

    PureMediaServer (QWidget *parent = 0);
    virtual ~PureMediaServer();

protected:

    MediaServer *m_mediaserver;
    CITPLib *m_citp;    
    QTcpSocket *m_pd_socket;

private:

    Ui::PureMediaServer ui;

    bool sendPacket(const char *buffer, int bufferLen);


public slots:



private slots:

    void close();
    void on_updateButton_clicked();
    void on_connectPDButton_clicked();

    void handleReadyRead();
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

};

#endif // PUREMEDIASERVER_H
