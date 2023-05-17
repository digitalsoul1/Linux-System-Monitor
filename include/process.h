#ifndef PROCESS_H
#define PROCESS_H

#include <string>

class Process {
 public:
  int Pid();                               // TODO: See src/process.cpp
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization();                  // TODO: See src/process.cpp
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process const &a) const;  // TODO: See src/process.cpp
  void SetPid(int i);
  void SetActiveJiffies(unsigned long);
  void SetUser(std::string user);
  void SetCommand(std::string command);
  void SetCpuUtilization(float utlization);
  void SetRam(std::string ram);
  void SetUpTime(long int uptime);

  // TODO: Declare any necessary private members
 private:
  int pid_;
  std::string user_;
  std::string command_;
  float cpuUtilization_;
  std::string ram_;
  long int uptime_;
  long activeJiffies_;
};

#endif