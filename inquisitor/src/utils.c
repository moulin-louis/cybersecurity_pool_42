#include "../inc/inquisitor.h"

__attribute__ ((noreturn)) void usage(void) {
    dprintf(2, "with root right:\n");
    dprintf(2, "./inquisitorr <IP-src> <MAC-src> <IP-target> <MAC-target>\n");
    exit(EXIT_FAILURE);
}

void hexdump(void *data, size_t len, int row) {
    for (size_t i = 0; i < len; i += row) {
        for (size_t j = i; j < i+row; j++) {
            dprintf(1, "%02x ", ((uchar *)data)[j]);
        }
        dprintf(1, "\n");
    }
    dprintf(1, "\n");
}

void print_packet(const t_packet_arp *packet) {
    dprintf(1, "ARP Packet:\n");
    dprintf(1, "Hardware Type: 0x%04x\n", ntohs(packet->hw_types));
    dprintf(1, "Protocol Type: 0x%04x\n", ntohs(packet->proto_types));
    dprintf(1, "Hardware Address Length: %u\n", packet->hw_addr_len);
    dprintf(1, "Protocol Address Length: %u\n", packet->proto_addr_len);
    dprintf(1, "Operation: %u\n", ntohs(packet->operation));
    
    dprintf(1, "Sender Hardware Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
           (uchar) packet->sender_hw_addr[0],
           (uchar) packet->sender_hw_addr[1],
           (uchar) packet->sender_hw_addr[2],
           (uchar) packet->sender_hw_addr[3],
           (uchar) packet->sender_hw_addr[4],
           (uchar) packet->sender_hw_addr[5]);
           
    dprintf(1, "Sender Protocol Address: %u.%u.%u.%u\n",
           (uchar) packet->sender_proto_addr[0],
           (uchar) packet->sender_proto_addr[1],
           (uchar) packet->sender_proto_addr[2],
           (uchar) packet->sender_proto_addr[3]);
           
    dprintf(1, "Target Hardware Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
           (uchar) packet->target_hw_addr[0],
           (uchar) packet->target_hw_addr[1],
           (uchar) packet->target_hw_addr[2],
           (uchar) packet->target_hw_addr[3],
           (uchar) packet->target_hw_addr[4],
           (uchar) packet->target_hw_addr[5]);
           
    dprintf(1, "Target Protocol Address: %u.%u.%u.%u\n",
           (uchar) packet->target_proto_addr[0],
           (uchar) packet->target_proto_addr[1],
           (uchar) packet->target_proto_addr[2],
           (uchar) packet->target_proto_addr[3]);
    dprintf(1, "\n");
}

void mac_str_to_hex(char *mac_addr, uchar *dest) {
    int ret = sscanf(mac_addr, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx", &dest[0], &dest[1], &dest[2], &dest[3], &dest[4], &dest[5]);
    if (ret != 6) {
        dprintf(2, "Error parsing mac address to byte array, file: %s, line: %d:, \n", __FILE__, __LINE__ - 2);
    }
}