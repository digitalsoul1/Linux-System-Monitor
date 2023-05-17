#include "system.h"

class LinuxSystem : public System {
 public:
  void Update();
  Processor& Cpu();
  std::vector<Process>& Processes() override;
  float MemoryUtilization();
  long UpTime();
  int TotalProcesses();
  int RunningProcesses();
  std::string OperatingSystemVersion();
  std::string OperatingSystem();
  LinuxSystem(Processor& linuxProcessor, std::vector<int>& pids)
      : processor_(linuxProcessor), pids_(pids) {
    this->Update();
  };

 private:
  Processor& processor_;
  std::vector<Process> processes_ = {};
  std::string operatingSystemVersion_{};
  std::string operatingSystem_{};
  std::vector<int>& pids_;
};