#include "../inc/inquisitor.h"

__attribute__ ((noreturn)) void usage(void) {
  dprintf(2, "sudo ./inquisitorr <IP-src> <MAC-src> <IP-target> <MAC-target>\n");
  exit(EXIT_FAILURE);
}

void hexdump(void *data, size_t len, int row) {
  for (size_t i = 0; i < len; i += row) {
    for (size_t j = i; j < i + row; j++) {
      if (j == len) {
        break;
      }
      dprintf(1, "%02x ", ((uint8_t *) data)[j]);
    }
    dprintf(1, "\n");
  }
  dprintf(1, "\n");
}

void decdump(void *data, size_t len, int row) {
  for (size_t i = 0; i < len; i += row) {
    for (size_t j = i; j < i + row; j++) {
      if (j == len) {
        break;
      }
      dprintf(1, "%02d ", ((uint8_t *) data)[j]);
    }
    dprintf(1, "\n");
  }
  dprintf(1, "\n");
}

void print_packet(const t_packet *packet) {
  dprintf(1, "ARP Packet:\n");
  dprintf(1, "Hardware Type: 0x%04x\n", ntohs(packet->ar_hrd));
  dprintf(1, "Protocol Type: 0x%04x\n", ntohs(packet->ar_pro));
  dprintf(1, "Hardware Address Length: %u\n", packet->ar_hln);
  dprintf(1, "Protocol Address Length: %u\n", packet->ar_pln);
  dprintf(1, "Operation: %u\n", ntohs(packet->ar_op));

  dprintf(1, "Sender Hardware Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
          (uint8_t) packet->ar_sha[0],
          (uint8_t) packet->ar_sha[1],
          (uint8_t) packet->ar_sha[2],
          (uint8_t) packet->ar_sha[3],
          (uint8_t) packet->ar_sha[4],
          (uint8_t) packet->ar_sha[5]);

  dprintf(1, "Sender Protocol Address: %u.%u.%u.%u\n",
          (uint8_t) packet->ar_sip[0],
          (uint8_t) packet->ar_sip[1],
          (uint8_t) packet->ar_sip[2],
          (uint8_t) packet->ar_sip[3]);

  dprintf(1, "Target Hardware Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
          (uint8_t) packet->ar_tha[0],
          (uint8_t) packet->ar_tha[1],
          (uint8_t) packet->ar_tha[2],
          (uint8_t) packet->ar_tha[3],
          (uint8_t) packet->ar_tha[4],
          (uint8_t) packet->ar_tha[5]);

  dprintf(1, "Target Protocol Address: %u.%u.%u.%u\n",
          (uint8_t) packet->ar_tip[0],
          (uint8_t) packet->ar_tip[1],
          (uint8_t) packet->ar_tip[2],
          (uint8_t) packet->ar_tip[3]);
  dprintf(1, "\n");
}

void mac_str_to_hex(int8_t *mac_addr, uint8_t *dest, const int8_t *caller, uint32_t line) {
  int ret = sscanf((char *) mac_addr, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx", &dest[0], &dest[1], &dest[2], &dest[3],
                   &dest[4], &dest[5]);
  if (ret != 6) {
    dprintf(2, "Error parsing mac address to byte array, file: %s, line: %d:, caller: %s:%d\n", __FILE__, __LINE__ - 2,
            caller, line);
  }
}