#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <unistd.h>
#include <QTimer>

MainWindow::MainWindow(ConnectWindow* win, std::tr1::unordered_map<std::string, int> pd, std::vector<std::string>product_v) :
    ui(new Ui::MainWindow),
    cw(win),
    socket_connected(false),
    socket_recv_started(false),
    p_d(pd),
    show_symbol(product_v),
    model_num(0),
    showmodel_no(0)
{
    struct_vector.clear();
    struct_vector.push_back("topic");
    struct_vector.push_back("exchange");
    struct_vector.push_back("product");
    struct_vector.push_back("bid1");
    struct_vector.push_back("bsize1");
    struct_vector.push_back("ask1");
    struct_vector.push_back("asize1");
    struct_vector.push_back("lastprice");
    struct_vector.push_back("volume");
    struct_vector.push_back("turnover");
    struct_vector.push_back("openinterest");
    struct_vector.push_back("time");
    struct_vector.push_back("nsec");
    struct_vector.push_back("source");
    ui->setupUi(this);
    ui->area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    ui->area->setBackground(QBrush(qRgb(75,75,75)));

    for (std::tr1::unordered_map<std::string, int>::iterator it = p_d.begin(); it != p_d.end(); it++) {
        if (it->second > model_num) {
            model_num = it->second;
        }
    }
    model_num++;

    ClearVariable();

    for (int i = 0; i < model_num + 1; i++) {
        market_model[i] = InitMarketModel(market_model[i]);
        // highlow_model[i] = InitHighlowModel(highlow_model[i]);
    }

    highlow_assemble_model = InitHighlowModel(highlow_assemble_model);


    // highlow_model[model_num] = InitHighlowModel(highlow_model[model_num]);
    marketview = new QTableView;
    highlowview = new QTableView;
    marketview->horizontalHeader()->setStretchLastSection(true);
    highlowview->horizontalHeader()->setStretchLastSection(true);
    marketview->setModel(market_model[showmodel_no]);
    highlowview->setModel(highlow_assemble_model);

    MarketWin = ui->area->addSubWindow(marketview);
    HighlowWin = ui->area->addSubWindow(highlowview);


    QSize area_size = ui->area->size();
    int height = area_size.rheight();
    int width = area_size.rwidth();
    double m_rate = 3.0/5;
    MarketWin->resize(width*m_rate, height);
    HighlowWin->resize(width*(1-m_rate), height);

    show_symbol.push_back("Other");

    // QMessageBox::information(this, "MarketData", "asdasdsad");
    /*
    for (int i = 0; i < model_num+1; i++) {
        show_symbol.push_back("Uninit");
    }


    show_symbol[0] = "BTCUSD";
    show_symbol[1] = "ETHUSD";
    show_symbol[2] = "ETHBTC";
    show_symbol[3] = "XRPBTC";
    show_symbol[4] = "LTCUSD";
    show_symbol[5] = "other";
    */

    for (int i = 0; i < show_symbol.size(); i++) {
        ui->comboBox->addItem(show_symbol[i].c_str());
    }
    ui->comboBox->setCurrentIndex(showmodel_no);
    // black_list.append("okcoin");
    //black_list.append("OKCOIN International");
}

void MainWindow::InitVariable() {

}

void MainWindow::ClearVariable() {
    pre_row.clear();
    exchange_map.clear();
    std::tr1::unordered_map<std::string, int> temp_map;
    for (int i = 0; i < NUM_MODEL; i++) {
        exchange_map[i] = temp_map;
    }
    content_map.clear();
    // price_map.clear();
    bid_map.clear();
    ask_map.clear();
    mid_map.clear();
    ex_count.clear();
    row.clear();
    high_bid.clear();
    low_ask.clear();
    high_bid_row.clear();
    low_ask_row.clear();

    for (int i = 0; i < model_num+1; i++) {
        ex_count[i] = -1;
        row[i] = -1;
        high_bid[i] = -1;
        low_ask[i] = 100000000;
        high_bid_row[i] = -1;
        low_ask_row[i] = -1;
        pre_row[i] = -1;
    }
}

