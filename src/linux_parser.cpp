#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "linux_users.h"

LinuxUsers* LinuxUsers ::linuxUsersPtr_ = NULL;
using std::stof;
using std::string;
using std::to_string;
using std::vector;

const long hertz = sysconf(_SC_CLK_TCK);
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  std::string memTotal;
  std::string memFree;
  std::string notUsed;
  std::string line;
  std::ifstream meminfo(kProcDirectory + kMeminfoFilename);
  if (meminfo.is_open()) {
    while (std::getline(meminfo, line)) {
      if (line.find("MemTotal:", 0) == 0) {
        std::istringstream(line) >> notUsed >> memTotal;
        continue;
      } else if (line.find("MemFree", 0) == 0) {
        std::istringstream(line) >> notUsed >> memFree;
        break;
      }
    }
  }

  return ((std::stof(memTotal) - std::stof(memFree)) / std::stof(memTotal));
}

long LinuxParser::UpTime() {
  std::string line;
  std::string uptime;
  std::ifstream uptimeFile(kProcDirectory + kUptimeFilename);

  if (uptimeFile.is_open()) {
    getline(uptimeFile, line);
    std::istringstream(line) >> uptime;
  };

  return std::stol(uptime);
}

long LinuxParser::Jiffies(const std::vector<std::string>& cpuUtilization) {
  return LinuxParser::ActiveJiffies(cpuUtilization) +
         LinuxParser::IdleJiffies(cpuUtilization);
}

long LinuxParser::ActiveJiffies(
    const std::vector<std::string>& cpuUtilization) {
  long kUser = std::stol(cpuUtilization[CPUStates::kUser_]);
  long kNice = std::stol(cpuUtilization[CPUStates::kNice_]);
  long kSystem = std::stol(cpuUtilization[CPUStates::kSystem_]);
  long kIrq = std::stol(cpuUtilization[CPUStates::kIRQ_]);
  long kSoftIRQ = std::stol(cpuUtilization[CPUStates::kSoftIRQ_]);
  long kSteal = std::stol(cpuUtilization[CPUStates::kSteal_]);

  return kUser + kNice + kSystem + kIrq + kSoftIRQ + kSteal;
}

long LinuxParser::ActiveJiffies(int pid) {
  std::vector<std::string> processStats;
  GetProcessStats(pid, processStats);

  return std::stol(processStats[13]);
}

long LinuxParser::IdleJiffies(const std::vector<std::string>& cpuUtilization) {
  long kIdle = std::stol(cpuUtilization[CPUStates::kIdle_]);
  long ioWait = std::stol(cpuUtilization[CPUStates::kIOwait_]);
  return kIdle + ioWait;
}

vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpuUtilization{};

  std::string line;
  std::string notUsed;
  std::string value;

  std::ifstream procStat(kProcDirectory + kStatFilename);

  if (procStat.is_open()) {
    getline(procStat, line);
    std::istringstream lineStream(line);
    lineStream >> notUsed;

    for (int i = 0; i < 10; i++) {
      std::string temp;
      lineStream >> temp;
      cpuUtilization.emplace_back(temp);
    }
  };

  return cpuUtilization;
}

int LinuxParser::TotalProcesses() {
  std::string line;
  std::string not_used;
  std::string temp_processes;

  std::ifstream pidVmSize(kProcDirectory + kStatFilename);
  while (std::getline(pidVmSize, line)) {
    if (line.find("processes") == 0) {
      std::istringstream(line) >> not_used >> temp_processes;
      break;
    }
  }

  return std::stoi(temp_processes);
}

int LinuxParser::RunningProcesses() {
  std::string line;
  std::string not_used;
  std::string temp_running;

  std::ifstream pidVmSize(kProcDirectory + kStatFilename);
  while (std::getline(pidVmSize, line)) {
    if (line.find("procs_running") == 0) {
      std::istringstream(line) >> not_used >> temp_running;
      break;
    }
  }

  return std::stoi(temp_running);
}

string LinuxParser::Command(int pid) {
  string line;
  string cmd;

  std::ifstream pidCmd(kProcDirectory + std::to_string(pid) + "/" +
                       kCmdlineFilename);

  std::getline(pidCmd, line);
  pidCmd.close();
  return line;
}

string LinuxParser::Ram(int pid) {
  std::string line;
  std::string not_used;
  std::string temp_ram{};

  std::ifstream pidVmSize(kProcDirectory + std::to_string(pid) +
                          kStatusFilename);
  if (pidVmSize.is_open()) {
    while (std::getline(pidVmSize, line)) {
      if (line.find("VmSize:") == 0) {
        std::istringstream(line) >> not_used >> temp_ram;
        break;
      }
    }
  }

  return std::to_string(std::stoi(temp_ram) / 1000);
}

string LinuxParser::Uid(int pid) {
  std::string line;
  std::string not_used;
  std::string uid{};

  std::ifstream pidUid(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (pidUid.is_open()) {
    while (std::getline(pidUid, line)) {
      if (line.find("Uid:") == 0) {
        std::istringstream(line) >> not_used >> uid;
        break;
      }
    }
  }

  return uid;
}

string LinuxParser::User(int pid) {
  return LinuxUsers::GetInstance()->GetUser(std::stoi(LinuxParser::Uid(pid)));
}

long LinuxParser::UpTime(int pid) {
  std::vector<std::string> processStats;
  GetProcessStats(pid, processStats);
  long uTime = std::stol(processStats[13]);
  // utime
  long sTime = std::stol(processStats[14]);
  // cutime
  long cuTime = std::stol(processStats[15]);
  // cstime
  long csTime = std::stol(processStats[16]);
  // starttime
  long startTime = std::stol(processStats[21]);

  long totalTime = uTime + sTime;
  totalTime = totalTime + cuTime + csTime;

  long seconds = LinuxParser::UpTime() - (startTime / hertz);

  return seconds;
}

void LinuxParser::GetProcessStats(
    const int& pid, std::vector<std::string>& processStatsVector) {
  std::string buffer;
  std::ifstream processUptime(kProcDirectory + std::to_string(pid) +
                              kStatFilename);

  std::getline(processUptime, buffer);

  size_t currentPosition = 1;
  size_t controlPosition = 0;

  while (controlPosition != string::npos) {
    currentPosition = buffer.find(' ', currentPosition);

    std::string data =
        buffer.substr(controlPosition == 0 ? 0 : controlPosition + 1,
                      currentPosition - controlPosition);

    processStatsVector.emplace_back(data);
    controlPosition = currentPosition;
    currentPosition++;
  };
}

float LinuxParser::Cpu(int pid) {
  std::vector<std::string> processStats;
  GetProcessStats(pid, processStats);
  // utime
  long uTime = std::stol(processStats[13]);
  // utime
  long sTime = std::stol(processStats[14]);
  // cutime
  long cuTime = std::stol(processStats[15]);
  // cstime
  long csTime = std::stol(processStats[16]);
  // starttime
  long startTime = std::stol(processStats[21]);

  // total_time
  long totalTime = uTime + sTime;
  totalTime = totalTime + cuTime + csTime;

  // number of clock per tick
  float seconds = LinuxParser::UpTime() - (float)(startTime / hertz);

  float cpuUtilization = (seconds == 0 ? 0 : (totalTime / hertz) / seconds);
  return cpuUtilization;
}