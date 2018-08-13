#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <unistd.h>
#include <QTimer>


MainWindow::MainWindow(ConnectWindow* win) :
    ui(new Ui::MainWindow),
    cw(win),
    ex_count(-1),
    row(-1),
    high(-1),
    low(10000000),
    high_row(-1),
    low_row(-1),
    socket_connected(false),
    socket_recv_started(false)
{
    black_list.append("OKCOIN China");
    black_list.append("OKCOIN International");
    ui->setupUi(this);
    openMarket();
    openHighLow();
}

void MainWindow::openMarket() {
       market_model = new QStandardItemModel();
       market_model->setColumnCount(6);
       market_model->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("Exchange"));
       market_model->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("Product"));
       market_model->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("LastPrice"));
       market_model->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("Time"));
       market_model->setHeaderData(4,Qt::Horizontal,QString::fromLocal8Bit("nsec"));
       market_model->setHeaderData(5,Qt::Horizontal,QString::fromLocal8Bit("source"));
       ui->tv->setModel(market_model);
       // ui->tv->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
       // ui->tv->horizontalHeader()->setResizeMode(0,QHeaderView::Fixed);
       // ui->tv->horizontalHeader()->setResizeMode(1,QHeaderView::Fixed);
       ui->tv->setColumnWidth(0, 70);
       ui->tv->setColumnWidth(1, 70);
       ui->tv->setColumnWidth(2, 100);
       ui->tv->setColumnWidth(3, 70);
       ui->tv->setColumnWidth(4, 50);
       ui->tv->setColumnWidth(5, 50);
}

void MainWindow::openHighLow() {
    highlow_model = new QStandardItemModel();
    highlow_model->setColumnCount(3);
    highlow_model->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("exchange"));
    highlow_model->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("LastPrice"));
    highlow_model->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("time"));
    QStringList l;
    l.append("High");
    l.append("Low");
    l.append("Delta");
    highlow_model->setVerticalHeaderLabels(l);
    ui->tv2->setModel(highlow_model);
    ui->tv->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tv2->setColumnWidth(0, 100);
    ui->tv2->setColumnWidth(1, 100);
    ui->tv2->setColumnWidth(2, 100);

    highlow_model->setItem(0,0,new QStandardItem(QString::fromLocal8Bit("Not init")));
    highlow_model->setItem(0,1,new QStandardItem(QString::fromLocal8Bit("0")));
    highlow_model->setItem(0,2,new QStandardItem(QString::fromLocal8Bit("0")));

    highlow_model->setItem(1,0,new QStandardItem(QString::fromLocal8Bit("Not init")));
    highlow_model->setItem(1,1,new QStandardItem(QString::fromLocal8Bit("0")));
    highlow_model->setItem(1,2,new QStandardItem(QString::fromLocal8Bit("0")));
}

void MainWindow::marketUpdate(std::string data) {
    QStandardItemModel *m = reinterpret_cast<QStandardItemModel*>(ui->tv->model());
    if (row >= 0) {
       for (int i = 0; i < ui->tv->model()->columnCount(); i++) {
          m->item(row, i)->setForeground(QBrush(QColor(0, 0, 0)));
       }
    }

    std::vector<std::string> v = Split(data, '|');
    v.erase(v.end());
    std::string exchange = v[0];
    std::string product = v[1];
    std::string last_price = v[2];
    double price = atof(v[2].c_str());
    std::string second = v[3];
    std::string nsec = v[4];
    if (black_list.contains(exchange)) {
        return;
    }

    if (price < 0.001) {
        return;
    }
    if (exchange_map.find(exchange) == exchange_map.end()) {
        exchange_map[exchange] = ++ex_count;
    }
    row = exchange_map[exchange];

    price_map[row] = price;
    content_map[row] = v;

    for (int i = 0; i < ui->tv->model()->columnCount(); i++) {
        m->setItem(row,i,new QStandardItem(QString::fromLocal8Bit(v[i].c_str())));
        m->item(row,i)->setForeground(QBrush(QColor(255, 0, 0)));
    }


    if (row == high_row) {
        int max_row = -1;
        double max_price = -1.0;
        for (std::tr1::unordered_map<int, double>::iterator it = price_map.begin(); it != price_map.end(); it++) {
            if (it->second > max_price) {
                max_price = it->second;
                max_row = it->first;
            }
        }
        high = max_price;
        UpdateHighLowWindow(0, max_row);
    } else if (row == low_row) {
        int min_row = -1;
        double min_price = 1000000;
        for (std::tr1::unordered_map<int, double>::iterator it = price_map.begin(); it != price_map.end(); it++) {
            if (it->second < min_price) {
                min_price = it->second;
                min_row = it->first;
            }
        }
        low = min_price;
        UpdateHighLowWindow(1, min_row);
    } else {
        if (price > high) {

            high = price;
            high_row = row;
            UpdateHighLowWindow(0, row);
        }
        if (price < low) {
            low = price;
            low_row = row;
            UpdateHighLowWindow(1, row);
        }
    }


}


