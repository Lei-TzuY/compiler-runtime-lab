#pragma once

#include <sys/types.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace mdbg {

struct ElfSymbol {
  std::string name;
  std::uint64_t value;
  std::uint64_t size;
  unsigned char type;
  unsigned char binding;
};

struct ResolvedSymbol {
  ElfSymbol symbol;
  std::uint64_t offset;
};

class ElfFile {
 public:
  explicit ElfFile(std::string path);

  [[nodiscard]] const std::string& path() const noexcept { return path_; }
  [[nodiscard]] bool is_pie() const noexcept;
  [[nodiscard]] const std::vector<ElfSymbol>& symbols() const noexcept { return symbols_; }
  [[nodiscard]] std::optional<ElfSymbol> find_symbol(std::string_view name) const;
  [[nodiscard]] std::optional<ResolvedSymbol> find_symbol_by_virtual_address(
      std::uint64_t address) const;

  [[nodiscard]] std::uint64_t load_bias(pid_t pid) const;
  [[nodiscard]] std::uint64_t runtime_address(pid_t pid, const ElfSymbol& symbol) const;
  [[nodiscard]] std::optional<ResolvedSymbol> find_symbol_by_runtime_address(
      pid_t pid, std::uint64_t address) const;

 private:
  void parse();

  std::string path_;
  std::vector<std::byte> bytes_;
  std::vector<ElfSymbol> symbols_;
  std::uint16_t elf_type_{0};
  std::uint64_t zero_offset_load_vaddr_{0};
};

}  // namespace mdbg
