#pragma once
#include <string>
#include <openssl/evp.h>
#include <filesystem>

std::string get_hex_string(std::string&);
std::string get_hash_content(EVP_MD_CTX*, std::filesystem::path);
