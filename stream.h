#ifndef STREAM_H
#define STREAM_H

#include <QObject>
#include <QMessageBox>
#include <QTimer>
#include <string>
#include <zmq.h>
#include <zmq.hpp>

class stream : public QObject
{
    Q_OBJECT
public:
    explicit stream(QObject *parent = nullptr);


signals:
     void DataUpdate(std::string data);


public slots:
    void StartRecv();
    void Start();

private:
    QTimer* t;
    zmq::socket_t* socket;
};

#endif // STREAM_H
