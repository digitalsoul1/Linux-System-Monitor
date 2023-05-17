#include "linux_system.h"

#include "linux_parser.h"

void LinuxSystem::Update() {
  // Set OS name & version;
  this->operatingSystem_ = LinuxParser::OperatingSystem();
  this->operatingSystemVersion_ = LinuxParser::Kernel();

  // Populate the pid vector with PIDs on the system
  this->pids_.clear();
  this->pids_ = LinuxParser::Pids();

  this->processes_.clear();
  this->processes_ = {};
  for (unsigned long int i = 0; i < this->pids_.size(); i++) {
    // Prevent crashes caused by processes that died / were closed
    // while executing this loop
    try {
      Process p;
      int temp_pid = pids_[i];
      p.SetPid(temp_pid);
      p.SetCommand(LinuxParser::Command(temp_pid));
      LinuxUsers::GetInstance()->Update();
      p.SetUser(LinuxParser::User(temp_pid));
      p.SetRam(LinuxParser::Ram(temp_pid));
      p.SetCpuUtilization(LinuxParser::Cpu(temp_pid));
      p.SetUpTime(LinuxParser::UpTime(temp_pid));
      p.SetActiveJiffies(LinuxParser::ActiveJiffies(temp_pid));

      this->processes_.emplace_back(p);
    } catch (...) {
      continue;
    };
  };

  std::sort(processes_.begin(), processes_.end());
}

std::string LinuxSystem::OperatingSystem() { return this->operatingSystem_; }

Processor& LinuxSystem::Cpu() { return this->processor_; }

std::vector<Process>& LinuxSystem::Processes() { return this->processes_; }

std::string LinuxSystem::OperatingSystemVersion() {
  return this->operatingSystemVersion_;
}

float LinuxSystem::MemoryUtilization() {
  return LinuxParser::MemoryUtilization();
}

int LinuxSystem::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int LinuxSystem::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int LinuxSystem::UpTime() { return LinuxParser::UpTime(); }