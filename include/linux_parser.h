#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>

#include "linux_users.h"

namespace LinuxParser {
// Paths
const std::string kProcDirectory{"/proc/"};
const std::string kCmdlineFilename{"/cmdline"};
const std::string kCpuinfoFilename{"/cpuinfo"};
const std::string kStatusFilename{"/status"};
const std::string kStatFilename{"/stat"};
const std::string kUptimeFilename{"/uptime"};
const std::string kMeminfoFilename{"/meminfo"};
const std::string kVersionFilename{"/version"};
const std::string kOSPath{"/etc/os-release"};
const std::string kPasswordPath{"/etc/passwd"};
const auto linuxUsers = LinuxUsers::GetInstance();

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};
std::vector<std::string> CpuUtilization();
long Jiffies(const std::vector<std::string> &cpuUtilization);
long ActiveJiffies(const std::vector<std::string> &cpuUtilization);
long ActiveJiffies(int pid);
long IdleJiffies(const std::vector<std::string> &cpuUtilization);

// Processes
std::string Command(int pid);
std::string Ram(int pid);
float Cpu(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);
void GetProcessStats(const int &pid,
                     std::vector<std::string> &processStatsVector);
};  // namespace LinuxParser

#endif