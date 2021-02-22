# CryptoUI


### brief introduction:
This is the UI code for cryptocurrency market_data, included exchanges: huobi, okex, binance, polenix, kraken.
it can be used to do simple arbtriage between several exchanges.
when highest_bid > lowest_ask, there is a no-risk oppounity(logically): buy in lowest_ask and sell in highest_bid.


### system:
* windows

it need a backend program, code is here https://github.com/nickhuangxinyu/cryptobackend/edit/master/README.md
you can just use mine(i have a server running on the backend, it's a us server, so the ui maybe slow to update due to the bad network)

### run command:
unzip the demo.rar,  double click trading-ui.exe, you will see the ui
first, you should click the connect button, it set the backend address you want to link.
you can just select mimas, its a running server(amazon cloud server). it may be slow since the us-china network is sometimes bad. 
Or, you can run backend program in anywhere, and fill the address by clicking self-define. 
Then, you click start, the window will start to receive market data.

### Function explaination:
the downmost window calculate the highest bid price and lowest ask price for one product in serveral exchanges.
when highest_bid > lowest_ask, there is a no-risk oppounity(logically): buy in lowest_ask and sell in highest_bid.

### Author:
XinYu Huang

any questions or suggestions are welcome, please contract me with:huangxy17@fudan.edu.cn, i will list your name here to thanks for
your contribution.

### Thanks list:


### Market_data format:
any data in the following format are supported by this UI.
"Topic|Exchange|product|bid1|bsize1|ask1|asize1|lastprice|volume|turnover|openinterest|time|nsec|source|end"
