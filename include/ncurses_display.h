#ifndef NCURSES_DISPLAY_H
#define NCURSES_DISPLAY_H

#include <curses.h>

#include "system.h"

class NCursesDisplay {
 public:
  void Display(System& system, int n = 10);

 private:
  void DisplaySystem(System& system, WINDOW* window);
  void DisplayProcesses(std::vector<Process>& processes, WINDOW* window, int n);
  std::string ProgressBar(float percent);
  // BetterMvwprintw fixes screen tearing for text in columns on screen
  // refresh
  int BetterMvwprintw(WINDOW* w, int row, int column, int next_column,
                      std::string text);

  class CursesString {
   public:
    CursesString(const size_t& length);
    void InjectString(const std::string& s);
    const char* Get();

   private:
    std::string cursesString_{};
    CursesString();
  };
  /////////////////////////////////////////////////////////////////////////
};

#endif