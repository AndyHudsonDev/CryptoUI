#ifndef STREAM_H
#define STREAM_H

#include <QObject>
#include <QMessageBox>
#include <QTimer>
#include <string>
#include <zmq.h>
#include <zmq.hpp>
#include "common_util.h"

class stream : public QObject
{
    Q_OBJECT
public:
    explicit stream(QObject *parent = nullptr);
    ~stream();


signals:
     void DataUpdate(std::string data);

     void SocketDisconnectDone();
     void MimasConnectDone();
     void LimitConnectDone();
     void SelfDefineConnectDone();
     void SocketRecvStarted();

public slots:
    void StartRecv();
    void Start();

    void OnDisconnectALLRequest();
    void OnMimasConnectRequest();
    void OnLimitConnectRequest();
    void OnSelfDefineConnectRequest(std::string address);

private:
    QTimer* t;
    zmq::context_t* context;
    zmq::socket_t* socket;
};

#endif // STREAM_H
