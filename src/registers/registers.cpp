#include "registers/registers.hpp"

namespace mdbg {

std::optional<std::uint64_t> register_value(const user_regs_struct& r,
                                            std::string_view name) {
#define MDBG_REG(member) \
  if (name == #member) return r.member
  MDBG_REG(rax); MDBG_REG(rbx); MDBG_REG(rcx); MDBG_REG(rdx);
  MDBG_REG(rsi); MDBG_REG(rdi); MDBG_REG(rbp); MDBG_REG(rsp);
  MDBG_REG(r8); MDBG_REG(r9); MDBG_REG(r10); MDBG_REG(r11);
  MDBG_REG(r12); MDBG_REG(r13); MDBG_REG(r14); MDBG_REG(r15);
  MDBG_REG(rip); MDBG_REG(eflags);
#undef MDBG_REG
  return std::nullopt;
}

std::vector<std::pair<std::string_view, std::uint64_t>>
general_purpose_registers(const user_regs_struct& r) {
  return {
      {"rax", r.rax}, {"rbx", r.rbx}, {"rcx", r.rcx}, {"rdx", r.rdx},
      {"rsi", r.rsi}, {"rdi", r.rdi}, {"rbp", r.rbp}, {"rsp", r.rsp},
      {"r8", r.r8},   {"r9", r.r9},   {"r10", r.r10}, {"r11", r.r11},
      {"r12", r.r12}, {"r13", r.r13}, {"r14", r.r14}, {"r15", r.r15},
      {"rip", r.rip}, {"eflags", r.eflags},
  };
}

}  // namespace mdbg
