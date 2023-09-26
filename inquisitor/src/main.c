#include "inquisitor.h"
#include <pcap.h>

void print_usage(void) {
  printf("./inquisitor <IP-src> <MAC-src> <IP-target> <MAC-target>");
}

int main(int ac, char **av) {
  (void)av;
  if (ac != 5) {
    print_usage();
    return (EXIT_FAILURE);
  }
  return (42);
}
