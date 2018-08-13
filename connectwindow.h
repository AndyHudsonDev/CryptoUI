#ifndef CONNECTWINDOW_H
#define CONNECTWINDOW_H

#include <QObject>
#include <QWidget>
#include <QCheckBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>
#include "common_util.h"

class ConnectWindow : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectWindow(QWidget *parent = nullptr);

signals:
    void DisconnectAll();
    void MimasSocketConnect();
    void LimitSocketConnect();
    void SelfDefineSocketConnect(std::string address);

public slots:
    void OnConnect();
    void OnCancel();
    void OnSelectSelfDefine();


private:
    QGridLayout* layout;
    QCheckBox* qc_mimas;
    QCheckBox* qc_limit;
    QCheckBox* qc_selfdefine;
    QPlainTextEdit* qte;
    bool mimas_select;
    bool limit_select;
    bool socket_connected;
};

#endif // CONNECTWINDOW_H
