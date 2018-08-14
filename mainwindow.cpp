#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <unistd.h>
#include <QTimer>


MainWindow::MainWindow(ConnectWindow* win, std::tr1::unordered_map<std::string, int> pd) :
    ui(new Ui::MainWindow),
    cw(win),
    socket_connected(false),
    socket_recv_started(false),
    p_d(pd),
    model_num(0),
    showmodel_no(0)
{
    // std::string combostr = ui->comboBox->currentText().toStdString();
    InitVariable();
    ui->setupUi(this);

    ui->tv->setModel(market_model[showmodel_no]);
    ui->tv2->setModel(highlow_model[showmodel_no]);
}

void MainWindow::InitVariable() {
    pre_row.clear();
    exchange_map.clear();
    model_map.clear();
    content_map.clear();
    price_map.clear();
    ex_count.clear();
    row.clear();
    high.clear();
    low.clear();
    high_row.clear();
    low_row.clear();

    for (std::tr1::unordered_map<std::string, int>::iterator it = p_d.begin(); it != p_d.end(); it++) {
        if (it->second > model_num) {
            model_num = it->second;
        }
    }
    model_num++;
    for (int i = 0; i < model_num; i++) {
        market_model[i] = InitMarketModel(market_model[i]);
        highlow_model[i] = InitHighlowModel(highlow_model[i]);
    }

    for (std::tr1::unordered_map<std::string, int>::iterator it = p_d.begin(); it != p_d.end(); it++) {
        model_map[it->first] = market_model[it->second];
        highlow_map[it->first] = highlow_model[it->second];
        ex_count[it->second] = -1;
        row[it->second] = -1;
        high[it->second] = -1;
        low[it->second] = 100000000;
        high_row[it->second] = -1;
        low_row[it->second] = -1;
        pre_row[it->second] = -1;
    }
    black_list.append("OKCOIN China");
    black_list.append("OKCOIN International");

}

QStandardItemModel* MainWindow::InitMarketModel(QStandardItemModel* m) {
       m = new QStandardItemModel();
       m->setColumnCount(6);
       m->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("Exchange"));
       m->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("Product"));
       m->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("LastPrice"));
       m->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("Time"));
       m->setHeaderData(4,Qt::Horizontal,QString::fromLocal8Bit("nsec"));
       m->setHeaderData(5,Qt::Horizontal,QString::fromLocal8Bit("source"));
       return m;
}

QStandardItemModel* MainWindow::InitHighlowModel(QStandardItemModel* m) {
    m = new QStandardItemModel();
    m->setColumnCount(3);
    m->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("exchange"));
    m->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("LastPrice"));
    m->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("time"));
    QStringList l;
    l.append("High");
    l.append("Low");
    l.append("Delta");
    m->setVerticalHeaderLabels(l);

    m->setItem(0,0,new QStandardItem(QString::fromLocal8Bit("Not init")));
    m->setItem(0,1,new QStandardItem(QString::fromLocal8Bit("0")));
    m->setItem(0,2,new QStandardItem(QString::fromLocal8Bit("0")));

    m->setItem(1,0,new QStandardItem(QString::fromLocal8Bit("Not init")));
    m->setItem(1,1,new QStandardItem(QString::fromLocal8Bit("0")));
    m->setItem(1,2,new QStandardItem(QString::fromLocal8Bit("0")));
    return m;
}

