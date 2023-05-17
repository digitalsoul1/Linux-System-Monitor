
#include <curses.h>

#include <csignal>
#include <iostream>

#include "linux_parser.h"
#include "linux_system.h"
#include "linux_users.h"
#include "ncurses_display.h"

void ExitProgram(int n) {
  LinuxUsers::ResetInstance();
  endwin();
  std::exit(0);
};

int main() {
  Processor linuxProcessor;
  std::vector<int> pids = LinuxParser::Pids();
  LinuxSystem system(linuxProcessor, pids);
  signal(SIGTERM, ExitProgram);
  signal(SIGINT, ExitProgram);
  NCursesDisplay ncCursesDisplay;
  ncCursesDisplay.Display(system);

  return 0;
}