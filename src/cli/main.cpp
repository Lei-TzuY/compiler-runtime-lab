#include "debugger/debugger.hpp"
#include "registers/registers.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

std::uintptr_t parse_address(const std::string& text) {
  std::size_t consumed = 0;
  const auto value = std::stoull(text, &consumed, 0);
  if (consumed != text.size()) {
    throw std::invalid_argument("invalid address");
  }
  return static_cast<std::uintptr_t>(value);
}

void print_stop(const mdbg::StopInfo& info) {
  using mdbg::StopReason;
  switch (info.reason) {
    case StopReason::InitialExec: std::cout << "stopped after exec\n"; break;
    case StopReason::Breakpoint:
      std::cout << "breakpoint at 0x" << std::hex << *info.breakpoint_address << std::dec << '\n';
      break;
    case StopReason::SingleStep: std::cout << "single-step trap\n"; break;
    case StopReason::Signal: std::cout << "stopped by signal " << info.value << '\n'; break;
    case StopReason::Trap: std::cout << "SIGTRAP (not a managed breakpoint)\n"; break;
    case StopReason::Exited: std::cout << "process exited with code " << info.value << '\n'; break;
    case StopReason::Signaled: std::cout << "process terminated by signal " << info.value << '\n'; break;
  }
}

}  // namespace

int main(int argc, char** argv) {
  if (argc < 2) {
    std::cerr << "usage: mdbg <program> [args...]\n";
    return 2;
  }

  std::vector<std::string> args;
  for (int i = 2; i < argc; ++i) args.emplace_back(argv[i]);

  try {
    auto debugger = mdbg::Debugger::launch(argv[1], args);
    print_stop(debugger.stop_info());

    std::string line;
    while (debugger.state() == mdbg::ProcessState::Stopped &&
           std::cout << "(mdbg) " && std::getline(std::cin, line)) {
      std::istringstream input(line);
      std::string command;
      input >> command;
      if (command.empty()) continue;
      if (command == "quit" || command == "q") break;
      if (command == "continue" || command == "c") {
        print_stop(debugger.continue_execution());
      } else if (command == "stepi" || command == "si") {
        print_stop(debugger.single_step());
      } else if (command == "regs") {
        for (const auto& [name, value] : mdbg::general_purpose_registers(debugger.registers())) {
          std::cout << std::setw(6) << name << " 0x" << std::hex << value << std::dec << '\n';
        }
      } else if (command == "reg") {
        std::string name;
        input >> name;
        const auto value = mdbg::register_value(debugger.registers(), name);
        if (!value) std::cout << "unknown register\n";
        else std::cout << name << " = 0x" << std::hex << *value << std::dec << '\n';
      } else if (command == "x") {
        std::string address_text;
        std::size_t length = 8;
        input >> address_text >> length;
        const auto address = parse_address(address_text);
        const auto bytes = debugger.read_memory(address, length);
        std::cout << "0x" << std::hex << address << ":";
        for (const auto byte : bytes) {
          std::cout << ' ' << std::setw(2) << std::setfill('0')
                    << std::to_integer<unsigned>(byte);
        }
        std::cout << std::setfill(' ') << std::dec << '\n';
      } else if (command == "break" || command == "b") {
        std::string address_text;
        input >> address_text;
        const auto address = parse_address(address_text);
        const auto id = debugger.add_breakpoint(address);
        std::cout << "Breakpoint " << id << " at 0x" << std::hex << address << std::dec << '\n';
      } else if (command == "delete") {
        std::size_t id = 0;
        input >> id;
        if (!debugger.remove_breakpoint(id)) std::cout << "no such breakpoint\n";
      } else if (command == "info") {
        std::string topic;
        input >> topic;
        if (topic != "breakpoints") {
          std::cout << "usage: info breakpoints\n";
          continue;
        }
        for (const auto& bp : debugger.breakpoints()) {
          std::cout << bp.id << " 0x" << std::hex << bp.address << std::dec
                    << (bp.installed ? " enabled" : " temporarily-restored") << '\n';
        }
      } else {
        std::cout << "commands: continue, stepi, regs, reg <name>, x <addr> [len], "
                     "break <addr>, delete <id>, info breakpoints, quit\n";
      }
    }
  } catch (const std::exception& error) {
    std::cerr << "mdbg: " << error.what() << '\n';
    return 1;
  }
  return 0;
}
