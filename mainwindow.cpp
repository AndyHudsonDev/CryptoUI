#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <unistd.h>
#include <QTimer>

std::vector<std::string> Split(std::string raw_string, char split_char) {
    std::vector<std::string> result;
    int pos = -1;
    for (unsigned int i = 0; i < raw_string.size(); i++) {
      if (raw_string[i] == split_char) {
        result.push_back(raw_string.substr(pos+1, i-pos-1));
        pos = i;
      }
    }
    result.push_back(raw_string.substr(pos+1, raw_string.size()-pos));
    return result;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    ex_count(-1),
    row(-1),
    high(-1),
    low(10000000),
    high_row(-1),
    low_row(-1)
{
    // dg = new Dialog();
    // QWidget * qw = new QWidget();
    black_list.append("OKCOIN China");
    black_list.append("OKCOIN International");
    ui->setupUi(this);
    openMarket();
    openHighLow();
    // QObject::connect(this, SIGNAL(ConnectClicked()), dg, SLOT(ShowFontPage()));
}

void MainWindow::openMarket() {
       QStandardItemModel *model = new QStandardItemModel();
       model->setColumnCount(6);

       model->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("Exchange"));
       model->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("Product"));
       model->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("LastPrice"));
       model->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("Time"));
       model->setHeaderData(4,Qt::Horizontal,QString::fromLocal8Bit("nsec"));
       model->setHeaderData(5,Qt::Horizontal,QString::fromLocal8Bit("source"));
       ui->tv->setModel(model);
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
    QStandardItemModel *model = new QStandardItemModel();
    model->setColumnCount(3);
    model->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("exchange"));
    model->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("LastPrice"));
    model->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("time"));
    QStringList l;
    l.append("High");
    l.append("Low");
    l.append("Delta");
    model->setVerticalHeaderLabels(l);
    ui->tv2->setModel(model);
    ui->tv->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tv2->setColumnWidth(0, 100);
    ui->tv2->setColumnWidth(1, 100);
    ui->tv2->setColumnWidth(2, 100);

    model->setItem(0,0,new QStandardItem(QString::fromLocal8Bit("Not init")));
    model->setItem(0,1,new QStandardItem(QString::fromLocal8Bit("0")));
    model->setItem(0,2,new QStandardItem(QString::fromLocal8Bit("0")));

    model->setItem(1,0,new QStandardItem(QString::fromLocal8Bit("Not init")));
    model->setItem(1,1,new QStandardItem(QString::fromLocal8Bit("0")));
    model->setItem(1,2,new QStandardItem(QString::fromLocal8Bit("0")));
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
        // QMessageBox::information(this, "MarketData", "ok for line 170");
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
    delete dg;
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
    /*
    zmq::context_t* context = new zmq::context_t(1);
    socket = new zmq::socket_t(*context, ZMQ_SUB);
    socket->setsockopt(ZMQ_SUBSCRIBE, "", 0);
    // socket->connect("tcp://54.67.2.43:40021");
    socket->connect("tcp://52.193.146.32:40021");
    QMessageBox::information(this, "Connected", "Connect to 52.193.146.32:40021: Success!");
    */
    // this->ui->tv->show();

    // emit ConnectClicked();
    // dg->show();
   ConnectPage* p = new ConnectPage();
   p->show();
}


void MainWindow::on_start_clicked()
{
    /*
    zmq::message_t message;
    socket->recv(&message);
    std::string recv_str = static_cast<char*>(message.data());
    std::vector<std::string> v = Split(recv_str, '|');
    v.erase(v.end());
    //QMessageBox::information(this, "MarketData", v[0].c_str());
    for (int i = 0; i < 4; i++) {
        reinterpret_cast<QStandardItemModel*>(ui->tv->model())->setItem(0,i,new QStandardItem(QString::fromLocal8Bit(v[i].c_str())));
    }
    */
    emit StartClicked();
    // QMessageBox::information(this, "Connected", "emit clicked");
}


void MainWindow::on_exit_clicked()
{
    exit(1);
}