QStandardItemModel* MainWindow::InitMarketModel(QStandardItemModel* m) {
       m = new QStandardItemModel();
       m->setColumnCount(struct_vector.size());
       for (int i = 0; i < m->columnCount(); i++) {
           m->setHeaderData(i,Qt::Horizontal,QString::fromLocal8Bit(struct_vector[i].c_str()));
       }
       return m;
}

QStandardItemModel* MainWindow::InitHighlowModel(QStandardItemModel* m) {
    m = new QStandardItemModel();
    m->setColumnCount(4);

    m->setHeaderData(0,Qt::Horizontal,QString::fromLocal8Bit("Product"));
    m->setHeaderData(1,Qt::Horizontal,QString::fromLocal8Bit("Exchange"));
    m->setHeaderData(2,Qt::Horizontal,QString::fromLocal8Bit("Price"));
    m->setHeaderData(3,Qt::Horizontal,QString::fromLocal8Bit("Time"));
    QStringList l;
    for (int i = 0; i < model_num+1; i++) {
        l.append("HighBid");
        l.append("LowAsk");
        l.append("Delta");
        l.append("Delta%");
    }
    m->setVerticalHeaderLabels(l);

    return m;
}

void MainWindow::marketUpdate(std::string data) {
    //QMessageBox::information(this, "MarketData", data.c_str());
    std::vector<std::string> v = Split(data, '|');
    if (v.empty()) {
        // QMessageBox::information(this, "vector size zero", "empty");
        return;
    }
    v.erase(v.end());
    if (v.size() < struct_vector.size()) {
        // QMessageBox::information(this, "Split size wrong", data.c_str());
        return;
    }
    std::string topic;
    std::string exchange;
    std::string product;
    std::string bid1;
    std::string bsize1;
    std::string ask1;
    std::string asize1;
    std::string lastprice;
    std::string volume;
    std::string turnover;
    std::string openinterest;
    std::string time;
    std::string nsec;
    std::string source;
    double numeric_bid1;
    double numeric_bsize1;
    double numeric_ask1;
    double numeric_asize1;
    double numeric_lastprice;
    double numeric_volume;
    double numeric_turnover;
    double numeric_openinterest;
    double numeric_time;
    double numeric_nsec;
    double numeric_mid;
    topic = v[0];
    if (topic == "Full") {
        topic       = v[0];
        exchange    = v[1];
        product     = v[2];
        bid1        = v[3];
        bsize1      = v[4];
        ask1        = v[5];
        asize1      = v[6];
        lastprice   = v[7];
        volume      = v[8];
        turnover    = v[9];
        openinterest= v[10];
        time        = v[11];
        nsec        = v[12];
        source      = v[13];

        numeric_bid1         = atof(v[3].c_str());
        numeric_bsize1       = atof(v[4].c_str());
        numeric_ask1         = atof(v[5].c_str());
        numeric_asize1       = atof(v[6].c_str());
        numeric_lastprice    = atof(v[7].c_str());
        numeric_mid = 0.5*numeric_ask1 + 0.5*numeric_bid1;

    } else {
        // QMessageBox::information(this, "Unknown topic", data.c_str());
        return;
    }

    int model_no;
    if (p_d.find(product) == p_d.end()) {
        model_no = model_num;
    } else {
        model_no = p_d[product];
    }
    QStandardItemModel *m = market_model[model_no];

    if (pre_row[model_no] >= 0) {
       for (int i = 0; i < m->columnCount(); i++) {
          m->item(pre_row[model_no], i)->setForeground(QBrush(QColor(0, 0, 0)));
       }
    }

    if (black_list.contains(exchange)) {
        return;
    }

    if (numeric_bid1 < 0.000001 ||  numeric_ask1 < 0.000001 || numeric_lastprice < 0.000001) {
        // QMessageBox::information(this, "Price format Error", data.c_str());
        return;
    }

    if (exchange_map[model_no].find(exchange) == exchange_map[model_no].end()) {
        exchange_map[model_no][exchange] = ex_count[model_no]+ 1;
        ex_count[model_no] = ex_count[model_no] + 1;
    }
    row[model_no] = exchange_map[model_no][exchange];

    // price_map[model_no][row[model_no]] = price;
    bid_map[model_no][row[model_no]] = numeric_bid1;
    ask_map[model_no][row[model_no]] = numeric_ask1;
    mid_map[model_no][row[model_no]] = numeric_mid;
    content_map[model_no][row[model_no]] = v;

    for (int i = 0; i < m->columnCount(); i++) {
        m->setItem(row[model_no],i,new QStandardItem(QString::fromLocal8Bit(v[i].c_str())));
        m->item(row[model_no],i)->setForeground(QBrush(QColor(255, 0, 0)));
    }
    pre_row[model_no] = row[model_no];

    if (row[model_no] == high_bid_row[model_no]) {
        int max_row = -1;
        double max_price = -1.0;
        for (std::tr1::unordered_map<int, double>::iterator it = bid_map[model_no].begin(); it != bid_map[model_no].end(); it++) {
            if (it->second > max_price) {
                max_price = it->second;
                max_row = it->first;
            }
        }
        high_bid[model_no] = max_price;
        UpdateHighLowWindow(model_no, 0, max_row);
    }
    if (row[model_no] == low_ask_row[model_no]) {
        int min_row = -1;
        double min_price = 1000000;
        for (std::tr1::unordered_map<int, double>::iterator it = ask_map[model_no].begin(); it != ask_map[model_no].end(); it++) {
            if (it->second < min_price) {
                min_price = it->second;
                min_row = it->first;
            }
        }
        low_ask[model_no] = min_price;
        UpdateHighLowWindow(model_no, 1, min_row);
    }
    if (row[model_no] != low_ask_row[model_no] && row[model_no] != high_bid_row[model_no]) {
        if (numeric_bid1 > high_bid[model_no]) {
            high_bid[model_no] = numeric_bid1;
            high_bid_row[model_no] = row[model_no];
            UpdateHighLowWindow(model_no, 0, row[model_no]);
        }
        if (numeric_ask1 < low_ask[model_no]) {
            low_ask[model_no] = numeric_ask1;
            low_ask_row[model_no] = row[model_no];
            UpdateHighLowWindow(model_no, 1, row[model_no]);
        }
    }
}


