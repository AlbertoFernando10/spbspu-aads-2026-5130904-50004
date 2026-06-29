#ifndef XX_HASH_HPP
#define XX_HASH_HPP

#include <cstddef>
#include <cstdint>
#include <string>

namespace alberto {

struct xx_hash
{
  size_t operator()(const void * data, size_t len) const noexcept;
  size_t operator()(const std::string & key) const noexcept;
  size_t operator()(char key) const noexcept;
};

}

#endif
