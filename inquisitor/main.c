#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <net/ethernet.h>
#include <netpacket/packet.h>

#define ARP_REPLY 0x0002
#define FIRST_COMPUTER 1
#define SECOND_COMPUTER 2

typedef uint8_t     uchar;
typedef uint16_t    ushort;
typedef uint32_t    uint;
typedef char        *string;

typedef struct {
    ushort  hw_types;
    ushort  po_types;
    uchar   hw_addr_len;
    uchar   po_addr_len;
    ushort  operation;
    uint    sender_hw_addr;
    uint    sender_po_addr;
    uint    target_hw_addr;
    uint    target_po_addr;
} packet_arp;

typedef struct {
    string ip_src;
    string mac_src;
    string ip_target;
    string mac_target;
} args;

__attribute__ ((noreturn)) static void usage(void) {
    printf("with root right:\n");
    printf("./inquisitorr <IP-src> <MAC-src> <IP-target> <MAC-target>\n");
    exit(EXIT_FAILURE);
}

static void parse_args(args *arguments, char **av) {
    arguments->ip_src = av[1];
    arguments->mac_src = av[2];
    arguments->ip_target = av[3];
    arguments->mac_target = av[4];
}

static int send_fake_arp_packet(args* arguments, int computer) {
    (void)arguments;
    (void)computer;
    return (EXIT_SUCCESS);
}

int main(int ac, char **av) {
    if (ac != 5)
        usage();
    packet_arp test_arp;
    args arguments;
    parse_args(&arguments, av);

    //create a socket using arg:
    //AF_PACKET: allow low level packet manipulation
    //SOCK_RAW: raw network protocol access
    //htons(ETH_P_ARP): convert EtherType for ARP into network short,
    //specify protocol used by the socket, ARP here
    int sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if (sock == -1) {
        dprintf(2, "socket error, file: %s, line: %d:, error: %s\n", __FILE__, __LINE__ - 2, strerror(errno));
    }
    struct ifreq ifr;
    strncpy(ifr.ifr_name, "eth0", strlen("eth0") + 1);
    ioctl(sock, SIOCGIFINDEX, &ifr); // grab interface index
    ioctl(sock, SIOCGIFHWADDR, &ifr); //grab hardware address aka MAC
    test_arp.operation = htons(ARP_REPLY);

    send_fake_arp_packet(&arguments, FIRST_COMPUTER);
    send_fake_arp_packet(&arguments, SECOND_COMPUTER);

    close(sock);
}