void MainWindow::UpdateHighLowWindow(int line, int r) {
    std::vector<std::string> temp_v = content_map[r];
    std::string exchange = temp_v[0];
    std::string product = temp_v[1];
    std::string last_price = temp_v[2];
    std::string second = temp_v[3];
    std::string nsec = temp_v[4];
    QStandardItemModel* m = reinterpret_cast<QStandardItemModel*>(ui->tv2->model());
    m->setItem(line,0,new QStandardItem(QString::fromLocal8Bit(exchange.c_str())));
    m->setItem(line,1,new QStandardItem(QString::fromLocal8Bit(last_price.c_str())));
    m->setItem(line,2,new QStandardItem(QString::fromLocal8Bit(second.c_str())));
    // QMessageBox::information(this, "MarketData", exchange.c_str());
    UpdateDeltaWindow();
}

void MainWindow::UpdateDeltaWindow() {
    QStandardItemModel* m = reinterpret_cast<QStandardItemModel*>(ui->tv2->model());
    if (high < 0 || low > 9999999) {
        m->setItem(2,0,new QStandardItem(QString::fromLocal8Bit("NULL")));
        m->setItem(2,1,new QStandardItem(QString::fromLocal8Bit("NULL")));
        m->setItem(2,2,new QStandardItem(QString::fromLocal8Bit("NULL")));
        return;
    }
    double delta_price = high - low;
    char delta_price_str[16];
    snprintf(delta_price_str, sizeof(delta_price_str), "%lf", delta_price);
    std::vector<std::string> high_v = content_map[high_row];
    std::vector<std::string> low_v = content_map[low_row];
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
    delete market_model;
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

void MainWindow::UpdateDelta() {
    QStandardItemModel* m = reinterpret_cast<QStandardItemModel*>(ui->tv2->model());
    double high_price = atof(m->item(0, 1)->accessibleText().toStdString().c_str());
    double low_price = atof(m->item(1,1)->accessibleText().toStdString().c_str());
    double price_delta = high_price - low_price;
    char price_delta_str[20];
    snprintf(price_delta_str, sizeof(price_delta_str), "%lf", price_delta);
    int high_time = atoi(m->item(0, 2)->accessibleText().toStdString().c_str());
    int low_time = atoi(m->item(1, 2)->accessibleText().toStdString().c_str());
    int time_delta = high_time - low_time;
    char time_delta_str[20];
    snprintf(time_delta_str, sizeof(time_delta_str), "%d", time_delta);
    m->setItem(2,0,new QStandardItem(QString::fromLocal8Bit("NULL")));
    m->setItem(2,1,new QStandardItem(QString::fromLocal8Bit(price_delta_str)));
    m->setItem(2,2,new QStandardItem(QString::fromLocal8Bit(time_delta_str)));
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
    if (socket_recv_started) {
        QMessageBox::information(this, "ERROR", "Disconnect First!");
        return;
    }
    delete market_model;
    delete highlow_model;
    ex_count = -1;
    row = -1;
    high = -1;
    low = 10000000;
    high_row = -1;
    low_row = -1;
    socket_connected = false;
    socket_recv_started = false;
    price_map.clear();
    content_map.clear();
    openMarket();
    openHighLow();
}
