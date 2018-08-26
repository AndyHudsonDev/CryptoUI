#include "common_util.h"

std::string ExtractString(std::string s, char rm_char) {
  if (s.empty()) {
    return s;
  }
  int first_content_pos = -1;
  int last_content_pos = -1;
  for (unsigned int i = 0; i < s.size(); i++) {
    if (s[i] != rm_char) {
      first_content_pos = i;
      break;
    }
  }
  if (first_content_pos == -1) {
    return "";
  }
  for (unsigned int i = s.size()-1; i >= 0; i--) {
    if (s[i] != rm_char) {
      last_content_pos = i;
      break;
    }
  }
  // printf("first last is %d %d\n", first_content_pos, last_content_pos);
  return s.substr(first_content_pos, last_content_pos-first_content_pos+1);
}

std::vector<std::string> Split(std::string raw_string, char split_char) {
    std::vector<std::string> result;
    int pos = -1;
    for (unsigned int i = 0; i < raw_string.size(); i++) {
      if (raw_string[i] == split_char) {
        std::string substr = ExtractString(raw_string.substr(pos+1, i-pos-1));
        if (!substr.empty()) {
          result.push_back(substr);
        }
        pos = i;
      }
    }
    std::string left_str = ExtractString(raw_string.substr(pos+1, raw_string.size()-pos));
    if (!left_str.empty()) {
      result.push_back(left_str);
    }
    return result;
}


void Register(std::tr1::unordered_map<std::string, int>* m, std::string contract, int no) {
  std::vector<std::string> connect_v;
  connect_v.push_back("-");
  connect_v.push_back("_");
  connect_v.push_back("");
  std::vector<std::string> v = Split(contract, '-');
  if (v.size() < 2) {
    printf("split size wrong!\n");
    exit(1);
  }
  std::string raw1 = v[0];
  std::string raw2 = v[1];
  std::string low1 = raw1;
  std::string low2 = raw2;
  std::string up1 = raw1;
  std::string up2 = raw2;
  transform(low1.begin(), low1.end(), low1.begin(), ::tolower);
  transform(low2.begin(), low2.end(), low2.begin(), ::tolower);
  transform(up1.begin(), up1.end(), up1.begin(), ::toupper);
  transform(up2.begin(), up2.end(), up2.begin(), ::toupper);
  for (int i = 0; i < connect_v.size(); i++) {
    if ((connect_v[i] == "X" || connect_v[i] == "Z") && (low1 != "btc" && low1 != "usd")) {
      continue;
    }
    (*m)[raw1+connect_v[i]+raw2] = no;
    (*m)[raw2+connect_v[i]+raw1] = no;
    (*m)[low1+connect_v[i]+low2] = no;
    (*m)[low2+connect_v[i]+low1] = no;
    (*m)[up1+connect_v[i]+up2] = no;
    (*m)[up2+connect_v[i]+up1] = no;

    if (up1 == "USD") {
      std::string temp1 = "USDT";
      std::string temp2 = "usdt";
      (*m)[temp2+connect_v[i]+low2] = no;
      (*m)[low2+connect_v[i]+temp2] = no;
      (*m)[temp1+connect_v[i]+up2] = no;
      (*m)[up2+connect_v[i]+temp1] = no;
    } else if (up2 == "USD") {
      std::string temp1 = "USDT";
      std::string temp2 = "usdt";
      (*m)[low1+connect_v[i]+temp2] = no;
      (*m)[temp2+connect_v[i]+low1] = no;
      (*m)[temp1+connect_v[i]+up1] = no;
      (*m)[up1+connect_v[i]+temp1] = no;
      std::string temp3 = "ZUSDT";
      (*m)[up1+connect_v[i]+temp3] = no;
    }

    if (up1 == "BTC") {
      std::string temp = "XXBT";
      if (connect_v[i] == "") {
        (*m)[temp+connect_v[i]+up2] = no;
        (*m)[up2+connect_v[i]+temp] = no;
      }
    } else if (up2 == "BTC") {
      std::string temp = "XXBT";
      if (connect_v[i] == "") {
        (*m)[up1+connect_v[i]+temp] = no;
        (*m)[temp+connect_v[i]+up1] = no;
      }
    }
  }
}

bool CheckAddressLegal(std::string address) {
    if (address.size() < 7) {
        return false;
    }
    if (address.substr(3,3) != "://") {
        return false;
    }
    return true;
}

