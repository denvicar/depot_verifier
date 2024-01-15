#include "util.h"
#include <string>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <unordered_map>
#include <iostream>

using std::ifstream;
using std::unordered_map;
using std::getline;
using std::string;
using std::find_if;
using std::isspace;
using std::cerr;
using std::make_pair;

void ltrim(string& line) {
    line.erase(line.begin(), find_if(line.begin(), line.end(), [](unsigned char ch) { return !isspace(ch);}));
}

void rtrim(string& line) {
    line.erase(find_if(line.rbegin(), line.rend(), [](unsigned char ch) { return !isspace(ch);}).base(), line.end());
}

unordered_map<string, string> read_config() {
    ifstream conf("depot_verifier.ini");
    unordered_map<string, string> properties;

    if (!conf.is_open()) {
        cerr << "Error while opening config file";
        exit(EXIT_FAILURE);
    }

    string line;
    while (getline(conf, line)) {
        ltrim(line);
        if (line.empty()) continue;
        if (line[0] == ';' || line[0] == '[' || line[0] == '#') continue;
        string key, value;
        size_t equalSign = line.find('=');
        size_t commentSign = 0;
        if (size_t c = line.find(';', equalSign); c != string::npos) commentSign = c;
        key = line.substr(0, equalSign);
        value = line.substr(equalSign + 1, line.size() - (line.size() - commentSign + 1) - equalSign);
        rtrim(key);
        rtrim(value);
        properties.insert(make_pair(key, value));
    }

    return properties;
}