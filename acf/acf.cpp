#include "acf.h"

#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "depot_utils.h"

using std::cerr;
using std::getline;
using std::ifstream;
using std::isspace;
using std::size_t;
using std::string;
using std::vector;
using std::filesystem::path;

namespace depotverifier {
  AcfInfo::AcfInfo(path p, string appid) : appid_(appid) {
    ifstream f(p);
    if (!f.is_open()) {
      cerr << "Error while opening config file";
      exit(EXIT_FAILURE);
    }

    string line;
    string manifest_prefix;
    size_t info_start;
    bool manifest_part = false;
    int bracket_count = 0;
    while (getline(f, line)) {
      ltrim(line);
      if (auto p = line.find(kAcfName); p != string::npos) {
        info_start = line.find("\"", p + kAcfName.size()) + 1;
        name_ = line.substr(info_start, line.find_last_of("\"") - info_start);
      } else if (auto p = line.find(kAcfInstallDir); p != string::npos) {
        info_start = line.find("\"", p + kAcfInstallDir.size()) + 1;
        installdir_
            = line.substr(info_start, line.find_last_of("\"") - info_start);
      } else if (auto p = line.find(kAcfManifest); p != string::npos) {
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
        if (auto p = line.find(kManifestStart); p != string::npos) {
          // concatenate manifest_prefix with buildid
          info_start = line.find("\"", p + kManifestStart.size()) + 1;
          string manifestId
              = line.substr(info_start, line.find_last_of("\"") - info_start);
          manifests_.push_back(manifest_prefix + "_" + manifestId
                               + ".manifest");
          bracket_count = 0;
        } else if (DepotIdLine(line)) {
          manifest_prefix = line.substr(1, line.find_last_of('"') - 1);
          continue;
        }
      }
    }
  }

  bool AcfInfo::DepotIdLine(const string& line) {
    for (const char& c : line) {
      if (c != '"' && !isdigit(c)) return false;
    }
    return true;
  }

  std::ostream& operator<<(std::ostream& os, const AcfInfo& obj) {
    os << "appid: \033[34m" << obj.appid_ << "\033[0m"
       << "\nname: \033[34m" << obj.name_ << "\033[0m"
       << "\ninstalldir: \033[34m" << obj.installdir_ << "\033[0m"
       << "\nmanifests: \033[34m" << obj.manifests_.size() << "\033[0m\n";
    return os;
  }

}  // namespace depotverifier