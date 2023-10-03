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
#include <linux/if_packet.h>

#define ARP_REQUEST 0x0001 // 1
#define ARP_REPLY   0x0002 // 2
#define HW_TYPE_ETHERNET 0x0001 // 1
#define HW_TYPE_IEEE_802 0x0006 //6
#define LEN_HW_ETHERNET 6
#define LEN_PROTO_IPV4 4


typedef uint8_t     uchar;
typedef uint16_t    ushort;
typedef uint32_t    uint;

typedef struct {
    char    *ip_src;
    char    *mac_src;
    char    *ip_target;
    char    *mac_target;
    struct  ifreq ifr;
    int     sock;
} t_inquisitor;

typedef struct {
    ushort  hw_types;
    ushort  proto_types;
    uchar   hw_addr_len;
    uchar   proto_addr_len;
    ushort  operation;
    char    sender_hw_addr[6];
    char    sender_proto_addr[4];
    char    target_hw_addr[6];
    char    target_proto_addr[4];
} t_packet_arp;

typedef struct {
    uchar   dst_mac[6];
    uchar   src_mac[6];
    ushort  type_len;
    uchar   data[sizeof(t_packet_arp) + 1];
} ethernet_frame;

void print_packet(const t_packet_arp *packet);
void mac_str_to_hex(char *mac_addr, uchar *dest);
int send_fake_arp_packet_1(t_inquisitor* inquisitor);
void usage(void);
void hexdump(void *data, size_t len, int row);