#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <cerrno>
#include <cctype>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <sys/time.h>
#include <csignal>
#include <pcap.h>
#include <atomic>
#include <thread>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/wait.h>
#include <regex>
#define HW_TYPE_ETHERNET 0x0001 // 1
#define LEN_HW_ETHERNET 6
#define LEN_PROTO_IPV4 4

using namespace std;

inline atomic_bool status_loop = true;
inline atomic_bool verbose = false;
inline atomic<pcap_t *> g_handler = nullptr;


typedef struct {
  int8_t *ip_src;
  int8_t *mac_src;
  int8_t mac_src_byte_arr[6];
  int8_t *ip_target;
  int8_t *mac_target;
  int8_t mac_target_byte_arr[6];
  int32_t sock;
  struct ifreq ifr;
  uint32_t index;
} t_inquisitor;

#pragma pack(1)
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
#pragma pack()

#pragma pack(1)
typedef struct {
  uint8_t dest_addr[ETHER_ADDR_LEN];
  uint8_t src_addr[ETHER_ADDR_LEN];
  uint8_t ethertype[ETHER_TYPE_LEN];
  uint8_t data[sizeof(t_packet)];
} ethernet_frame;
typedef struct addr_t addr_t;
#pragma pack()

# define RED "\033[0;31m"
# define GREEN "\033[0;32m"
# define YELLOW "\033[0;33m"
# define RESET "\x1B[0m"

void mac_str_to_hex(int8_t *mac_addr, uint8_t *dest);
void send_fake_arp_packet(t_inquisitor *inquisitor, uint32_t target);
__attribute__((noreturn)) void error(const char *func_error, const char *error_msg, const char *file, int line, const char *func_caller);
void restore_arp_tables(t_inquisitor *inquisitor, uint32_t target);
void capture_packet(t_inquisitor *inquisitor);
