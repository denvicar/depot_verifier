#pragma once
#include <filesystem>
#include <string>
#include <vector>

using std::string;
using std::vector;
using std::filesystem::path;

struct acf_info {
  string name;
  string installdir;
  vector<string> manifests;
};

const string ACF_NAME = "\"name\"";
const string ACF_INSTALL_DIR = "\"installdir\"";
const string ACF_MANIFEST = "\"InstalledDepots\"";
const string MANIFEST_START = "\"manifest\"";

bool depot_id_line(const string&);

acf_info get_acf_info(std::filesystem::path);