void MainWindow::marketUpdate(std::string data) {
    // QMessageBox::information(this, "MarketData", data.c_str());
    std::vector<std::string> v = Split(data, '|');
    v.erase(v.end());
    std::string exchange = v[0];
    std::string product = v[1];
    std::string last_price = v[2];
    double price = atof(v[2].c_str());
    std::string second = v[3];
    std::string nsec = v[4];
    std::string source = v[5];
    if (p_d.find(product) == p_d.end()) {
        QMessageBox::information(this, "modelnofail", product.c_str());
        return;
    }
    int model_no = p_d[product];

    if (model_map.find(product) == model_map.end()) {
        // TODO(nick): add other
        QMessageBox::information(this, "NULL", product.c_str());
        return;
    }
    QStandardItemModel *m = model_map[product];

    if (pre_row[model_no] >= 0) {
       for (int i = 0; i < m->columnCount(); i++) {
          m->item(pre_row[model_no], i)->setForeground(QBrush(QColor(0, 0, 0)));
       }
    }

    if (black_list.contains(exchange)) {
        return;
    }

    if (price < 0.001) {
        return;
    }

    if (exchange_map[model_no].find(exchange) == exchange_map[model_no].end()) {
        exchange_map[model_no][exchange] = ex_count[model_no]+1;
        ex_count[model_no] = ex_count[model_no] + 1;
    }
    row[model_no] = exchange_map[model_no][exchange];

    price_map[model_no][row[model_no]] = price;
    content_map[model_no][row[model_no]] = v;

    for (int i = 0; i < m->columnCount(); i++) {
        m->setItem(row[model_no],i,new QStandardItem(QString::fromLocal8Bit(v[i].c_str())));
        m->item(row[model_no],i)->setForeground(QBrush(QColor(255, 0, 0)));
    }
    pre_row[model_no] = row[model_no];

    if (row[model_no] == high_row[model_no]) {
        int max_row = -1;
        double max_price = -1.0;
        for (std::tr1::unordered_map<int, double>::iterator it = price_map[model_no].begin(); it != price_map[model_no].end(); it++) {
            if (it->second > max_price) {
                max_price = it->second;
                max_row = it->first;
            }
        }
        high[model_no] = max_price;
        UpdateHighLowWindow(model_no, 0, max_row);
    } else if (row[model_no] == low_row[model_no]) {
        int min_row = -1;
        double min_price = 1000000;
        for (std::tr1::unordered_map<int, double>::iterator it = price_map[model_no].begin(); it != price_map[model_no].end(); it++) {
            if (it->second < min_price) {
                min_price = it->second;
                min_row = it->first;
            }
        }
        low[model_no] = min_price;
        UpdateHighLowWindow(model_no, 1, min_row);
    } else {
        if (price > high[model_no]) {
            high[model_no] = price;
            high_row[model_no] = row[model_no];
            UpdateHighLowWindow(model_no, 0, row[model_no]);
        }
        if (price < low[model_no]) {
            low[model_no] = price;
            low_row[model_no] = row[model_no];
            UpdateHighLowWindow(model_no, 1, row[model_no]);
        }
    }
}


void MainWindow::UpdateHighLowWindow(int no, int line, int r) {
    std::vector<std::string> temp_v = content_map[no][r];
    std::string exchange = temp_v[0];
    std::string product = temp_v[1];
    std::string last_price = temp_v[2];
    std::string second = temp_v[3];
    std::string nsec = temp_v[4];
    std::string source = temp_v[5];
    QStandardItemModel* m = highlow_model[no];
    m->setItem(line,0,new QStandardItem(QString::fromLocal8Bit(exchange.c_str())));
    m->setItem(line,1,new QStandardItem(QString::fromLocal8Bit(last_price.c_str())));
    m->setItem(line,2,new QStandardItem(QString::fromLocal8Bit(second.c_str())));
    // QMessageBox::information(this, "MarketData", exchange.c_str());
    UpdateDeltaWindow(no);
}

