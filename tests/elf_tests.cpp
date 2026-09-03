#include "elf/elf.hpp"

#include <elf.h>

#include <iostream>
#include <stdexcept>
#include <string>

namespace {
void require(bool condition, const std::string& message) {
  if (!condition) throw std::runtime_error(message);
}

void test_symbols(const std::string& path, bool expect_pie) {
  const mdbg::ElfFile elf(path);
  require(elf.is_pie() == expect_pie, "ELF PIE classification mismatch for " + path);
  const auto one = elf.find_symbol("breakpoint_one");
  const auto two = elf.find_symbol("breakpoint_two");
  require(one && two, "expected fixture function symbols");
  require(one->type == STT_FUNC && two->type == STT_FUNC, "fixture symbols should be functions");
  const auto resolved = elf.find_symbol_by_virtual_address(one->value);
  require(resolved && resolved->symbol.name == "breakpoint_one" && resolved->offset == 0,
          "address-to-symbol lookup failed");
}
}  // namespace

int main(int argc, char** argv) {
  if (argc != 4) return 2;
  try {
    test_symbols(argv[1], true);
    test_symbols(argv[2], false);
    const mdbg::ElfFile stripped(argv[3]);
    require(!stripped.find_symbol("breakpoint_one"),
            "fully stripped fixture should not claim local function symbols");
    std::cout << "all ELF tests passed\n";
  } catch (const std::exception& error) {
    std::cerr << "ELF test failure: " << error.what() << '\n';
    return 1;
  }
  return 0;
}
