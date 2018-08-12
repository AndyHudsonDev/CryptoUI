#include "connectpage.h"

ConnectPage::ConnectPage(QWidget *parent) : QWidget(parent)
{

    QCheckBox* qc_mimas = new QCheckBox(tr("mimas"));
    QCheckBox* qc_limit = new QCheckBox(tr("limit"));
    QCheckBox* qc_selfdefine = new QCheckBox(tr("self_define"));

    QPushButton* connect_button = new QPushButton("Connect");
    QPushButton* cancel_button = new QPushButton("Cancel");
    QPlainTextEdit* qte = new QPlainTextEdit();
    QGridLayout *layout = new QGridLayout;

    layout->addWidget(qc_limit, 0, 0);
    layout->addWidget(qc_mimas, 100, 0);
    layout->addWidget(qc_selfdefine, 200, 0);


    layout->addWidget(qte, 200, 0);
    layout->addWidget(qb, 0, 0);
    setLayout(layout);
}

ConnectPage::~ConnectPage(){

}

void ConnectPage::ShowPage() {

}
