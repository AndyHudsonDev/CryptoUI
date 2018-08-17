#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H

#include <tr1/unordered_map>
#include <string>
#include <vector>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <algorithm>


std::vector<std::string> Split(std::string raw_string, char split_char);
bool CheckAddressLegal(std::string address);
bool CheckTimeStringLegal(std::string time);
void Register(std::tr1::unordered_map<std::string, int>* m, std::string contract, int no);
int Translate(std::string time);

#endif // COMMON_UTIL_H
