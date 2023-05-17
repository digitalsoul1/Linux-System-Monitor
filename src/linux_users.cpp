#include "linux_users.h"

#include <filesystem>
#include <fstream>
#include <sstream>

LinuxUsers::LinuxUsers() { this->Update(); };

LinuxUsers* LinuxUsers::GetInstance() {
  {
    if (linuxUsersPtr_ == NULL) {
      linuxUsersPtr_ = new LinuxUsers();
      return linuxUsersPtr_;
    } else
      return linuxUsersPtr_;
  };
};

void LinuxUsers::Update() {
  std::filesystem::path etcpasswd("/etc/passwd");
  std::string line;
  std::string unused;
  std::string user;
  std::string uid;

  std::filesystem::file_time_type lastModified =
      std::filesystem::last_write_time(std::filesystem::path(etcpasswd));

  if (lastModified > this->lastModified_ || this->firstRun == true) {
    std::ifstream passwd_file(etcpasswd.c_str());

    this->users_.clear();
    while (std::getline(passwd_file, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream(line) >> user >> unused >> uid;
      this->users_[std::stoi(uid)] = user;
    };

    this->lastModified_ = std::filesystem::last_write_time(etcpasswd);

    if (this->firstRun) firstRun == false;
  }
}

// Returns an empty string if user was not found
const std::string& LinuxUsers::GetUser(int uid) { return this->users_[uid]; };