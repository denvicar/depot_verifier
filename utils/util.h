#pragma once
#include <string>
#include <unordered_map>
using std::string;
using std::unordered_map;

void ltrim(string&);

void rtrim(string&);

unordered_map<string, string> read_config();