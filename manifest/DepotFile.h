#pragma once
#include "DepotManifest.h"
#include "../protobuf/content_manifest.pb.h"
#include <string>
#include <cstdint>

using std::string;
using std::uint64_t;

enum EDepotFileFlag {
	Directory = 64,
	Executable = 32
};

class DepotFile {
public:
	DepotFile(DepotManifest dm, ContentManifestPayload_FileMapping fm) : manifest(dm), file_mapping(fm) {}
	string filename() const { return file_mapping.filename(); }
	string linktarget() const { return file_mapping.linktarget(); }
	string sha_content() const { return file_mapping.sha_content(); }
	uint64_t size() const { return file_mapping.size(); }
	auto flags() const { return file_mapping.flags(); }
	bool is_directory() const { return (flags() & EDepotFileFlag::Directory) > 0; }
	bool is_symlink() const { return !file_mapping.linktarget().empty(); }
	bool is_file() const { return !is_directory() && !is_symlink(); }
	bool is_executable() const { return (flags() & EDepotFileFlag::Executable) > 0; }


private:
	DepotManifest manifest;
	ContentManifestPayload_FileMapping file_mapping;
};