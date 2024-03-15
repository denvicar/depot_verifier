#ifndef _HASH_H
#define _HASH_H

#include <filesystem>
#include <string>

#include "./sha1.hpp"

// std::string get_hash_content(EVP_MD_CTX*, std::filesystem::path);

namespace depotverifier {
  std::string ByteToHex(const std::string&);

  class HashManager {
  public:
    void SetFile(std::filesystem::path path);
    std::string GetHash();

  private:
    Sha1Sum sha_gen;

    std::filesystem::path current_file;
    std::string hash;
    void ComputeHash();
  };
}  // namespace depotverifier

#endif  // _HASH_H