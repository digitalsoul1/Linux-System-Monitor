#include "ncurses_display.h"

#include <curses.h>

#include <algorithm>
#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include "format.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::string;
using std::to_string;

// 50 bars uniformly displayed from 0 - 100 %
// 2% is one bar(|)
std::string NCursesDisplay::ProgressBar(float percent) {
  std::string result{"0%"};
  int size{50};
  float bars{percent * size};

  for (int i{0}; i < size; ++i) {
    result += i <= bars ? '|' : ' ';
  }

  string display{to_string(percent * 100).substr(0, 4)};
  if (percent < 0.1 || percent == 1.0)
    display = " " + to_string(percent * 100).substr(0, 3);
  return result + " " + display + "/100%";
}

void NCursesDisplay::DisplaySystem(System& system, WINDOW* window) {
  int row{0};
  mvwprintw(window, ++row, 2, "%s",
            ("OS: " + system.OperatingSystem()).c_str());
  mvwprintw(window, ++row, 2, "%s",
            ("Kernel: " + system.OperatingSystemVersion()).c_str());
  mvwprintw(window, ++row, 2, "%s", "CPU: ");
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 10, "%s", "");
  wprintw(window, "%s", this->ProgressBar(system.Cpu().Utilization()).c_str());
  wattroff(window, COLOR_PAIR(1));
  mvwprintw(window, ++row, 2, "%s", "Memory: ");
  wattron(window, COLOR_PAIR(1));
  mvwprintw(window, row, 10, "%s", "");
  wprintw(window, "%s", this->ProgressBar(system.MemoryUtilization()).c_str());
  wattroff(window, COLOR_PAIR(1));
  this->BetterMvwprintw(
      window, ++row, 2, window->_maxx - 1,
      ("Total Processes: " + to_string(system.TotalProcesses())));
  this->BetterMvwprintw(
      window, ++row, 2, window->_maxx - 1,
      ("Running Processes: " + to_string(system.RunningProcesses())));
  mvwprintw(window, ++row, 2, "%s",
            ("Up Time: " + Format::ElapsedTime(system.UpTime())).c_str());
  wrefresh(window);
}

#include <iostream>
void NCursesDisplay::DisplayProcesses(std::vector<Process>& processes,
                                      WINDOW* window, int n) {
  int row{0};
  int const pid_column{2};
  int const user_column{9};
  int const cpu_column{16};
  int const ram_column{26};
  int const time_column{35};
  int const command_column{46};
  wattron(window, COLOR_PAIR(2));
  mvwprintw(window, ++row, pid_column, "PID");
  mvwprintw(window, row, user_column, "USER");
  mvwprintw(window, row, cpu_column, "CPU[%%]");
  mvwprintw(window, row, ram_column, "RAM[MB]");
  mvwprintw(window, row, time_column, "TIME+");
  mvwprintw(window, row, command_column, "COMMAND");
  wattroff(window, COLOR_PAIR(2));
  int const num_processes = int(processes.size()) > n ? n : processes.size();
  for (int i = 0; i < num_processes; ++i) {
    this->BetterMvwprintw(window, ++row, pid_column, user_column,
                          std::to_string(processes[i].Pid()));
    this->BetterMvwprintw(window, row, user_column, cpu_column,
                          processes[i].User());
    float cpu = processes[i].CpuUtilization() * 100;
    this->BetterMvwprintw(window, row, cpu_column, ram_column,
                          std::to_string(cpu).substr(0, 4));
    this->BetterMvwprintw(window, row, ram_column, time_column,
                          processes[i].Ram());
    this->BetterMvwprintw(window, row, time_column, command_column,
                          Format::ElapsedTime(processes[i].UpTime()));
    this->BetterMvwprintw(window, row, command_column, window->_maxx - 1,
                          processes[i].Command());
  }
}

void NCursesDisplay::Display(System& system, int n) {
  initscr();      // start ncurses
  noecho();       // do not print input values
  start_color();  // enable color

  int x_max{getmaxx(stdscr)};
  WINDOW* system_window = newwin(9, x_max - 1, 0, 0);
  WINDOW* process_window =
      newwin(3 + n, x_max - 1, system_window->_maxy + 1, 0);

  while (1) {
    system.Update();
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    box(system_window, 0, 0);
    box(process_window, 0, 0);
    this->DisplaySystem(system, system_window);
    this->DisplayProcesses(system.Processes(), process_window, n);
    wrefresh(system_window);
    wrefresh(process_window);
    doupdate();
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  endwin();
}

int NCursesDisplay::BetterMvwprintw(WINDOW* w, int row, int column,
                                    int next_column, std::string text) {
  text.erase(std::find(text.begin(), text.end(), '\0'), text.end());
  size_t charDistance = next_column - column;

  CursesString cursesString(charDistance);
  cursesString.InjectString(text);

  return mvwprintw(w, row, column, "%s", cursesString.Get());
}

NCursesDisplay::CursesString::CursesString(const size_t& length) {
  this->cursesString_.resize(length);
  std::fill(this->cursesString_.begin(), this->cursesString_.end(), ' ');
}

void NCursesDisplay::CursesString::InjectString(const std::string& s) {
  size_t sLength = s.length() >= this->cursesString_.length()
                       ? this->cursesString_.length()
                       : s.length();
  this->cursesString_.replace(0, sLength, s.substr(0, sLength));
}

const char* NCursesDisplay::CursesString::Get() {
  return this->cursesString_.c_str();
}