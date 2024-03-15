#ifndef _ACF_H
#define _ACF_H
#include <filesystem>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

using std::string;
using std::vector;
using std::filesystem::path;

namespace depotverifier {
  class AcfInfo {
  public:
    AcfInfo(){};
    AcfInfo(std::vector<std::string> manifests, std::string game)
        : manifests_(manifests), installdir_(game){};
    AcfInfo(path p, string appid);
    string name() const { return name_; };
    string installdir() const { return installdir_; };
    const vector<string>& manifests() const { return manifests_; };
    friend std::ostream& operator<<(std::ostream& os, const AcfInfo& obj);

  private:
    static constexpr std::string_view kAcfName = "\"name\"";
    static constexpr std::string_view kAcfInstallDir = "\"installdir\"";
    static constexpr std::string_view kAcfManifest = "\"InstalledDepots\"";
    static constexpr std::string_view kManifestStart = "\"manifest\"";

    bool DepotIdLine(const string&);

    string name_;
    string installdir_;
    vector<string> manifests_;
    string appid_;
  };

}  // namespace depotverifier
#endif  // _ACF_H