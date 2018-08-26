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
    show_symbol.push_back("Other");
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
    double m_rate = 2.0/5;
    MarketWin->resize(width, m_rate*height);
    HighlowWin->resize(width, (1-m_rate)*height);



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
    std::vector<std::string> column_title;
    column_title.push_back("Product");
    column_title.push_back("Delta%");
    column_title.push_back("Highest_bid");
    column_title.push_back("High_ex");
    column_title.push_back("Lowest_ask");
    column_title.push_back("Low_ex");
    column_title.push_back("Delta");
    column_title.push_back("Time_diff");
    column_title.push_back("Time_high");
    column_title.push_back("Time_low");

    m = new QStandardItemModel();
    m->setColumnCount(column_title.size());

    for (int i = 0; i < column_title.size(); i++) {
        m->setHeaderData(i,Qt::Horizontal,QString::fromLocal8Bit(column_title[i].c_str()));
    }

    for (int i = 0; i < show_symbol.size(); i++) {
        for (int j = 0; j < column_title.size(); j++) {
            m->setItem(i, j, new QStandardItem(QString::fromLocal8Bit("NULL")));
        }
        m->setItem(i, 0, new QStandardItem(QString::fromLocal8Bit(show_symbol[i].c_str())));
    }

    return m;
}


void MainWindow::marketUpdate(std::string data) {
    //QMessageBox::information(this, "MarketData", data.c_str());
    MarketSnapshot shot = HandleSnapshot(data);
    std::string exchange = Split(shot.ticker, '/')[0];
    std::string topic       = "SNAPSHOT";

    std::string product     = Split(shot.ticker, '/')[0];

    double numeric_bid1         = shot.bids[0];
    double numeric_bsize1       = shot.bid_sizes[0];
    double numeric_ask1         = shot.asks[0];
    double numeric_asize1       = shot.ask_sizes[0];
    double numeric_lastprice    = shot.last_trade;
    double numeric_lastsize     = shot.last_trade_size;
    double numeric_volume       = shot.volume;
    double numeric_turnover     = shot.turnover;
    double numeric_openinterest = shot.open_interest;
    double numeric_mid = 0.5*numeric_ask1 + 0.5*numeric_bid1;

    char bid1[16];
    char bsize1[16];
    char ask1[16];
    char asize1[16];
    char lastprice[16];
    char lastsize[16];
    char volume[16];
    char turnover[16];
    char openinterest[16];
    char time[16];
    char nsec[16];
    char source[16];
    snprintf(bid1, sizeof(bid1), "%lf", numeric_bid1);
    snprintf(bsize1, sizeof(bsize1), "%lf", numeric_bsize1);
    snprintf(ask1, sizeof(ask1), "%lf", numeric_ask1);
    snprintf(asize1, sizeof(asize1), "%lf", numeric_asize1);
    snprintf(lastprice, sizeof(lastprice), "%lf", numeric_lastprice);
    snprintf(lastsize, sizeof(lastsize), "%d", numeric_lastsize);
    snprintf(volume, sizeof(volume), "%lf", numeric_volume);
    snprintf(turnover, sizeof(turnover), "%lf", numeric_turnover);
    snprintf(openinterest, sizeof(openinterest), "%lf", numeric_openinterest);
    snprintf(time, sizeof(time), "%lf", shot.time.tv_sec);
    std::vector<std::string> v;
    v.push_back(topic);
    v.push_back(exchange);
    v.push_back(product);
    v.push_back(bid1);
    v.push_back(bsize1);
    v.push_back(ask1);
    v.push_back(asize1);
    v.push_back(lastprice);
    v.push_back(volume);
    v.push_back(turnover);
    v.push_back(openinterest);
    v.push_back(time);
    v.push_back(nsec);
    v.push_back(source);
    /*
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
    */

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
    content_map[model_no][row[model_no]] = shot;

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
        // UpdateHighLowWindow(model_no, 0, max_row);
        UpdateHighLow(model_no, "high");
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
        // UpdateHighLowWindow(model_no, 1, min_row);
        UpdateHighLow(model_no, "low");
    }
    if (row[model_no] != low_ask_row[model_no] && row[model_no] != high_bid_row[model_no]) {
        if (numeric_bid1 > high_bid[model_no]) {
            high_bid[model_no] = numeric_bid1;
            high_bid_row[model_no] = row[model_no];
            // UpdateHighLowWindow(model_no, 0, row[model_no]);
            UpdateHighLow(model_no, "high");
        }
        if (numeric_ask1 < low_ask[model_no]) {
            low_ask[model_no] = numeric_ask1;
            low_ask_row[model_no] = row[model_no];
            // UpdateHighLowWindow(model_no, 1, row[model_no]);
            UpdateHighLow(model_no, "low");
        }
    }
}

