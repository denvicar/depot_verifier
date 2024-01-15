#include "DepotManifest.h"
#include "DepotFile.h"
#include "StructReader.h"
#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <fstream>

using std::uint8_t;
using std::uint32_t;
using std::vector;
using std::runtime_error;

typedef uint8_t u8;


//vector<u8> decompress(vector<u8>& data) {
//	mz_zip_archive zip;
//	mz_zip_reader_init_mem(&zip, data.data(), data.size(), 0);
//
//	mz_uint num_files = mz_zip_reader_get_num_files(&zip);
//	if (num_files > 0) {
//		mz_zip_archive_file_stat stat;
//		if (mz_zip_reader_file_stat(&zip, 0, &stat)) {
//			vector<u8> decompressedContent(stat.m_uncomp_size);
//			if (mz_zip_reader_extract_to_mem(&zip, 0, decompressedContent.data(), decompressedContent.size(), 0) == MZ_TRUE) {
//				mz_zip_reader_end(&zip);
//				return decompressedContent;
//			}
//		}
//	}
//
//	mz_zip_reader_end(&zip);
//	return data;
//}

DepotManifest::DepotManifest(vector<uint8_t> d) : data(d) {
	deserialize(data);
}


void DepotManifest::deserialize(vector<uint8_t>& d) {
	//d = decompress(d);

	StructReader sr = StructReader(d);
	vector<uint32_t> output;
	vector<uint8_t> bytes;
	uint32_t magic = 0;
	uint32_t length = 0;

	output = sr.unpack("<II");
	magic = output[0];
	length = output[1];

	if (magic != DepotManifest::PROTOBUF_PAYLOAD_MAGIC) {
		throw runtime_error("Expecting protobuf payload");
	}

	bytes = sr.read(length);
	payload.ParseFromString(string(bytes.begin(),bytes.end()));

	output = sr.unpack("<II");
	magic = output[0];
	length = output[1];

	if (magic != DepotManifest::PROTOBUF_METADATA_MAGIC) {
		throw runtime_error("Expecting protobuf metadata");
	}

	bytes = sr.read(length);
	metadata.ParseFromString(string(bytes.begin(), bytes.end()));

	output = sr.unpack("<II");
	magic = output[0];
	length = output[1];

	if (magic != DepotManifest::PROTOBUF_SIGNATURE_MAGIC) {
		throw runtime_error("Expecting protobuf signature");
	}

	bytes = sr.read(length);
	signature.ParseFromString(string(bytes.begin(), bytes.end()));

	output = sr.unpack("<I");
	magic = output[0];

	if (magic != DepotManifest::PROTOBUF_ENDOFMANIFEST_MAGIC) {
		throw runtime_error("Expecting protobuf end of manifest");
	}

	curr = payload.mappings().begin();
}
void DepotManifest::decrypt_filenames(vector<uint8_t>& d) {
	if (!metadata.filenames_encrypted()) {
		return;
	}

}

bool DepotManifest::has_next() const {
	return curr != payload.mappings().end();
}

DepotFile DepotManifest::next() {
	DepotFile df = DepotFile(*this, *curr);
	curr++;
	return df;
}

DepotManifest read_manifest_file(std::filesystem::path manifest) {
	std::ifstream f{ manifest , std::ios::binary };
	f.seekg(0, std::ios::end);
	auto fileSize = f.tellg();
	f.seekg(0, std::ios::beg);
	vector<uint8_t> content(fileSize);

	f.read(reinterpret_cast<char*>(content.data()), fileSize);
	f.close();
	return DepotManifest(content);
}

//auto DepotManifest::iter_files(string pattern) {
//	vector<DepotFile> res;
//	if (!filenames_encrypted()) {
//		cout << size();
//		int i = 0;
//		for (auto& mapping : payload.mappings()) {
//			++i;
//			cout << i << "/" << size() << endl;
//			res.push_back(DepotFile(*this, mapping));
//		}
//	}
//	return res;
//}

