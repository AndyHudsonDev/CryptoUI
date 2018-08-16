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
