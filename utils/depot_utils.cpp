#include "depot_utils.h"

#include <algorithm>
#include <boost/program_options.hpp>
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <filesystem>

namespace po = boost::program_options;
namespace fs = std::filesystem;

using std::cerr;
using std::find_if;
using std::getline;
using std::ifstream;
using std::isspace;
using std::make_pair;
using std::string;
using std::unordered_map;

void ltrim(string& line) {
  line.erase(line.begin(),
             find_if(line.begin(), line.end(),
                     [](unsigned char ch) { return !isspace(ch); }));
}

void rtrim(string& line) {
  line.erase(find_if(line.rbegin(), line.rend(),
                     [](unsigned char ch) { return !isspace(ch); })
                 .base(),
             line.end());
}

po::variables_map InitParse(int ac, char* av[], string& help_message) {
  po::arg = "PATH";
  po::options_description generic("Generic options");
  generic.add_options()("help,h", "generate this help message")
  ("manifests,m", po::value<std::vector<std::string>>()->multitoken(), "list of manifests files")
  ("game,g", po::value<std::string>(), "game directory");

  po::options_description file_options("Config file options");
  file_options.add_options()("SteamDirectories.AcfDir",
                             po::value<std::string>(),
                             "path where the acf files are stored")(
      "SteamDirectories.ManifestDir", po::value<std::string>(),
      "path where the manifest files are stored")(
      "SteamDirectories.GameDir", po::value<std::string>(),
      "path where the game directories are stored");

  po::options_description hidden("Hidden");
  hidden.add_options()("appids", po::value<std::vector<std::string>>(),
                       "list of appids");

  po::options_description cmd_line_options;
  cmd_line_options.add(generic).add(hidden);
  po::options_description visible;
  visible.add(generic).add(file_options);
  po::positional_options_description pd;
  pd.add("appids", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(ac, av)
                .options(cmd_line_options)
                .positional(pd)
                .run(),
            vm);

  if(fs::exists("depot_verifier.ini"))
    po::store(po::parse_config_file("depot_verifier.ini", file_options, true),
            vm);
  po::notify(vm);

  help_message = GetHelpMessage(visible);
  return vm;
}

std::string GetHelpMessage(po::options_description opt) {
  std::stringstream ss;
  ss << "Usage: depot_verifier.exe [options] appid1 [appid2 [...]]";
  ss << opt;
  ss << "\nOptions (except appids) have to be specified in depot_verifier.ini."
     << "\nIf the -g (or --game) and -m (or --manifests) are specified the values in depot_verifier.ini are ignored."
     << "\nAlso in that case you don't need to specify appids."
     << "\n";
  return ss.str();
}

unordered_map<string, string> ParseOptions(po::variables_map vm) {
  unordered_map<string, string> props;

  for (auto& x : vm) {
    if (x.first != "appids") props.emplace(x.first, x.second.as<string>());
  }
  return props;
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
    if (size_t c = line.find(';', equalSign); c != string::npos)
      commentSign = c;
    key = line.substr(0, equalSign);
    value = line.substr(
        equalSign + 1,
        line.size() - (line.size() - commentSign + 1) - equalSign);
    rtrim(key);
    rtrim(value);
    properties.insert(make_pair(key, value));
  }

  return properties;
}