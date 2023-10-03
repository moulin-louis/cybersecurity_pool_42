#include "../inc/inquisitor.h"

void base_init_packet(t_packet_arp *packet) {
    packet->hw_types = htons(HW_TYPE_ETHERNET);
    packet->proto_types  = htons(ETH_P_IP);
    packet->hw_addr_len = LEN_HW_ETHERNET;
    packet->proto_addr_len = LEN_PROTO_IPV4;
    packet->operation = htons(ARP_REPLY);
}

void fill_field_packet_1(t_inquisitor *inquisitor, t_packet_arp *packet) {
    mac_str_to_hex(inquisitor->mac_src, (uchar *)packet->sender_hw_addr);
    inet_pton(AF_INET, inet_ntoa(( (struct sockaddr_in *)&inquisitor->ifr.ifr_addr )->sin_addr), &packet->sender_proto_addr);
    mac_str_to_hex(inquisitor->mac_target, (uchar *)packet->target_hw_addr);
    inet_pton(AF_INET, inquisitor->ip_target, &packet->target_proto_addr);
}

void create_ethernet_frame(ethernet_frame *frame,
            const uchar *src_mac,
            const uchar* dst_mac,
            ushort type,
            uchar *data,
            size_t data_len) {
    memcpy(frame->dst_mac, dst_mac, 6);
    memcpy(frame->src_mac, src_mac, 6);
    frame->type_len = htons(type);
    memcpy(frame->data, data, data_len);
}

void create_ethernet_frame_arp(ethernet_frame *frame, uchar *src_mac, uchar *dst_mac, t_packet_arp *packet) {
    uchar buff[sizeof(t_packet_arp)];
    memcpy(buff, packet, sizeof(t_packet_arp)); //serialize arp packet
    create_ethernet_frame(frame, src_mac, dst_mac, 0x0806, buff, sizeof(t_packet_arp));
}

int send_fake_arp_packet_1(t_inquisitor* inquisitor) {
    t_packet_arp packet;
    ethernet_frame frame;
    int byte_sent = 0;
    
    base_init_packet(&packet);
    fill_field_packet_1(inquisitor, &packet);
    print_packet(&packet);

    uchar dst_mac[6];
    memset(dst_mac, 0, sizeof(dst_mac));    
    mac_str_to_hex(inquisitor->mac_target, dst_mac);
    dprintf(1, "HEXDUMP DST_MAC\n");
    hexdump(dst_mac, sizeof(dst_mac), 6);

    uchar src_mac[6] = {0};
    dprintf(1, "HEXDUMP ifr_hwaddr\n");
    hexdump(&inquisitor->ifr.ifr_hwaddr.sa_data, 6, 4);
    mac_str_to_hex(inquisitor->ifr.ifr_hwaddr.sa_data, src_mac);
    dprintf(1, "HEXDUMP SRC_MAC\n");
    hexdump(src_mac, sizeof(src_mac), 6);

    // create_ethernet_frame_arp(&frame, dst_mac, mac, &packet);
    dprintf(1, "HEXDUMP ETHERNET_FRAME:\n");
    hexdump(&frame, sizeof(frame), 4);
    // byte_sent = sendto(inquisitor->sock, &frame, sizeof(ethernet_frame), 0, (struct sockaddr *)&sa, sizeof(sa));
    if (byte_sent == -1) {
        dprintf(2, "sendto error: %s, file: %s, line: %d: \n",  strerror(errno), __FILE__, __LINE__ - 2);
    }
    dprintf(1, "byte_sent = %d\n", byte_sent);
    return (EXIT_SUCCESS);
}