void MainWindow::UpdateHighLowWindow(int no, int line, int r) {
    std::vector<std::string> temp_v = content_map[no][r];
    int show_line = line + 4*no;
    std::string topic       = temp_v[0];
    std::string exchange    = temp_v[1];
    std::string product     = temp_v[2];
    std::string bid1        = temp_v[3];
    std::string bsize1      = temp_v[4];
    std::string ask1        = temp_v[5];
    std::string asize1      = temp_v[6];
    std::string lastprice   = temp_v[7];
    std::string volume      = temp_v[8];
    std::string turnover    = temp_v[9];
    std::string openinterest= temp_v[10];
    std::string time        = temp_v[11];
    std::string nsec        = temp_v[12];
    std::string source      = temp_v[13];

    std::string standard_product;
    if (p_d.find(product) == p_d.end()) {
        standard_product = product;
    } else {
        standard_product = show_symbol[p_d[product]];
    }

    std::string show_price;
    if (line == 0) {
        show_price = bid1;
    } else if (line == 1) {
        show_price = ask1;
    } else {
        // QMessageBox::information(this, "Wrong line", exchange.c_str());
        return;
    }

    // QStandardItemModel* m = highlow_model[no];
    QStandardItemModel* m = highlow_assemble_model;
    m->setItem(show_line,0,new QStandardItem(QString::fromLocal8Bit(standard_product.c_str())));
    m->setItem(show_line,1,new QStandardItem(QString::fromLocal8Bit(exchange.c_str())));
    m->setItem(show_line,2,new QStandardItem(QString::fromLocal8Bit(show_price.c_str())));
    m->setItem(show_line,3,new QStandardItem(QString::fromLocal8Bit(time.c_str())));
    // QMessageBox::information(this, "MarketData", exchange.c_str());
    UpdateDeltaWindow(no);
}

