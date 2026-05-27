#ifndef XXHASH_HPP
#define XXHASH_HPP
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace xxhash_impl {

static constexpr uint32_t PRIME1 = 0x9E3779B1u;
static constexpr uint32_t PRIME2 = 0x85EBCA77u;
static constexpr uint32_t PRIME3 = 0xC2B2AE3Du;
static constexpr uint32_t PRIME4 = 0x27D4EB2Fu;
static constexpr uint32_t PRIME5 = 0x165667B1u;
inline uint32_t
rotl32(uint32_t x, int r)
{
  return (x << r) | (x >> (32 - r));
}
inline uint32_t
xxhash32(const void* input, size_t len, uint32_t seed = 0)
{
  const uint8_t* p   = static_cast< const uint8_t* >(input);
  const uint8_t* end = p + len;
  uint32_t h32 = 0;

  if (len >= 16) {
    uint32_t v1 = seed + PRIME1 + PRIME2;
    uint32_t v2 = seed + PRIME2;
    uint32_t v3 = seed;
    uint32_t v4 = seed - PRIME1;
    do {
      uint32_t tmp = 0;
      memcpy(&tmp, p, 4);
      v1 = rotl32(v1 + tmp * PRIME2, 13) * PRIME1;
      p += 4;
      memcpy(&tmp, p, 4);
      v2 = rotl32(v2 + tmp * PRIME2, 13) * PRIME1;
      p += 4;
      memcpy(&tmp, p, 4);
      v3 = rotl32(v3 + tmp * PRIME2, 13) * PRIME1;
      p += 4;
      memcpy(&tmp, p, 4);
      v4 = rotl32(v4 + tmp * PRIME2, 13) * PRIME1;
      p += 4;
    } while (p <= end - 16);
    h32 = rotl32(v1,  1) + rotl32(v2,  7)
        + rotl32(v3, 12) + rotl32(v4, 18);
  } else {
    h32 = seed + PRIME5;
  }
  h32 += static_cast< uint32_t >(len);

  while (p + 4 <= end) {
    uint32_t tmp = 0;
    memcpy(&tmp, p, 4);
    h32 = rotl32(h32 + tmp * PRIME3, 17) * PRIME4;
    p += 4;
  }
  while (p < end) {
    h32 = rotl32(h32 + (*p) * PRIME5, 11) * PRIME1;
    ++p;
  }

  h32 ^= h32 >> 15;
  h32 *= PRIME2;
  h32 ^= h32 >> 13;
  h32 *= PRIME3;
  h32 ^= h32 >> 16;
  return h32;
}

}

#endif
