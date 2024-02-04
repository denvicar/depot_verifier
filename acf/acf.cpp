#include "acf.h"

#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "../utils/util.h"

using std::cerr;
using std::getline;
using std::ifstream;
using std::isspace;
using std::size_t;
using std::string;
using std::vector;
using std::filesystem::path;

bool depot_id_line(const string& line) {
  for (const char& c : line) {
    if (c != '"' && !isdigit(c)) return false;
  }
  return true;
}

acf_info get_acf_info(path p) {
  ifstream f(p);
  if (!f.is_open()) {
    cerr << "Error while opening config file";
    exit(EXIT_FAILURE);
  }
  acf_info info;

  string line;
  string manifest_prefix;
  size_t info_start;
  bool manifest_part = false;
  int bracket_count = 0;
  while (getline(f, line)) {
    ltrim(line);
    if (auto p = line.find(ACF_NAME); p != string::npos) {
      info_start = line.find("\"", p + ACF_NAME.size()) + 1;
      info.name = line.substr(info_start, line.find_last_of("\"") - info_start);
    } else if (auto p = line.find(ACF_INSTALL_DIR); p != string::npos) {
      info_start = line.find("\"", p + ACF_INSTALL_DIR.size()) + 1;
      info.installdir
          = line.substr(info_start, line.find_last_of("\"") - info_start);
    } else if (auto p = line.find(ACF_MANIFEST); p != string::npos) {
      manifest_part = true;
      getline(f, line);
      continue;
    }

    if (manifest_part) {
      if (line[0] == '}') {
        if (bracket_count == 1)
          manifest_part = false;
        else
          bracket_count++;
      }
      if (auto p = line.find(MANIFEST_START); p != string::npos) {
        // concatenate manifest_prefix with buildid
        info_start = line.find("\"", p + MANIFEST_START.size()) + 1;
        string manifestId
            = line.substr(info_start, line.find_last_of("\"") - info_start);
        info.manifests.push_back(manifest_prefix + "_" + manifestId
                                 + ".manifest");
        bracket_count = 0;
      } else if (depot_id_line(line)) {
        manifest_prefix = line.substr(1, line.find_last_of('"') - 1);
        continue;
      }
    }
  }

  return info;
}