void MainWindow::UpdateDeltaWindow(int no) {
    int show_line = no*4 + 2;
    // QStandardItemModel* m = highlow_model[no];
    QStandardItemModel* m = highlow_assemble_model;
    if (high_bid[no] < 0 || low_ask[no] > 9999999) {
        m->setItem(show_line,0,new QStandardItem(QString::fromLocal8Bit("NULL")));
        m->setItem(show_line,1,new QStandardItem(QString::fromLocal8Bit("NULL")));
        m->setItem(show_line,2,new QStandardItem(QString::fromLocal8Bit("NULL")));
        m->setItem(show_line,3,new QStandardItem(QString::fromLocal8Bit("NULL")));
        //m->setItem(show_line,4,new QStandardItem(QString::fromLocal8Bit("NULL")));
        return;
    }
    double delta_price = high_bid[no] - low_ask[no];
    char delta_price_str[16];
    char delta_percentage_str[16];
    char delta_show_str[32];
    snprintf(delta_price_str, sizeof(delta_price_str), "%.3f", delta_price);
    snprintf(delta_percentage_str, sizeof(delta_percentage_str), "%.3f", delta_price*1.0/low_ask[no]*100);
    snprintf(delta_show_str, sizeof(delta_show_str), "%.3f(%s%)", delta_price, delta_percentage_str);
    std::vector<std::string> high_v = content_map[no][high_bid_row[no]];
    std::vector<std::string> low_v = content_map[no][low_ask_row[no]];
    std::string hightime_str = high_v[11];
    std::string lowtime_str = low_v[11];

    char delta_time_str[16];

    int hightime_sec = Translate(hightime_str);
    int lowtime_sec = Translate(lowtime_str);
    if (hightime_sec == -1 || lowtime_sec == -1) {
        snprintf(delta_time_str, sizeof(delta_time_str), "%s", "NULL");
    } else {
        snprintf(delta_time_str, sizeof(delta_time_str), "%d", hightime_sec-lowtime_sec);
    }

    m->setItem(show_line,0,new QStandardItem(QString::fromLocal8Bit("NULL")));
    m->setItem(show_line,1,new QStandardItem(QString::fromLocal8Bit("NULL")));
    m->setItem(show_line,2,new QStandardItem(QString::fromLocal8Bit(delta_price_str)));
    m->setItem(show_line,3,new QStandardItem(QString::fromLocal8Bit(delta_time_str)));

    m->setItem(show_line+1,0,new QStandardItem(QString::fromLocal8Bit("NULL")));
    m->setItem(show_line+1,1,new QStandardItem(QString::fromLocal8Bit("NULL")));
    m->setItem(show_line+1,2,new QStandardItem(QString::fromLocal8Bit(delta_percentage_str)));
    m->setItem(show_line+1,3,new QStandardItem(QString::fromLocal8Bit("NULL")));
    for (int i = 0; i < m->columnCount(); i++) {
        m->item(show_line+1,i)->setForeground(QBrush(QColor(255, 0, 0)));
    }

}

MainWindow::~MainWindow()
{
    delete ui;
    for (int i = 0; i < model_num+1; i++) {
        delete market_model[i];
        // delete highlow_model[i];
    }
    delete highlow_assemble_model;
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

    ClearVariable();
    for (int i = 0; i < model_num + 1; i++) {
        market_model[i] = InitMarketModel(market_model[i]);
    }

    highlow_assemble_model = InitHighlowModel(highlow_assemble_model);
    marketview->setModel(market_model[showmodel_no]);
    highlowview->setModel(highlow_assemble_model);
    ui->comboBox->setCurrentIndex(showmodel_no);
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    if (index > model_num) {
        QMessageBox::information(this, "ERROR", "no this option");
        return;
    }
    marketview->setModel(market_model[index]);
    // highlowview->setModel(highlow_model[index]);
}
