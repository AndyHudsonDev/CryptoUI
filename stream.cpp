#include "stream.h"
#include <QMessageBox>
#include <unistd.h>

stream::stream(QObject *parent) : QObject(parent)
{
    std::vector<std::string> connect_address_v;
    connect_address_v.push_back("tcp://54.67.2.43:40021");
    connect_address_v.push_back("tcp://52.193.146.32:40021");
    zmq::context_t* context = new zmq::context_t(1);
    socket = new zmq::socket_t(*context, ZMQ_SUB);
    socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
    for (int i = 0; i < connect_address_v.size(); i++) {
        socket->connect(connect_address_v[i].c_str());
    }
}

void stream::Start() {
    t = new QTimer();
    QObject::connect(t, SIGNAL(timeout()), this, SLOT(StartRecv()));
    t->start(200);
}

void stream::StartRecv() {
    zmq::message_t message;
    socket->recv(&message);
    std::string recv_str = static_cast<char*>(message.data());
    emit DataUpdate(recv_str);
}

