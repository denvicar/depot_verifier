#include "hash.h"

#include <openssl/evp.h>
#include <openssl/sha.h>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

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
  HashManager::HashManager() { ctx = EVP_MD_CTX_new(); }
  HashManager::~HashManager() { EVP_MD_CTX_free(ctx); }
  void HashManager::ComputeHash() {
    EVP_DigestInit_ex(ctx, EVP_sha1(), nullptr);
    ifstream f(current_file, std::ios::binary);
    vector<char> buffer(8192);

    while (true) {
      f.read(buffer.data(), buffer.size());
      EVP_DigestUpdate(ctx, buffer.data(), f.gcount());
      if (f.eof()) break;
    }

    unsigned char digest[SHA_DIGEST_LENGTH];

    EVP_DigestFinal_ex(ctx, digest, nullptr);

    stringstream result;

    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
      result << hex << std::setw(2) << std::setfill('0')
             << static_cast<int>(digest[i]);
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