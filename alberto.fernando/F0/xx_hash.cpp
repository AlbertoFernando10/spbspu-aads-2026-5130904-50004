#include "xx_hash.hpp"

namespace alberto {

size_t xx_hash::operator()(const void * data, size_t len) const noexcept
{
  static constexpr uint64_t fnv_prime = 0x00000100000001B3ULL;
  static constexpr uint64_t fnv_offset = 0xCBF29CE484222325ULL;

  uint64_t hash = fnv_offset;
  const unsigned char * const bytes = static_cast< const unsigned char * >(data);

  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast< uint64_t >(bytes[i]);
    hash *= fnv_prime;
  }

  return static_cast< size_t >(hash);
}

size_t xx_hash::operator()(const std::string & key) const noexcept
{
  return operator()(key.data(), key.size());
}

size_t xx_hash::operator()(char key) const noexcept
{
  uint64_t hash = static_cast< uint64_t >(static_cast< unsigned char >(key));
  hash ^= hash >> 33;
  hash *= 0xFF51AFD7ED558CCDULL;
  hash ^= hash >> 33;
  hash *= 0xC4CEB9FE1A85EC53ULL;
  hash ^= hash >> 33;

  return static_cast< size_t >(hash);
}

}
