#ifndef _SHA1_HPP
#define _SHA1_HPP
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>

namespace depotverifier {
  class Sha1Sum {
  private:
    uint32_t digest[5];
    std::string buffer;
    uint64_t transforms;

  public:
    Sha1Sum();
    void Update(const std::string &s);
    void Update(std::istream &is);
    std::string Final();
    static std::string FromFile(const std::string &filename);
  };

  static const size_t kIntegersPerBlock = 16;  // 16*32 = 512 bit sha1 block
  static const size_t kBytesInBlock
      = kIntegersPerBlock * 4;  // number of bytes in a block (64)

  inline static void Reset(uint32_t digest[], std::string &buffer,
                           uint64_t &transforms) {
    digest[0] = 0x67452301;
    digest[1] = 0xefcdab89;
    digest[2] = 0x98badcfe;
    digest[3] = 0x10325476;
    digest[4] = 0xc3d2e1f0;

    buffer = "";
    transforms = 0;
  }

  inline static uint32_t RotateLeft(const uint32_t value, const size_t bits) {
    return (value << bits) | (value >> (32 - bits));
  }

  inline Sha1Sum::Sha1Sum() { Reset(digest, buffer, transforms); }

  inline std::string Sha1Sum::FromFile(const std::string &filename) {
    std::ifstream stream(filename, std::ios::binary);
    Sha1Sum checksum;
    checksum.Update(stream);
    return checksum.Final();
  }

  inline static void BufferToBlock(const std::string &buffer,
                                   uint32_t block[kIntegersPerBlock]) {
    for (int i = 0; i < kIntegersPerBlock; ++i) {
      block[i] = (buffer[4 * i] & 0xff) << 24 | (buffer[4 * i + 1] & 0xff) << 16
                 | (buffer[4 * i + 2] & 0xff) << 8 | (buffer[4 * i + 3] & 0xff);
    }
  }

  inline static void Transform(uint32_t digest[],
                               uint32_t block[kIntegersPerBlock],
                               uint64_t &transforms) {
    uint32_t extended_block[80];
    for (int i = 0; i < kIntegersPerBlock; ++i) extended_block[i] = block[i];
    for (int i = 16; i < 80; ++i) {
      extended_block[i]
          = RotateLeft((extended_block[i - 3] ^ extended_block[i - 8]
                        ^ extended_block[i - 14] ^ extended_block[i - 16]),
                       1);
    }

    uint32_t a = digest[0];
    uint32_t b = digest[1];
    uint32_t c = digest[2];
    uint32_t d = digest[3];
    uint32_t e = digest[4];
    uint32_t f, k, temp;

    for (int i = 0; i < 80; ++i) {
      if (i <= 19) {
        f = (b & c) | ((~b) & d);
        k = 0x5a827999;
      } else if (i <= 39) {
        f = b ^ c ^ d;
        k = 0x6ed9eba1;
      } else if (i <= 59) {
        f = (b & c) | (b & d) | (c & d);
        k = 0x8f1bbcdc;
      } else {
        f = b ^ c ^ d;
        k = 0xca62c1d6;
      }

      temp = RotateLeft(a, 5) + f + e + k + extended_block[i];
      e = d;
      d = c;
      c = RotateLeft(b, 30);
      b = a;
      a = temp;
    }

    digest[0] += a;
    digest[1] += b;
    digest[2] += c;
    digest[3] += d;
    digest[4] += e;
    transforms++;
  }

  inline void Sha1Sum::Update(std::istream &is) {
    while (true) {
      char temp[kBytesInBlock];
      is.read(temp, kBytesInBlock - buffer.size());
      buffer.append(temp, static_cast<size_t>(is.gcount()));
      if (buffer.size() != kBytesInBlock) return;
      uint32_t block[kIntegersPerBlock];
      BufferToBlock(buffer, block);
      Transform(digest, block, transforms);
      buffer.clear();
    }
  }

  inline void Sha1Sum::Update(const std::string &s) {
    std::istringstream is(s);
    Update(is);
  }

  inline std::string Sha1Sum::Final() {
    uint64_t total_bits = (transforms * kBytesInBlock + buffer.size()) * 8;
    buffer += (char)0x80;
    size_t orig_size = buffer.size();
    while (buffer.size() < kBytesInBlock) {
      buffer += (char)0x00;
    }
    uint32_t block[kIntegersPerBlock];
    BufferToBlock(buffer, block);
    if (orig_size > kBytesInBlock - 8) {
      Transform(digest, block, transforms);
      for (size_t i = 0; i < kIntegersPerBlock - 2; ++i) {
        block[i] = 0;
      }
    }

    block[kIntegersPerBlock - 1] = (uint32_t)total_bits;
    block[kIntegersPerBlock - 2] = (uint32_t)(total_bits >> 32);
    Transform(digest, block, transforms);

    unsigned char output[20];
    for (size_t i = 0; i < sizeof(digest) / sizeof(digest[0]); ++i) {
      output[i * 4] = digest[i] >> 24;
      output[i * 4 + 1] = (digest[i] >> 16) & 0xff;
      output[i * 4 + 2] = (digest[i] >> 8) & 0xff;
      output[i * 4 + 3] = (digest[i] & 0xff);
    }

    Reset(digest, buffer, transforms);
    return std::string((char *)output, sizeof(output));
  }

}  // namespace depotverifier

#endif  // _SHA1_HPP