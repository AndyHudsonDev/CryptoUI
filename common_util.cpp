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
