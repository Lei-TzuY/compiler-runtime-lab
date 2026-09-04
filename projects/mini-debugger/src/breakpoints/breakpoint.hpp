#pragma once

#include <cstddef>
#include <cstdint>

namespace mdbg {

struct Breakpoint {
  std::size_t id;
  std::uintptr_t address;
  std::byte original_byte;
  bool installed;
};

}  // namespace mdbg
