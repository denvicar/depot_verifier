#include "StructReader.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

using std::size_t;
using std::uint32_t;
using std::uint8_t;
using std::vector;

uint32_t get_unsignedint(const vector<uint8_t>& v) {
  uint32_t res = 0;
  for (auto it = v.rbegin(); it != v.rend(); ++it) {
    res = (res << 8) | *it;
  }
  return res;
}

std::size_t StructReader::size() const { return data.size(); }

std::vector<uint8_t> StructReader::read(const int n = 1) {
  this->offset += n;
  return std::vector<uint8_t>(&(this->data[this->offset - n]),
                              &(this->data[this->offset]));
}

size_t StructReader::rlen() const {
  return std::max<size_t>(size_t(0), StructReader::size() - offset);
}

std::vector<uint32_t> StructReader::unpack(const std::string& format_text) {
  int number_of_elements = format_text.size() - 1;
  vector<uint32_t> res;
  for (int i = 0; i < number_of_elements; ++i) {
    res.push_back(get_unsignedint(vector<uint8_t>(
        data.begin() + offset, data.begin() + (offset + sizeof(uint32_t)))));
    offset += sizeof(uint32_t);
  }
  return res;
}
