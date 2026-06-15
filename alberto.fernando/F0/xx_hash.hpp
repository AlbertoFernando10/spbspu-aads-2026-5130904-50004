#ifndef XX_HASH_HPP
#define XX_HASH_HPP

#include <cstddef>
#include <cstdint>
#include <string>

namespace alberto {
struct xx_hash
{
  size_t operator()(const std::string& key) const noexcept
  {
    static constexpr uint64_t fnv_prime = 0x00000100000001B3ULL;
    static constexpr uint64_t fnv_offset = 0xCBF29CE484222325ULL;
    uint64_t hash = fnv_offset;
    for (unsigned char c : key)
    {
      hash ^= static_cast<uint64_t>(c);
      hash *= fnv_prime;
    }
    return static_cast<size_t>(hash);
  }

  size_t operator()(char key) const noexcept
  {
    uint64_t hash = static_cast<uint64_t>(static_cast<unsigned char>(key));
    hash ^= hash >> 33;
    hash *= 0xFF51AFD7ED558CCDULL;
    hash ^= hash >> 33;
    hash *= 0xC4CEB9FE1A85EC53ULL;
    hash ^= hash >> 33;
    return static_cast<size_t>(hash);
  }
};

}

#endif
