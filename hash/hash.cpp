#include "hash.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include "./sha1.hpp"

using std::hex;
using std::ifstream;
using std::setfill;
using std::setw;
using std::string;
using std::stringstream;
using std::vector;
using std::filesystem::path;

string depotverifier::ByteToHex(string& hash) {
  stringstream result;
  for (unsigned char ch : hash) {
    result << hex << setw(2) << setfill('0') << static_cast<int>(ch);
  }
  return result.str();
}

namespace depotverifier {

  void HashManager::ComputeHash() {
    ifstream f(current_file, std::ios::binary);
    vector<char> buffer(8192);

    while (true) {
      f.read(buffer.data(), buffer.size());
      sha_gen.Update(std::string(buffer.data(), f.gcount()));
      if (f.eof()) break;
    }

    std::string digest = sha_gen.Final();

    stringstream result;

    for (unsigned char ch : digest) {
      result << hex << std::setw(2) << std::setfill('0')
             << static_cast<int>(ch);
    }

    hash = result.str();
  }

  std::string HashManager::GetHash() {
    if (hash.size() != 0) return hash;
    ComputeHash();
    return hash;
  };

  void HashManager::SetFile(std::filesystem::path path) {
    hash = "";
    current_file = path;
  }

}  // namespace depotverifier