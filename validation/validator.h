#ifndef _VALIDATOR_H
#define _VALIDATOR_H
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

#include "../acf/acf.h"
#include "../hash/hash.h"
#include "../manifest/DepotFile.h"

namespace depotverifier {
  class Validator {
  private:
    std::filesystem::path manifest_dir_;
    std::vector<std::filesystem::path> game_files_;
    std::vector<std::string> processed_files_;
    std::unordered_map<std::string, std::vector<int>> summary_;
    HashManager hash_manager_;
    int ok = 0, missing = 0, wrong = 0;
    std::filesystem::path current_game_dir_;
    AcfInfo* current_acf_;

    void ValidateDepotFile(const DepotFile& df);
    void UpdateSummary();
    void CheckUnknownFiles();
    void ReadGameFiles();

  public:
    Validator(std::filesystem::path manifest_dir)
        : manifest_dir_(manifest_dir), current_acf_(nullptr){};
    Validator()
        : manifest_dir_(""), current_acf_(nullptr){};
    void set_acf(AcfInfo* acf) { current_acf_ = acf; };
    void set_game_dir(const std::filesystem::path& dir) {
      current_game_dir_ = dir;
    };
    std::vector<std::filesystem::path> game_files() const {
      return game_files_;
    };
    std::vector<std::string> processed_files() const {
      return processed_files_;
    };
    void PrintSummary() const;
    void Clear();
    void Validate();
  };
}  // namespace depotverifier

#endif  // _VALIDATOR_H