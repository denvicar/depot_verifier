#pragma once
#include <cstdint>
#include <string>
#include <vector>

class StructReader {
private:
  std::vector<std::uint8_t> data;
  std::size_t offset;

public:
  StructReader(std::vector<uint8_t> d) : offset(0), data(d) {}
  std::size_t size(void) const;
  std::size_t rlen(void) const;
  std::vector<uint8_t> read(const int);
  std::vector<uint32_t> unpack(const std::string& format_text);
};
