#include <string>
#include <openssl/evp.h>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <openssl/sha.h>
#include "hash.h"

using std::string;
using std::stringstream;
using std::hex;
using std::setw;
using std::setfill;
using std::filesystem::path;
using std::ifstream;
using std::vector;


string get_hex_string(string& hash) {
    stringstream result;
    for (unsigned char ch : hash) {
        result << hex << setw(2) << setfill('0') << static_cast<int>(ch);
    }
    return result.str();
}

string get_hash_content(EVP_MD_CTX* ctx, std::filesystem::path fileName) {
    EVP_DigestInit_ex(ctx, EVP_sha1(), nullptr);
    ifstream f(fileName, std::ios::binary);
    vector<char> buffer(8192);

    while (true) {
        f.read(buffer.data(), buffer.size());
        EVP_DigestUpdate(ctx, buffer.data(), f.gcount());
        if (f.eof()) break;
    }

    unsigned char hash[SHA_DIGEST_LENGTH];

    EVP_DigestFinal_ex(ctx, hash, nullptr);

    stringstream result;

    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        result << hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return result.str();
}
