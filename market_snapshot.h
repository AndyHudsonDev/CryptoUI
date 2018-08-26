#ifndef MARKET_SNAPSHOT_H
#define MARKET_SNAPSHOT_H

#include <stdio.h>
#include <sys/time.h>

#include <algorithm>

#define MARKET_DATA_DEPTH 5
#define MAX_TICKER_LENGTH 32

struct MarketSnapshot {
  char ticker[MAX_TICKER_LENGTH];
  double bids[MARKET_DATA_DEPTH];
  double asks[MARKET_DATA_DEPTH];
  double bid_sizes[MARKET_DATA_DEPTH];
  double ask_sizes[MARKET_DATA_DEPTH];
  double last_trade;
  double last_trade_size;
  int volume;
  double turnover;
  double open_interest;
  bool is_trade_update;  // true if updated caused by trade
  timeval time;

  bool is_initialized;

  MarketSnapshot()
      : last_trade(0.0),
        last_trade_size(0),
        volume(0),
        turnover(0.0),
        open_interest(0.0),
        is_trade_update(false),
        time(),
        is_initialized(false) {
    ticker[0] = 0;
    for (int i = 0; i < MARKET_DATA_DEPTH; ++i) {
      bids[i] = 0.0;
      asks[i] = 0.0;
      bid_sizes[i] = 0;
      ask_sizes[i] = 0;
    }
  }

  void Show(FILE* stream, int depth = MARKET_DATA_DEPTH) const {
    fprintf(stream, "%ld %06ld SNAPSHOT %s |",
            time.tv_sec, time.tv_usec, ticker);

    int n = std::min(depth, MARKET_DATA_DEPTH);
    for (int i = 0; i < n; ++i) {
      fprintf(stream, " %.12g %.12g | %.12f x %.12f |",
              bids[i], asks[i], bid_sizes[i], ask_sizes[i]);
    }

    if (is_trade_update) {

      // T for trade update
      fprintf(stream, " %.12g %d %d T\n", last_trade, last_trade_size, volume);
    } else {
      // M for market update
      fprintf(stream, " %.12g %d %d M %.12g %.12g\n", last_trade, last_trade_size, volume, turnover, open_interest);
    }
  }
};
#endif // MARKET_SNAPSHOT_H
