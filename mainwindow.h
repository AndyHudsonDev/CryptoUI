#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "view.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QAbstractTableModel>
#include <QMdiArea>

#include <QApplication>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>

#include <zmq.h>
#include <zmq.hpp>
#include <tr1/unordered_map>

#include "connectwindow.h"
#include "common_util.h"


class MdiChild;
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
class QSignalMapper;
class QAbstractItemModel;
class QSortFilterProxyModel;
class QTreeView;

class LoginForm;
class ChpwdForm;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(ConnectWindow *win);
    ~MainWindow();

signals:
    void StartClicked();
    void ConnectClicked();
    void ReStart();
    void MainWindowDisconnect();

public slots:
    void marketUpdate(std::string data);
    void OnDisconnectDone();
    void OnMimasConnectDone();
    void OnLimitConnectDone();
    void OnSelfDefineConnectDone();
    void OnSocketRecvStarted();



private slots:
    void on_connect_clicked();

    void on_start_clicked();

    void on_exit_clicked();


    void openMarket();
    void openHighLow();

    void on_disconnect_clicked();

    void on_clear_clicked();

private:
    void UpdateHigh(std::vector<std::string> v);
    void UpdateLow(std::vector<std::string> v);
    void UpdateDelta();
    void UpdateHighLowWindow(int line, int r);
    void UpdateDeltaWindow();
    Ui::MainWindow *ui;
    zmq::socket_t* socket;
    QMdiArea *mdiArea;
    std::tr1::unordered_map<std::string, int> exchange_map;
    int ex_count;
    int row;
    double high;
    double low;
    int high_row;
    int low_row;
    QMdiSubWindow* MarketWindow;
    QMdiSubWindow* HighLowWindow;
    QStandardItemModel* market_model;
    QStandardItemModel* highlow_model;
    std::tr1::unordered_map<int, double> price_map;
    std::tr1::unordered_map<int, std::vector<std::string>> content_map;
    QList<std::string> black_list;
    ConnectWindow* cw;
    bool socket_connected;
    bool socket_recv_started;
};

#endif // MAINWINDOW_H
