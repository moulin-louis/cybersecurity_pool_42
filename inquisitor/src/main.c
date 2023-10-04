#include "../inc/inquisitor.h"

void init_inqui(t_inquisitor *inquisitor, char **av) {
  inquisitor->ip_src = (int8_t *) av[1];
  inquisitor->mac_src = (int8_t *) av[2];
  inquisitor->ip_target = (int8_t *) av[3];
  inquisitor->mac_target = (int8_t *) av[4];

  inquisitor->sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
  if (inquisitor->sock == -1) {
    dprintf(2, "socket error: %s, file: %s, line: %d: \n", strerror(errno), __FILE__, __LINE__ - 2);
    exit(EXIT_FAILURE);
  }

  inquisitor->ifr.ifr_addr.sa_family = AF_INET;
  strncpy(inquisitor->ifr.ifr_name, "eth0", strlen("eth0") + 1);
  int retval = ioctl(inquisitor->sock, SIOCGIFADDR, &inquisitor->ifr);
  if (retval) {
    dprintf(2, "ioctl error: %s, file: %s, line: %d: \n", strerror(errno), __FILE__, __LINE__ - 2);
  }
  inquisitor->ifr.ifr_ifru.ifru_ivalue = (int)if_nametoindex("eth0");
//  if (retval) {
//    dprintf(2, "ioctl error: %s, file: %s, line: %d: \n", strerror(errno), __FILE__, __LINE__ - 2);
//  }
  retval = ioctl(inquisitor->sock, SIOCGIFHWADDR, &inquisitor->ifr);
  if (retval) {
    dprintf(2, "ioctl error: %s, file: %s, line: %d: \n", strerror(errno), __FILE__, __LINE__ - 2);
  }

  mac_str_to_hex(inquisitor->mac_src, (uint8_t *) inquisitor->mac_src_byte_arr, (const int8_t *) __func__, __LINE__);
  mac_str_to_hex(inquisitor->mac_target, (uint8_t *) inquisitor->mac_target_byte_arr, (const int8_t *) __func__,
                 __LINE__);

  dprintf(1, "my interface_index: %u/%d\n", inquisitor->ifr.ifr_ifru.ifru_ivalue, inquisitor->ifr.ifr_ifru.ifru_ivalue);
  dprintf(1, "should be: %u/%d\n", , if_nametoindex("eth0"));
  dprintf(1, "my ip: %s\n", inet_ntoa(((struct sockaddr_in *) &inquisitor->ifr.ifr_addr)->sin_addr));
  dprintf(1, "my mac address: %02x:%02x:%02x:%02x:%02x:%02x\n",
          (uint8_t) inquisitor->ifr.ifr_hwaddr.sa_data[0],
          (uint8_t) inquisitor->ifr.ifr_hwaddr.sa_data[1],
          (uint8_t) inquisitor->ifr.ifr_hwaddr.sa_data[2],
          (uint8_t) inquisitor->ifr.ifr_hwaddr.sa_data[3],
          (uint8_t) inquisitor->ifr.ifr_hwaddr.sa_data[4],
          (uint8_t) inquisitor->ifr.ifr_hwaddr.sa_data[5]);
  dprintf(1, "\n");
}

int main(int ac, char **av) {
  t_inquisitor inquisitor;
  int retval;

  if (ac != 5)
    usage();
  dprintf(1, "BASE INFO:\n");
  dprintf(1, "MAC_SRC: %s\n", av[2]);
  dprintf(1, "IP_SRC: %s\n", av[1]);
  dprintf(1, "MAC_TARGET: %s\n", av[4]);
  dprintf(1, "IP_TARGET: %s\n", av[3]);
  dprintf(1, "\n");
  init_inqui(&inquisitor, av);
  retval = send_fake_arp_packet_1(&inquisitor);
  if (retval)
    return (printf("Error sending first arp packet"), EXIT_FAILURE);
  close(inquisitor.sock);
}
