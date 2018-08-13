#include "stream.h"
#include <QMessageBox>
#include <unistd.h>

stream::stream(QObject *parent) : QObject(parent)
{
    context = new zmq::context_t(1);
    socket = new zmq::socket_t(*context, ZMQ_SUB);
    socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
    t = new QTimer();
}

stream::~stream() {
    delete context;
    delete socket;
    delete t;
}

void stream::Start() {
    QObject::connect(t, SIGNAL(timeout()), this, SLOT(StartRecv()));
    t->start(20);
    emit SocketRecvStarted();
}

void stream::StartRecv() {
    zmq::message_t message;
    socket->recv(&message);
    std::string recv_str = static_cast<char*>(message.data());
    emit DataUpdate(recv_str);
}

void stream::OnDisconnectALLRequest() {
    t->stop();
    socket->close();
    context = new zmq::context_t(1);
    socket = new zmq::socket_t(*context, ZMQ_SUB);
    socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
    emit SocketDisconnectDone();
}

void stream::OnLimitConnectRequest() {
    socket->connect("tcp://52.193.146.32:40021");
    emit LimitConnectDone();
}

void stream::OnMimasConnectRequest() {
    socket->connect("tcp://54.67.2.43:40021");
    emit MimasConnectDone();
}

void stream::OnSelfDefineConnectRequest(std::string address) {
    std::vector<std::string> address_v = Split(address, ';');
    for (int i = 0; i < address_v.size(); i++) {
        if (!CheckAddressLegal(address_v[i])) {
            return;
        }
        socket->connect(address_v[i].c_str());
    }
    emit SelfDefineConnectDone();
}
