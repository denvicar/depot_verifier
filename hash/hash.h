#pragma once
#include <openssl/evp.h>

#include <filesystem>
#include <string>

std::string get_hex_string(std::string&);
std::string get_hash_content(EVP_MD_CTX*, std::filesystem::path);
