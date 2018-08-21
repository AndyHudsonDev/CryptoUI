#include "mainwindow.h"
#include "stream.h"
#include "connectwindow.h"
#include <QApplication>
#include <QObject>
#include <tr1/unordered_map>
#include "common_util.h"
//#include <libconfig.h++>

//using namespace libconfig;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    std::tr1::unordered_map<std::string, int> pd;
    std::vector<std::string> product_v;
    product_v.push_back("BTC-USD");
    product_v.push_back("ETH-USD");
    product_v.push_back("ETH-BTC");
    product_v.push_back("XRP-BTC");
    product_v.push_back("LTC-USD");
    product_v.push_back("LTC-BTC");
    product_v.push_back("XRP-USD");

    product_v.push_back("ETC-USD");
    product_v.push_back("ETC-BTC");
    product_v.push_back("ETC-ETH");
    product_v.push_back("EOS-USD");
    product_v.push_back("EOS-BTC");

    product_v.push_back("BCH-USD");
    product_v.push_back("BCH-BTC");

    product_v.push_back("TRX-USD");
    product_v.push_back("TRX-BTC");
    product_v.push_back("TRX-ETH");

    product_v.push_back("NEO-BTC");
    product_v.push_back("NEO-USD");

    product_v.push_back("XLM-BTC");
    product_v.push_back("ADA-BTC");
    product_v.push_back("IOTA-BTC");
    product_v.push_back("ZEC-BTC");
    product_v.push_back("QTUM-BTC");
    product_v.push_back("DASH-BTC");
    product_v.push_back("OMG-BTC");




    for (int i = 0; i < product_v.size(); i++) {
        Register(&pd, product_v[i].c_str(), i);
    }
    pd["XXBTZUSD"] = 0;
    pd["XXRPXXBT"] = 3;
    pd["XLTCXXBT"] = 5;
    pd["XLTCZUSD"] = 4;
    pd["XETHZUSD"] = 1;
    pd["XETHXXBT"] = 2;
    pd["XETCXXBT"] = 8;
    pd["XETCZUSD"] = 7;
    pd["XETCXETH"] = 9;
    pd["ETH_ETC"] = 9;
    /*
    Register(&pd, "BTC-USD", 0);
    Register(&pd, "ETH-USD", 1);
    Register(&pd, "ETH-BTC", 2);
    Register(&pd, "XRP-BTC", 3);
    */
    /*
    pd["XBT-USD"] = 0;
    pd["BTC-USDT"] = 0;
    pd["BTC-USD"] = 0;
    pd["USDT-BTC"] = 0;
    pd["btcusdt"] = 0;
    pd["XXBTZUSD"] = 0;
    pd["btc_usd"] = 0;
    pd["btc_usdt"] = 0;
    pd["BTCUSDT"] = 0;

    pd["ETH-USD"] = 1;
    pd["USD-ETH"] = 1;
    pd["ETH-USDT"] = 1;
    pd["USDT-ETH"] = 1;
    pd["ETH-USDT"] = 1;
    pd["ethusdt"] = 1;
    pd["XETHZUSD"] = 1;
    pd["eth_usd"] = 1;
    pd["eth_usdt"] = 1;
    pd["ETHUSDT"] = 1;

    pd["ETH-BTC"] = 2;
    pd["BTC-ETH"] = 2;
    pd["ETH-XBT"] = 2;
    pd["ethbtc"] = 2;
    pd["eth_btc"] = 2;
    pd["XETHXXBT"] = 2;
    pd["ETHBTC"] = 2;

    pd["XRP-BTC"] = 3;
    pd["BTC-XRP"] = 3;
    pd["xrpbtc"] = 3;
    pd["xrp_btc"] = 3;
    pd["XXRPXXBT"] = 3;
    pd["XRPBTC"] = 3;

    pd["LTC-USD"]  = 4;
    pd["LTC-USDT"] = 4;
    pd["LTC-USD"]  = 4;
    pd["USDT-LTC"] = 4;
    pd["ltcusdt"]  = 4;
    pd["XLTCZUSD"] = 4;
    pd["ltc_usd"]  = 4;
    pd["ltc_usdt"] = 4;
    pd["LTCUSDT"]  = 4;

    pd["LTC-USD"]  = 4;
    pd["LTC-USDT"] = 4;
    pd["LTC-USD"]  = 4;
    pd["USDT-LTC"] = 4;
    pd["ltcusdt"]  = 4;
    pd["XLTCZUSD"] = 4;
    pd["ltc_usd"]  = 4;
    pd["ltc_usdt"] = 4;
    pd["LTCUSDT"]  = 4;
    */


    stream s;
    ConnectWindow cw;
    MainWindow w(&cw, pd, product_v);

    QObject::connect(&cw, SIGNAL(DisconnectAll()), &s, SLOT(OnDisconnectALLRequest()));
    QObject::connect(&cw, SIGNAL(MimasSocketConnect()), &s, SLOT(OnMimasConnectRequest()));
    QObject::connect(&cw, SIGNAL(LimitSocketConnect()), &s, SLOT(OnLimitConnectRequest()));
    QObject::connect(&cw, SIGNAL(SelfDefineSocketConnect(std::string)), &s, SLOT(OnSelfDefineConnectRequest(std::string)));

    QObject::connect(&s, SIGNAL(SocketDisconnectDone()), &w, SLOT(OnDisconnectDone()));
    QObject::connect(&s, SIGNAL(MimasConnectDone()), &w, SLOT(OnMimasConnectDone()));
    QObject::connect(&s, SIGNAL(LimitConnectDone()), &w, SLOT(OnLimitConnectDone()));
    QObject::connect(&s, SIGNAL(SelfDefineConnectDone()), &w, SLOT(OnSelfDefineConnectDone()));

    QObject::connect(&s, SIGNAL(SocketRecvStarted()), &w, SLOT(OnSocketRecvStarted()));
    QObject::connect(&w, SIGNAL(MainWindowDisconnect()), &s, SLOT(OnDisconnectALLRequest()));

    QObject::connect(&w, SIGNAL(StartClicked()), &s, SLOT(Start()));
    QObject::connect(&s, SIGNAL(DataUpdate(std::string)), &w, SLOT(marketUpdate(std::string)));

    w.show();

    return a.exec();
}
