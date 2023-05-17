#include "processor.h"

#include "linux_parser.h"

float Processor::Utilization() {
  auto cpuUtilization = LinuxParser::CpuUtilization();
  float idleJiffies = LinuxParser::IdleJiffies(cpuUtilization);
  float activeJiffies = LinuxParser::ActiveJiffies(cpuUtilization);
  float total = idleJiffies + activeJiffies;

  return (total - idleJiffies) / total;
}