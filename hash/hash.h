#ifndef _HASH_H
#define _HASH_H
#include <openssl/evp.h>

#include <filesystem>
#include <string>

// std::string get_hash_content(EVP_MD_CTX*, std::filesystem::path);

namespace depotverifier {
  std::string ByteToHex(std::string&);

  class HashManager {
  public:
    HashManager();
    void SetFile(std::filesystem::path path);
    std::string GetHash();
    ~HashManager();

  private:
    EVP_MD_CTX* ctx;
    std::filesystem::path current_file;
    std::string hash;
    void ComputeHash();
  };
}  // namespace depotverifier

#endif  // _HASH_H