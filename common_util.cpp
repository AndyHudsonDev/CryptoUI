#include "common_util.h"


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
