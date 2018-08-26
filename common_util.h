#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H

#include <tr1/unordered_map>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <algorithm>

#include "market_snapshot.h"

std::vector<std::string> Split(std::string raw_string, char split_char=' ');
std::string ExtractString(std::string s, char rm_char=' ');
bool CheckAddressLegal(std::string address);
bool CheckTimeStringLegal(std::string time);
void Register(std::tr1::unordered_map<std::string, int>* m, std::string contract, int no);
int Translate(std::string time);

template <class T>
bool CheckVSize(std::vector<T>v, int size) {
  int real_size = v.size();
  if (real_size == size) {
    return true;
  } else {
    printf("check vector size failed, size if %d, require_size is %d\n", real_size, size);
    // PrintVector(v);
    return false;
  }
}

MarketSnapshot HandleSnapshot(std::string raw_shot);

#endif // COMMON_UTIL_H
