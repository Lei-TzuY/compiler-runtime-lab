#include "process/process.hpp"

#include "ptrace/ptrace.hpp"

#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
#include <csignal>
#include <cstring>
#include <stdexcept>
#include <utility>

namespace mdbg {
namespace {

std::runtime_error system_error(const char* operation) {
  return std::runtime_error(std::string(operation) + " failed: " + std::strerror(errno));
}

}  // namespace

Process::~Process() { cleanup(); }

Process::Process(Process&& other) noexcept
    : pid_(std::exchange(other.pid_, -1)),
      state_(other.state_),
      exit_code_(other.exit_code_),
      termination_signal_(other.termination_signal_) {}

Process& Process::operator=(Process&& other) noexcept {
  if (this != &other) {
    cleanup();
    pid_ = std::exchange(other.pid_, -1);
    state_ = other.state_;
    exit_code_ = other.exit_code_;
    termination_signal_ = other.termination_signal_;
  }
  return *this;
}

Process Process::launch(const std::string& executable,
                        const std::vector<std::string>& arguments) {
  const pid_t child = ::fork();
  if (child == -1) {
    throw system_error("fork");
  }

  if (child == 0) {
    try {
      lowlevel::traceme();
    } catch (...) {
      _exit(126);
    }

    std::vector<char*> argv;
    argv.reserve(arguments.size() + 2);
    argv.push_back(const_cast<char*>(executable.c_str()));
    for (const auto& argument : arguments) {
      argv.push_back(const_cast<char*>(argument.c_str()));
    }
    argv.push_back(nullptr);
    ::execv(executable.c_str(), argv.data());
    _exit(127);
  }

  Process process(child);
  const auto initial = process.wait();
  if (initial.kind != WaitEvent::Kind::Stopped || initial.value != SIGTRAP) {
    throw std::runtime_error("tracee did not stop with SIGTRAP after exec");
  }
  lowlevel::set_options(child, PTRACE_O_EXITKILL);
  return process;
}

WaitEvent Process::wait() {
  int status = 0;
  pid_t result;
  do {
    result = ::waitpid(pid_, &status, 0);
  } while (result == -1 && errno == EINTR);
  if (result == -1) {
    throw system_error("waitpid");
  }

  if (WIFEXITED(status)) {
    state_ = ProcessState::Exited;
    exit_code_ = WEXITSTATUS(status);
    return {WaitEvent::Kind::Exited, *exit_code_};
  }
  if (WIFSIGNALED(status)) {
    state_ = ProcessState::Signaled;
    termination_signal_ = WTERMSIG(status);
    return {WaitEvent::Kind::Signaled, *termination_signal_};
  }
  if (WIFSTOPPED(status)) {
    state_ = ProcessState::Stopped;
    return {WaitEvent::Kind::Stopped, WSTOPSIG(status)};
  }
  throw std::runtime_error("waitpid returned an unsupported process state");
}

void Process::cleanup() noexcept {
  if (pid_ <= 0 || state_ == ProcessState::Exited || state_ == ProcessState::Signaled) {
    return;
  }
  ::kill(pid_, SIGKILL);
  int status = 0;
  while (::waitpid(pid_, &status, 0) == -1 && errno == EINTR) {
  }
  pid_ = -1;
}

}  // namespace mdbg
