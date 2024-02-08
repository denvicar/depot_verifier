#define CATCH_CONFIG_MAIN
#include <algorithm>
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <string>

#include "../acf/acf.h"
#include "../hash/hash.h"
#include "../manifest/DepotManifest.h"
#include "test_config.h"

TEST_CASE("Test ACF parsing") {
  depotverifier::AcfInfo acf(std::filesystem::path(kTestAcfPath), "999999");
  REQUIRE(acf.name() == "TEST NAME");
  REQUIRE(acf.installdir() == "TEST INSTALL DIR");
  REQUIRE(acf.manifests().size() == 1);
  REQUIRE(acf.manifests()[0] == "999999_1099988877766655544.manifest");
}

TEST_CASE("Test hash calculation") {
  depotverifier::HashManager hash_manager;
  hash_manager.SetFile(std::filesystem::path{kTestHashPath});
  REQUIRE_THAT(
      hash_manager.GetHash(),
      Catch::Matchers::Equals("DA39A3EE5E6B4B0D3255BFEF95601890AFD80709",
                              Catch::CaseSensitive::No));
}