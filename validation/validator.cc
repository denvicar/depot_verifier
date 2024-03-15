#include "validator.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>

#include "../manifest/DepotFile.h"
#include "../manifest/DepotManifest.h"

namespace filesystem = std::filesystem;
using std::cout;

namespace depotverifier {
  void Validator::Clear() {
    game_files_.clear();
    processed_files_.clear();
    missing = 0;
    wrong = 0;
    ok = 0;
  }

  void Validator::PrintSummary() const {
    for (const auto& [name, stats] : summary_) {
      std::cout << name << '\n';
      std::cout << "total: " << stats[0] << " missing: \033[31m" << stats[1]
                << "\033[0m wrong: \033[31m" << stats[2]
                << "\033[0m ok: \033[32m" << stats[3] << "\033[0m\n";
      if (stats[3] == stats[0]) std::cout << "VALID!\n";
    }
  }

  void Validator::ValidateDepotFile(const DepotFile& df) {
    if (!df.is_file()) return;
    if (std::find(processed_files_.begin(), processed_files_.end(),
                  df.filename())
        != processed_files_.end())
      return;

    cout << "validating " << df.filename() << " " << processed_files_.size() + 1
         << '/' << game_files_.size() << ' ';

    filesystem::path df_path = current_game_dir_ / df.filename();
    hash_manager_.SetFile(df_path);

    if (!filesystem::exists(df_path)) {
      cout << "\033[31mMISSING\033[0m" << '\n';
      missing++;
    } else if (filesystem::file_size(df_path) != df.size()) {
      cout << "\033[31mDIFFERENT SIZE!\033[0m \n";
      wrong++;
    } else if (auto sha = hash_manager_.GetHash();
               sha != depotverifier::ByteToHex(df.sha_content())) {
      cout << "\033[31mDIFFERENT SHA!\033[0m \n";
      wrong++;
    } else {
      cout << "\033[32mOK!\033[0m \n";
      ok++;
    }

    processed_files_.push_back(df.filename());
  }

  void Validator::UpdateSummary() {
    summary_.insert(
        make_pair(current_acf_->name(),
                  vector<int>{static_cast<int>(processed_files_.size()),
                              missing, wrong, ok}));
  }

  void Validator::CheckUnknownFiles() {
    if (processed_files_.size() != game_files_.size()) {
      cout << "there are some unknown files in game directory: " << '\n';
      for (const auto& game_file : game_files_) {
        if (std::find_if(processed_files_.begin(), processed_files_.end(),
                         [game_file](string name) {
                           return name.find(game_file.filename().string())
                                  != string::npos;
                         })
            == processed_files_.end())
          cout << game_file.relative_path() << '\n';
      }
    }
  }

  void Validator::ReadGameFiles() {
    for (const filesystem::directory_entry& game_entry :
         filesystem::recursive_directory_iterator{current_game_dir_}) {
      if (game_entry.is_directory()) continue;
      game_files_.push_back(game_entry.path());
    }
  }

  void Validator::Validate() {
    ReadGameFiles();

    cout << "game files: " << game_files_.size() << '\n';
    const vector<string>& manifests = current_acf_->manifests();
    for (auto it = manifests.rbegin(); it != manifests.rend(); ++it) {
      cout << "manifest: " << *it << '\n';
      filesystem::path manifest_path = manifest_dir_.string().size() == 0
                                           ? filesystem::path{(*it)}
                                           : (manifest_dir_ / (*it));
      if (!filesystem::exists(manifest_path)) {
        cout << "does not exist, going to the next one" << '\n';
        continue;
      }
      DepotManifest dm = read_manifest_file(manifest_path);
      while (dm.has_next()) {
        ValidateDepotFile(dm.next());
      }
    }

    UpdateSummary();
    CheckUnknownFiles();
  }

}  // namespace depotverifier
