#include <boost/program_options.hpp>
#include <cctype>
#include <filesystem>
#include <iostream>

#include "acf.h"
#include "depot_utils.h"
#include "validator.h"

using namespace std;
namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  ios_base::sync_with_stdio(false);

  string help_message;
  po::variables_map vm = InitParse(argc, argv, help_message);

  if (vm.count("help")) {
    cout << help_message;
    return 1;
  }

  if ((vm.count("manifests") == 0 && vm.count("game") != 0)
  || (vm.count("manifests") != 0 && vm.count("game") == 0)) {
    cerr << "The flags -g (--game) and -m (--manifests) need to be specified both or not at all\n";
    cerr << help_message;
    return 1;
  }

  if (vm.count("game")!=0) {
    string game_dir = vm["game"].as<string>();
    depotverifier::Validator validator;
    depotverifier::AcfInfo info(vm["manifests"].as<vector<string>>(), game_dir);
    validator.set_acf(&info);
    validator.set_game_dir(path { game_dir });
    validator.Validate();
    validator.Clear();
    validator.PrintSummary();
    return 0;
  }
  
  // did not use -m and -g, so appids have to be present
  if (vm.count("appids") < 1) {
    cerr << "Expected at least one appid\n";
    cerr << help_message;
    return 1;
  }

  filesystem::path acf_dir(vm["SteamDirectories.AcfDir"].as<string>());
  filesystem::path manifest_dir(
      vm["SteamDirectories.ManifestDir"].as<string>());
  filesystem::path game_dir(vm["SteamDirectories.GameDir"].as<string>());

  if (!filesystem::exists(acf_dir) || !filesystem::exists(manifest_dir)
      || !filesystem::exists(game_dir)) {
    cerr << "Check provided options, one of the specified paths does not "
            "exist\n";
    return 1;
  }

  depotverifier::Validator validator(manifest_dir);

  for (const string& appid : vm["appids"].as<vector<string>>()) {
    for (const filesystem::directory_entry& entry :
         filesystem::directory_iterator{acf_dir}) {
      if (entry.path().string().find(appid) != string::npos) {
        depotverifier::AcfInfo info(entry.path(), appid);
        std::cout << info;
        filesystem::path game_main_dir = game_dir / (info.installdir());

        validator.set_acf(&info);
        validator.set_game_dir(game_main_dir);

        validator.Validate();
        validator.Clear();
      }
    }
  }

  validator.PrintSummary();

  return 0;
}
