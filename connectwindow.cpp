#include "connectwindow.h"

ConnectWindow::ConnectWindow(QWidget *parent)
    : QWidget(parent),
      mimas_select(false),
      limit_select(false)
{
    qc_mimas = new QCheckBox(tr("mimas"));
    qc_limit = new QCheckBox(tr("limit"));
    qc_selfdefine = new QCheckBox(tr("self_define"));

    connect(qc_selfdefine, SIGNAL(clicked()), this, SLOT(OnSelectSelfDefine()));

    QPushButton* connect_button = new QPushButton("Connect");
    connect(connect_button, SIGNAL(clicked()), this, SLOT(OnConnect()));
    QPushButton* cancel_button = new QPushButton("Cancel");
    connect(cancel_button, SIGNAL(clicked()), this, SLOT(OnCancel()));

    layout = new QGridLayout;

    layout->addWidget(qc_limit, 0, 0);
    layout->addWidget(qc_mimas, 100, 0);
    layout->addWidget(qc_selfdefine, 200, 0);

    layout->addWidget(connect_button, 300, 0);
    layout->addWidget(cancel_button, 300, 50);
    setLayout(layout);
}

void ConnectWindow::OnConnect() {
    emit DisconnectAll();
    if (qc_mimas->isChecked()) {
        emit MimasSocketConnect();
    }
    if (qc_limit->isChecked()) {
        emit LimitSocketConnect();
    }
    if (qc_selfdefine->isChecked()) {
        std::string text = qte->toPlainText().toStdString();
        // QMessageBox::information(this, "MarketData", text.c_str());
        std::vector<std::string> address_v = Split(text, ';');
        QMessageBox::information(this, "MarketData", address_v[0].c_str());
        emit SelfDefineSocketConnect(text.c_str());
    }
    this->close();
}

void ConnectWindow::OnCancel() {
    this->close();
}

void ConnectWindow::OnSelectSelfDefine() {
    qte = new QPlainTextEdit();
    layout->addWidget(qte, 500, 0);
}
