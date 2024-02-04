#include <openssl/evp.h>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

#include "acf/acf.h"
#include "hash/hash.h"
#include "manifest/DepotFile.h"
#include "manifest/DepotManifest.h"
#include "utils/util.h"

using namespace std;

int main(int argc, char* argv[]) {
  ios_base::sync_with_stdio(false);

  if (argc < 2) {
    cerr << "Expected at least one appid\n";
    cerr << "Usage: depot_verifier.exe appid [appid...]\n";
    return 1;
  }

  if (argc == 2 && argv[1] == "-h") {
    cout << "Usage: depot_verifier.exe appid [appid...]\nSpecify the relevant "
            "paths in depot_verifier.ini\n";
    return 0;
  }

  unordered_map<string, string> properties = read_config();

  filesystem::path acf_dir(properties["AcfDir"]);
  filesystem::path manifest_dir(properties["ManifestDir"]);
  filesystem::path game_dir(properties["GameDir"]);

  if (!filesystem::exists(acf_dir) || !filesystem::exists(manifest_dir)
      || !filesystem::exists(game_dir)) {
    cerr << "Check depot_verifier.ini, one of the specified paths does not "
            "exist\n";
    return 1;
  }

  EVP_MD_CTX* ctx = EVP_MD_CTX_new();

  acf_info info;
  vector<filesystem::path> game_files;
  vector<string> processed_files;
  unordered_map<string, vector<int>> summary;

  for (int i = 1; i < argc; ++i) {
    for (const filesystem::directory_entry& entry :
         filesystem::directory_iterator{acf_dir}) {
      if (entry.path().string().find(argv[i]) != string::npos) {
        info = get_acf_info(entry.path());
        std::cout << "appid: \033[34m" << argv[i] << "\033[0m"
                  << "\nname: \033[34m" << info.name << "\033[0m"
                  << "\ninstalldir: \033[34m" << info.installdir << "\033[0m"
                  << "\nmanifests: \033[34m" << info.manifests.size()
                  << "\033[0m\n";

        int missing = 0, wrong = 0, ok = 0;

        filesystem::path game_main_dir = game_dir / (info.installdir);

        for (const filesystem::directory_entry& game_entry :
             filesystem::recursive_directory_iterator{game_main_dir}) {
          if (game_entry.is_directory()) continue;
          game_files.push_back(game_entry.path());
        }

        cout << "game files: " << game_files.size() << '\n';

        for (auto it = info.manifests.rbegin(); it != info.manifests.rend();
             ++it) {
          cout << "manifest: " << *it << '\n';
          filesystem::path manifest_path = manifest_dir / (*it);
          if (!filesystem::exists(manifest_path)) {
            cout << "does not exist, going to the next one" << '\n';
            continue;
          }
          DepotManifest dm = read_manifest_file(manifest_path);
          while (dm.has_next()) {
            DepotFile df = dm.next();
            if (!df.is_file()) continue;
            if (find(processed_files.begin(), processed_files.end(),
                     df.filename())
                != processed_files.end())
              continue;
            cout << "validating " << df.filename() << " "
                 << processed_files.size() + 1 << '/' << game_files.size()
                 << ' ';
            filesystem::path df_path = game_main_dir / df.filename();
            if (!filesystem::exists(df_path)) {
              cout << "\033[31mMISSING\033[0m" << '\n';
              missing++;
            } else if (filesystem::file_size(df_path) != df.size()) {
              cout << "\033[31mDIFFERENT SIZE!\033[0m \n";
              wrong++;
            } else if (auto sha = get_hash_content(ctx, df_path);
                       sha != get_hex_string(df.sha_content())) {
              cout << "\033[31mDIFFERENT SHA!\033[0m \n";
              wrong++;
            } else {
              cout << "\033[32mOK!\033[0m \n";
              ok++;
            }
            processed_files.push_back(df.filename());
          }
        }

        summary.insert(make_pair(
            info.name, vector<int>{static_cast<int>(processed_files.size()),
                                   missing, wrong, ok}));

        if (processed_files.size() != game_files.size()) {
          cout << "there are some unknown files in game directory: " << '\n';
          for (const auto& game_file : game_files) {
            if (find_if(processed_files.begin(), processed_files.end(),
                        [game_file](string name) {
                          return name.find(game_file.filename().string())
                                 != string::npos;
                        })
                == processed_files.end())
              cout << game_file.relative_path() << '\n';
          }
        }
        processed_files.clear();
        game_files.clear();
      }
    }
  }

  for (const auto& [name, stats] : summary) {
    cout << name << '\n';
    cout << "total: " << stats[0] << " missing: \033[31m" << stats[1]
         << "\033[0m wrong: \033[31m" << stats[2] << "\033[0m ok: \033[32m"
         << stats[3] << "\033[0m\n";
    if (stats[3] == stats[0]) cout << "VALID!\n";
  }

  EVP_MD_CTX_free(ctx);

  return 0;
}
