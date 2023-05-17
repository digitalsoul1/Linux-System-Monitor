#ifndef LINUX_USERS_H
#define LINUX_USERS_H

#include <algorithm>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

class LinuxUsers {
 public:
  void Update();
  const std::string &GetUser(int uid);
  static LinuxUsers *GetInstance();
  static void ResetInstance() { delete linuxUsersPtr_; };

  LinuxUsers(const LinuxUsers &obj) = delete;

 private:
  LinuxUsers();
  static LinuxUsers *linuxUsersPtr_;
  std::map<int, std::string> users_;
  std::filesystem::file_time_type lastModified_;
  bool firstRun = true;
};
#endif