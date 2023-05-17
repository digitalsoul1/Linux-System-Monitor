#include "format.h"

#include <string>

using std::string;

string Format::ElapsedTime(long seconds) {
  char elapsedTime[26];
  long hours = seconds / 3600;
  long minutes = (seconds % 3600) / 60;
  seconds = (seconds % 60);
  std::sprintf(elapsedTime, "%02ld:%02ld:%02ld", hours, minutes, seconds);

  return string(elapsedTime);
}