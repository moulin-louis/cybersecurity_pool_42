//
// Created by loumouli on 9/22/23.
//

#include "IronDome.hpp"

IronDome::IronDome() = default;

IronDome::~IronDome() = default;

void IronDome::parse_args(char **av, int ac) {
  if (ac == 1) {
    cout << "User send 0 path to look at, defaulting to /home" << endl;
    this->look_path.push_back({.path =  "/home", .entropy = 0});
    return ;
  }
  for (uint iter = 1; iter < ac; iter++) {
        struct stat tmp{};
        int retval = stat(av[iter], &tmp);
        if (retval == 0)
          this->look_path.push_back({.path =  av[iter], .entropy = 0});
        else {
          cout << "Invalid path, skipping it" << endl;
        }
  }
  this->calcul_entr_files();
}

void IronDome::calcul_entr_files() {
  for ( auto & file : this->look_path) {
    file.entropy = IronDome::calcul_entr(file.path);
  }
}

double IronDome::calcul_entr(const string &path) {
  char char_seen[256];
  int fd;

  fd = open(path.c_str(), O_RDONLY);
  if (fd == -1)
    throw runtime_error("Write error");
  memset(char_seen, 0, 256);
  string content;
  while (true) {
    char buff[2048];
    uint retval = read(fd, buff, 2047);
    if (retval == -1)
      throw runtime_error("Read error");
    if (retval == 0)
      break;
    buff[retval] = 0;
    content.append(buff);
  }
  for (auto & c : content)
    char_seen[c] += 1;
  
  return 0;
}
