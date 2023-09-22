//
// Created by loumouli on 9/22/23.
//

#ifndef IRON_DOME_IRONDOME_HPP
#define IRON_DOME_IRONDOME_HPP

#include <iostream>
#include <vector>
#include <random>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <unistd.h>
using namespace std;

typedef struct s_file {
  string path;
  double entropy;
} t_file;

class IronDome {
public:
  vector<t_file> look_path;
  IronDome();
  ~IronDome();
  void parse_args(char **, int);
  void calcul_entr_files();
  static double calcul_entr(const string &);
};


#endif //IRON_DOME_IRONDOME_HPP