void MainWindow::UpdateHighLow(int row, std::string topic) {
    QStandardItemModel* m = highlow_assemble_model;
    if (topic == "high") {
        MarketSnapshot shot = content_map[row][high_bid_row[row]];
        char high_bid[16];
        char high_bid_size[16];
        char time_sec[16];
        snprintf(high_bid, sizeof(high_bid), "%lf", shot.bids[0]);
        snprintf(high_bid_size, sizeof(high_bid_size), "%lf", shot.bid_sizes[0]);
        snprintf(time_sec, sizeof(time_sec), "%d", shot.time.tv_sec);
        m->setItem(row, 2, new QStandardItem(QString::fromLocal8Bit(high_bid)));
        m->setItem(row, 3, new QStandardItem(QString::fromLocal8Bit(high_bid_size)));
        m->setItem(row, 8, new QStandardItem(QString::fromLocal8Bit(time_sec)));
    } else if (topic == "low") {
        MarketSnapshot shot = content_map[row][high_bid_row[row]];
        char low_ask[16];
        char low_ask_size[16];
        char time_sec[16];
        snprintf(low_ask, sizeof(low_ask), "%lf", shot.asks[0]);
        snprintf(low_ask_size, sizeof(low_ask_size), "%lf", shot.ask_sizes[0]);
        snprintf(time_sec, sizeof(time_sec), "%d", shot.time.tv_sec);
        m->setItem(row, 4, new QStandardItem(QString::fromLocal8Bit(low_ask)));
        m->setItem(row, 5, new QStandardItem(QString::fromLocal8Bit(low_ask_size)));
        m->setItem(row, 9, new QStandardItem(QString::fromLocal8Bit(time_sec)));
    } else {
        QMessageBox::information(this, "Wrong topic for high low", topic.c_str());
        exit(1);
    }
    UpdateDelta(row);
}

void MainWindow::UpdateDelta(int row) {
    QStandardItemModel* m = highlow_assemble_model;
    std::string high_bid_str = m->item(row,2)->text().toStdString();
    // QMessageBox::information(this, "highbid str", high_bid_str.c_str());
    std::string low_ask_str = m->item(row,4)->text().toStdString();
    std::string high_time_str = m->item(row,8)->text().toStdString();
    std::string low_time_str = m->item(row,9)->text().toStdString();
    if (high_bid_str == "NULL" || low_ask_str == "NULL" || high_time_str == "NULL" || low_time_str == "NULL") {
        return;
    }
    double high_bid = atof(high_bid_str.c_str());
    double low_ask = atof(low_ask_str.c_str());
    char delta_str[16];
    snprintf(delta_str, sizeof(delta_str), "%lf", high_bid - low_ask);
    // double delta_percent = delta*1.0/low_ask;
    char delta_percent_str[16];
    snprintf(delta_percent_str, sizeof(delta_percent_str), "%lf", (high_bid-low_ask)*100.0/low_ask);
    int high_time_int = Translate(high_time_str);
    int low_time_int = Translate(low_time_str);
    char time_diff[16];
    snprintf(time_diff, sizeof(time_diff), "%d", high_time_int-low_time_int);

    m->setItem(row, 1, new QStandardItem(QString::fromLocal8Bit(delta_percent_str)));
    m->setItem(row, 6, new QStandardItem(QString::fromLocal8Bit(delta_str)));
    m->setItem(row, 7, new QStandardItem(QString::fromLocal8Bit(time_diff)));
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
