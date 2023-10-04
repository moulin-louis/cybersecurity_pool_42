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

#define HW_TYPE_ETHERNET 0x0001 // 1
#define LEN_HW_ETHERNET 6
#define LEN_PROTO_IPV4 4
#define ETHERNET_DATA_MAX 1500

typedef struct {
  int8_t *ip_src;
  int8_t *mac_src;
  int8_t mac_src_byte_arr[6];
  int8_t *ip_target;
  int8_t *mac_target;
  int8_t mac_target_byte_arr[6];
  int32_t sock;
  struct ifreq ifr;
} t_inquisitor;

typedef struct {
  uint16_t ar_hrd;    /* Format of hardware address.  */
  uint16_t ar_pro;    /* Format of protocol address.  */
  uint8_t ar_hln;    /* Length of hardware address.  */
  uint8_t ar_pln;    /* Length of protocol address.  */
  uint16_t ar_op;    /* ARP opcode (command).  */

  uint8_t ar_sha[ETH_ALEN];  /* Sender hardware address.  */
  uint8_t ar_sip[4];    /* Sender IP address.  */
  uint8_t ar_tha[ETH_ALEN];  /* Target hardware address.  */
  uint8_t ar_tip[4];    /* Target IP address.  */
} t_packet;

#pragma pack(1)
typedef struct {
  uint8_t preamble[7];
  uint8_t sfd;
  uint8_t dest_addr[ETHER_ADDR_LEN];
  uint8_t src_addr[ETHER_ADDR_LEN];
  uint8_t ethertype[ETHER_TYPE_LEN];
  uint8_t data[ETHERNET_DATA_MAX];  // Maximum Ethernet frame payload is 1500 bytes
  uint8_t fcs[4];
} ethernet_frame;
#pragma pack()

void print_packet(const t_packet *packet);

void mac_str_to_hex(int8_t *mac_addr, uint8_t *dest, const int8_t *caller, uint32_t line);

int send_fake_arp_packet_1(t_inquisitor *inquisitor);

void usage(void);

void hexdump(void *data, size_t len, int32_t row);

void decdump(void *data, size_t len, int32_t row);