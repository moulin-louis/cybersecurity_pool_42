
#include "IronDome.hpp"

const int SLEEP_INTERVAL = 2;

bool do_heartbeat() {

}

void init_daemon() {
  pid_t pid, sid;

  uid_t uid = getuid();
  if (uid != 0) {
    cout << "Program should be run as root." << endl;
    exit(EXIT_FAILURE);
  }
  pid = fork();
  if(pid > 0) {
    cout << "Child created, parent exiting.." << endl;
    exit(EXIT_SUCCESS);
  }
  else if(pid < 0) {
    cout << strerror(errno) << endl;
    exit(EXIT_FAILURE);
  }
  umask(0);

  openlog("daemon-named", LOG_NOWAIT | LOG_PID, LOG_USER);
  syslog(LOG_NOTICE, "Successfully started daemon-name");

  sid = setsid();
  if(sid < 0) {
    cout << "Could not generate session ID for child process" << endl;
    exit(EXIT_FAILURE);
  }
}

// For security purposes, we don't allow any arguments to be passed into the daemon
int main(int ac, char **av) {
  IronDome ir_dome;
//  init_daemon();
  cout << "Daemon running now..." << endl;
  ir_dome.parse_args(av, ac);
  while(true) {
    if (do_heartbeat())
      break;
    sleep(SLEEP_INTERVAL);
  }
  syslog(LOG_NOTICE, "Stopping daemon-name");
  closelog();
  exit(EXIT_SUCCESS);
}