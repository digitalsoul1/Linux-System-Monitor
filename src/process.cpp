#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return this->pid_; }

float Process::CpuUtilization() { return this->cpuUtilization_; }

string Process::Command() { return this->command_; }

string Process::Ram() { return this->ram_; }

string Process::User() { return this->user_; }

long int Process::UpTime() { return this->uptime_; }

bool Process::operator<(Process const& a) const {
  return this->cpuUtilization_ > a.cpuUtilization_;
}

void Process::SetPid(int pid) { this->pid_ = pid; }
void Process::SetActiveJiffies(unsigned long jiffies) {
  this->activeJiffies_ = jiffies;
};
void Process::SetCommand(std::string command) { this->command_ = command; };
void Process::SetUser(std::string user) { this->user_ = user; };
void Process::SetUpTime(long uptime) { this->uptime_ = uptime; };
void Process::SetCpuUtilization(float utilization) {
  this->cpuUtilization_ = utilization;
};
void Process::SetRam(std::string ram) { this->ram_ = ram; }