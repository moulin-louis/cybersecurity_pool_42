#include "../inc/inquisitor.h"

__attribute__ ((noreturn)) void usage(void) {
  dprintf(2, RED "ERROR: sudo ./inquisitorr <IP-src> <MAC-src> <IP-target> <MAC-target>\n" RESET);
  exit(EXIT_FAILURE);
}

__attribute__ ((noreturn)) void error(const char *func_error, const char *error_msg, const char *file, int line, const char *func_caller) {
  char errno_tmp = errno;
  dprintf(2, RED "ERROR: %s error: %s, caller: %s, file: %s, line: %d" RESET, func_error,
          error_msg ? error_msg : strerror(errno), func_caller, file, line);
  if (g_sock)
    close(g_sock);
  exit(errno_tmp);
}

void hexdump(void *data, size_t len, int32_t row) {
  if (row == 0) {
    for (size_t i = 0; i < len; i++)
      dprintf(1, "%02x ", ((uint8_t *) data)[i]);
    dprintf(1, "\n");
    return;
  }
  for (size_t i = 0; i < len; i += row) {
    for (size_t j = i; j < i + row; j++) {
      if (j == len)
        break;
      dprintf(1, "%02x ", ((uint8_t *) data)[j]);
    }
    dprintf(1, "\n");
  }
  dprintf(1, "\n");
}

void asciidump(void *data, size_t len, int32_t row) {
  if (!row) {
    for (size_t i = 0; i < len; i++) {
      if (!isprint(((uint8_t *) data)[i]))
        dprintf(1, ". ");
      else
        dprintf(1, "%c ", ((uint8_t *) data)[i]);
    }
    dprintf(1, "\n");
    return;
  }
  for (size_t i = 0; i < len; i += row) {
    for (size_t j = i; j < i + row; j++) {
      if (j == len)
        break;
      if (!isprint(((uint8_t *) data)[j]))
        dprintf(1, ". ");
      else
        dprintf(1, "%c ", ((uint8_t *) data)[j]);
    }
    dprintf(1, "\n");
  }
  dprintf(1, "\n");
}

void decdump(void *data, size_t len, int32_t row) {
  if (row == 0) {
    for (size_t i = 0; i < len; i++)
      dprintf(1, "%02d ", ((uint8_t *) data)[i]);
    dprintf(1, "\n");
    return;
  }
  for (size_t i = 0; i < len; i += row) {
    for (size_t j = i; j < i + row; j++) {
      if (j == len)
        break;
      dprintf(1, "%02d ", ((uint8_t *) data)[i]);
    }
    dprintf(1, "\n");
  }
  dprintf(1, "\n");
}

void print_packet(const t_packet *packet) {
  dprintf(1, GREEN);
  dprintf(1, "LOG: ARP Packet:\n");
  dprintf(1, "LOG: Hardware Type: 0x%04x\n", ntohs(packet->ar_hrd));
  dprintf(1, "LOG: Protocol Type: 0x%04x\n", ntohs(packet->ar_pro));
  dprintf(1, "LOG: Hardware Address Length: %u\n", packet->ar_hln);
  dprintf(1, "LOG: Protocol Address Length: %u\n", packet->ar_pln);
  dprintf(1, "LOG: Operation: %u\n", ntohs(packet->ar_op));

  dprintf(1, "LOG: Sender Hardware Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
          (uint8_t) packet->ar_sha[0],
          (uint8_t) packet->ar_sha[1],
          (uint8_t) packet->ar_sha[2],
          (uint8_t) packet->ar_sha[3],
          (uint8_t) packet->ar_sha[4],
          (uint8_t) packet->ar_sha[5]);

  dprintf(1, "LOG: Sender Protocol Address: %u.%u.%u.%u\n",
          (uint8_t) packet->ar_sip[0],
          (uint8_t) packet->ar_sip[1],
          (uint8_t) packet->ar_sip[2],
          (uint8_t) packet->ar_sip[3]);

  dprintf(1, "LOG: Target Hardware Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
          (uint8_t) packet->ar_tha[0],
          (uint8_t) packet->ar_tha[1],
          (uint8_t) packet->ar_tha[2],
          (uint8_t) packet->ar_tha[3],
          (uint8_t) packet->ar_tha[4],
          (uint8_t) packet->ar_tha[5]);

  dprintf(1, "LOG: Target Protocol Address: %u.%u.%u.%u\n",
          (uint8_t) packet->ar_tip[0],
          (uint8_t) packet->ar_tip[1],
          (uint8_t) packet->ar_tip[2],
          (uint8_t) packet->ar_tip[3]);
  dprintf(1, "\n");
  dprintf(1, RESET);
}

void mac_str_to_hex(int8_t *mac_addr, uint8_t *dest) {
  int ret = sscanf((char *) mac_addr, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx", &dest[0], &dest[1], &dest[2], &dest[3],
                   &dest[4], &dest[5]);
  if (ret != 6)
    error("mac_str_to_hex", "failed parsing mac address to byte array", __FILE__, __LINE__, __func__);
}

time_t	gettime(void) {
    struct timeval	tv;
    gettimeofday(&tv, NULL);
    return (((tv.tv_sec * 1000) + (tv.tv_usec / 1000)) - init_time);
}