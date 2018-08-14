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

#define NUM_MODEL 32

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
    MainWindow(ConnectWindow *win, std::tr1::unordered_map<std::string, int>pd);
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

    void on_disconnect_clicked();

    void on_clear_clicked();

    void on_comboBox_currentIndexChanged(const QString &arg1);

private:
    void SetModel(QTableView*v, QStandardItemModel*m);
    void InitVariable();
    void UpdateHigh(std::vector<std::string> v);
    void UpdateLow(std::vector<std::string> v);
    void UpdateHighLowWindow(int no, int line, int r);
    void UpdateDeltaWindow(int no);
    QStandardItemModel* InitMarketModel(QStandardItemModel* m);
    QStandardItemModel* InitHighlowModel(QStandardItemModel* m);
    Ui::MainWindow *ui;
    zmq::socket_t* socket;
    QMdiArea *mdiArea;
    std::tr1::unordered_map<int, std::tr1::unordered_map<std::string, int>> exchange_map;
    std::tr1::unordered_map<int, int> ex_count;
    std::tr1::unordered_map<int, int> row;
    std::tr1::unordered_map<int, double> high;
    std::tr1::unordered_map<int, double> low;
    std::tr1::unordered_map<int, int> high_row;
    std::tr1::unordered_map<int, int> low_row;
    QMdiSubWindow* MarketWindow;
    QMdiSubWindow* HighLowWindow;
    QStandardItemModel* market_model[NUM_MODEL];
    QStandardItemModel* highlow_model[NUM_MODEL];
    std::tr1::unordered_map<int, std::tr1::unordered_map<int, double>> price_map;
    std::tr1::unordered_map<int, std::tr1::unordered_map<int, std::vector<std::string>>> content_map;
    QList<std::string> black_list;
    ConnectWindow* cw;
    bool socket_connected;
    bool socket_recv_started;
    std::tr1::unordered_map<std::string, int> p_d;
    std::tr1::unordered_map<std::string, QStandardItemModel*> model_map;
    std::tr1::unordered_map<std::string, QStandardItemModel*> highlow_map;
    std::tr1::unordered_map<int, int> pre_row;
    int model_num;
    int showmodel_no;
};

#endif // MAINWINDOW_H
