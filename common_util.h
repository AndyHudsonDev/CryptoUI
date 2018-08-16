#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H

#include <vector>
#include <string>


std::vector<std::string> Split(std::string raw_string, char split_char);
bool CheckAddressLegal(std::string address);
bool CheckTimeStringLegal(std::string time);
int Translate(std::string time);

#endif // COMMON_UTIL_H