bool CheckTimeStringLegal(std::string time) {
    if (time.size() < 5 || time.size() > 8) {
        return false;
    }
    std::vector<std::string> time_v = Split(time, ':');
    if (time_v.size() != 3) {
        return false;
    }
    int hour = atoi(time_v[0].c_str());
    int min = atoi(time_v[1].c_str());
    int sec = atoi(time_v[2].c_str());
    if (hour > 23 || hour < 0 || min > 59 || min < 0 || sec>59 || sec < 0) {
        return false;
    }
    return true;
}

int Translate(std::string time) {
    if (time.size() < 5 || time.size() > 8) {
        return -1;
    }
    std::vector<std::string> time_v = Split(time, ':');
    if (time_v.size() != 3) {
        return -1;
    }
    int hour = atoi(time_v[0].c_str());
    int min = atoi(time_v[1].c_str());
    int sec = atoi(time_v[2].c_str());
    if (hour > 23 || hour < 0 || min > 59 || min < 0 || sec>59 || sec < 0) {
        return -1;
    }
    return hour*3600 + min*60 + sec;

}

MarketSnapshot HandleSnapshot(std::string raw_shot) {
  MarketSnapshot shot;
  std::vector<std::string> content = Split(raw_shot, '|');
  if (!CheckVSize(content, 12)) {
    return shot;
  }

  std::vector<std::string> time_content = Split(content[0], ' ');
  if (!CheckVSize(time_content, 4)) {
    return shot;
  }
  std::string unix_sec = time_content[0];
  std::string unix_usec = time_content[1];
  std::string topic = time_content[2];
  std::string ticker = time_content[3];

  // printf("unix_sec=%s, unix_usec=%s, topic=%s, ticker=%s\n", unix_sec.c_str(), unix_usec.c_str(), topic.c_str(), ticker.c_str());

  std::vector<std::string> bidask_price[5];
  std::vector<std::string> bidask_size[5];
  std::string bid_price[5];
  std::string ask_price[5];
  std::string bid_size[5];
  std::string ask_size[5];
  for (int i = 0; i < 5; i++) {
    bidask_price[i] = Split(content[1+2*i]);
    if (!CheckVSize(bidask_price[i], 2)) {
      return shot;
    }
    bid_price[i] = bidask_price[i][0];
    ask_price[i] = bidask_price[i][1];
    bidask_size[i] = Split(content[2+2*i]);
    if (!CheckVSize(bidask_size[i], 3)) {
      return shot;
    }
    bid_size[i] = bidask_size[i][0];
    ask_size[i] = bidask_size[i][2];
    // printf("bid[%d]=%s, bid_size[%d]=%s, ask[%d]=%s, ask_size[%d]=%s\n", i, bid_price[i].c_str(), i, bid_size[i].c_str(), i, ask_price[i].c_str(), i, ask_size[i].c_str());
  }

  std::vector<std::string> other_content = Split(content[11]);
  if (!CheckVSize(other_content, 6)) {
    return shot;
  }
  std::string last_price = other_content[0];
  std::string last_size = other_content[1];
  std::string volume = other_content[2];
  std::string trade_topic = other_content[3];
  std::string turnover = other_content[4];
  std::string open_interest = other_content[5];

  // printf("lastprice=%s, lastsize=%s, volume=%s, trade_topic=%s, turnover=%s, open_interest=%s\n", last_price.c_str(), last_size.c_str(), volume.c_str(), trade_topic.c_str(), turnover.c_str(), open_interest.c_str());

  shot.time.tv_sec = atoi(unix_sec.c_str());
  shot.time.tv_usec = atoi(unix_usec.c_str());

  snprintf(shot.ticker, sizeof(shot.ticker), "%s", ticker.c_str());

  for (int i = 0; i < 5; i++) {
      shot.bids[i] = atof(bid_price[i].c_str());
      shot.bid_sizes[i] = atof(bid_size[i].c_str());
      shot.asks[i] = atof(ask_price[i].c_str());
      shot.ask_sizes[i] = atof(ask_size[i].c_str());
    }

    shot.last_trade = atof(last_price.c_str());
    shot.last_trade_size = atof(last_size.c_str());
    shot.volume = atof(volume.c_str());
    shot.turnover = atof(turnover.c_str());
    shot.open_interest = atof(open_interest.c_str());
    return shot;
}
