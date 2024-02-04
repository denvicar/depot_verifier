#pragma once
#include <cstdint>
#include <filesystem>
#include <vector>

#include "../protobuf/content_manifest.pb.h"

using std::uint32_t;
using std::uint8_t;
using std::vector;
class DepotFile;

class DepotManifest {
public:
  static const uint32_t PROTOBUF_PAYLOAD_MAGIC = 0x71F617D0;
  static const uint32_t PROTOBUF_METADATA_MAGIC = 0x1F4812BE;
  static const uint32_t PROTOBUF_SIGNATURE_MAGIC = 0x1B81B817;
  static const uint32_t PROTOBUF_ENDOFMANIFEST_MAGIC = 0x32C415AB;
  DepotManifest(vector<uint8_t> data);
  auto depot_id() { return metadata.depot_id(); }
  auto gid() { return metadata.gid_manifest(); }
  auto creation_time() { return metadata.creation_time(); }
  auto size_original() { return metadata.cb_disk_original(); }
  auto size_compressed() { return metadata.cb_disk_compressed(); }
  auto filenames_encrypted() { return metadata.filenames_encrypted(); }
  void deserialize(vector<uint8_t>&);
  void decrypt_filenames(vector<uint8_t>&);
  int size() const { return payload.mappings_size(); }
  bool has_next() const;
  DepotFile next();

private:
  vector<uint8_t> data;
  ContentManifestMetadata metadata;
  ContentManifestPayload payload;
  ContentManifestSignature signature;
  decltype(payload.mappings().begin()) curr;
};

DepotManifest read_manifest_file(std::filesystem::path);