void MainWindow::UpdateDeltaWindow(int no) {
    QStandardItemModel* m = highlow_model[no];
    if (high[no] < 0 || low[no] > 9999999) {
        m->setItem(2,0,new QStandardItem(QString::fromLocal8Bit("NULL")));
        m->setItem(2,1,new QStandardItem(QString::fromLocal8Bit("NULL")));
        m->setItem(2,2,new QStandardItem(QString::fromLocal8Bit("NULL")));
        return;
    }
    double delta_price = high[no] - low[no];
    char delta_price_str[16];
    snprintf(delta_price_str, sizeof(delta_price_str), "%lf", delta_price);
    std::vector<std::string> high_v = content_map[no][high_row[no]];
    std::vector<std::string> low_v = content_map[no][low_row[no]];
    std::string hightime_str = high_v[3];
    std::string lowtime_str = low_v[3];
    int delta_time = atoi(hightime_str.c_str()) - atoi(lowtime_str.c_str());
    char delta_time_str[16];
    snprintf(delta_time_str, sizeof(delta_time_str), "%d", delta_time);

    m->setItem(2,0,new QStandardItem(QString::fromLocal8Bit("NULL")));
    m->setItem(2,1,new QStandardItem(QString::fromLocal8Bit(delta_price_str)));
    m->setItem(2,2,new QStandardItem(QString::fromLocal8Bit(delta_time_str)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete highlow_model;
    for (int i = 0; i < model_num; i++) {
        delete market_model[i];
    }
}

void MainWindow::UpdateHigh(std::vector<std::string> v) {
    std::string exchange = v[0];
    std::string product = v[1];
    std::string last_price = v[2];
    std::string second = v[3];
    std::string nsec = v[4];
    QStandardItemModel* m = reinterpret_cast<QStandardItemModel*>(ui->tv2->model());
    m->setItem(0,0,new QStandardItem(QString::fromLocal8Bit(exchange.c_str())));
    m->setItem(0,1,new QStandardItem(QString::fromLocal8Bit(last_price.c_str())));
    m->setItem(0,2,new QStandardItem(QString::fromLocal8Bit(second.c_str())));
}

void MainWindow::UpdateLow(std::vector<std::string> v) {
    std::string exchange = v[0];
    std::string product = v[1];
    std::string last_price = v[2];
    std::string second = v[3];
    std::string nsec = v[4];
    QStandardItemModel* m = reinterpret_cast<QStandardItemModel*>(ui->tv2->model());
    m->setItem(1,0,new QStandardItem(QString::fromLocal8Bit(exchange.c_str())));
    m->setItem(1,2,new QStandardItem(QString::fromLocal8Bit(last_price.c_str())));
    m->setItem(1,2,new QStandardItem(QString::fromLocal8Bit(second.c_str())));
}

void MainWindow::on_connect_clicked()
{
   socket_recv_started = false;
   cw->show();
}


void MainWindow::on_start_clicked()
{
    if (!socket_connected) {
        QMessageBox::information(this, "ERROR", "Connect server First!");
        return;
    }
    emit StartClicked();
}


void MainWindow::on_exit_clicked()
{
    exit(1);
}

void MainWindow::OnDisconnectDone() {
    socket_connected = false;
    socket_recv_started = false;
    emit ReStart();
}

void MainWindow::OnMimasConnectDone()
{
    socket_connected = true;
}

void MainWindow::OnLimitConnectDone() {
    socket_connected = true;
}

void MainWindow::OnSelfDefineConnectDone(){
    socket_connected = true;
}


void MainWindow::OnSocketRecvStarted() {
    socket_recv_started = true;
}

void MainWindow::on_disconnect_clicked()
{
    emit MainWindowDisconnect();
}

void MainWindow::on_clear_clicked()
{
    ui->comboBox->setCurrentIndex(showmodel_no);
    if (socket_recv_started) {
        QMessageBox::information(this, "ERROR", "Disconnect First!");
        return;
    }

    InitVariable();
    ui->tv->setModel(market_model[showmodel_no]);
    ui->tv2->setModel(highlow_model[showmodel_no]);
}

void MainWindow::on_comboBox_currentIndexChanged(const QString &arg1)
{
    std::string s = arg1.toStdString();
    if (s == "BTCUSD") {
        ui->tv->setModel(market_model[0]);
        ui->tv2->setModel(highlow_model[0]);
    }
    if (s == "ETHUSD") {
        ui->tv->setModel(market_model[1]);
        ui->tv2->setModel(highlow_model[1]);
    }
}
