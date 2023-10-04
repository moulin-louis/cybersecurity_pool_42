#include "../inc/inquisitor.h"

void base_init_packet(t_packet *packet) {
  packet->ar_hrd = htons(HW_TYPE_ETHERNET);
  packet->ar_pro = htons(ETH_P_IP);
  packet->ar_hln = LEN_HW_ETHERNET;
  packet->ar_pln = LEN_PROTO_IPV4;
  packet->ar_op = htons(ARPOP_REPLY);
}

void fill_field_packet_1(t_inquisitor *inquisitor, t_packet *packet) {
  uint32_t int_addr;
  memcpy(packet->ar_sha, inquisitor->ifr.ifr_hwaddr.sa_data, 6); //my mac address
  int_addr = inet_addr((const char *) inquisitor->ip_src);
  memcpy(packet->ar_sip, &int_addr, 4); //src ip target
  memcpy(packet->ar_tha, inquisitor->mac_target_byte_arr, 6); //target mac address
  int_addr = inet_addr((const char *) inquisitor->ip_target);
  memcpy(packet->ar_tip, &int_addr, 4); //target ip address
}

void init_ether_frame(ethernet_frame *frame, t_packet *packet) {
  memset(frame, 0, sizeof(*frame));
  //init preamble and sfd to help with synchronization
  memset(frame->preamble, 0xAA, 7);
  frame->sfd = 0xD5;
  memcpy(frame->dest_addr, packet->ar_tha, 6);
  memcpy(frame->src_addr, packet->ar_sha, 6);
  *(uint16_t *) frame->ethertype = 0x0806;
  memcpy(frame->data, (uint8_t *) packet, sizeof(*packet));
  memset(frame->data + sizeof(*packet), 0, 1500 - sizeof(*packet));
  memset(frame->fcs, 0, 4);
}

int send_fake_arp_packet_1(t_inquisitor *inquisitor) {
  t_packet packet;
  ethernet_frame frame;
  struct sockaddr_ll dest_addr;
  ssize_t byte_sent;

  base_init_packet(&packet);
  fill_field_packet_1(inquisitor, &packet);
//  dprintf(1, "HEXDUMP ARP_PACKET\n");
//  hexdump(&packet, sizeof(packet), sizeof(packet));
//  print_packet(&packet);
  init_ether_frame(&frame, &packet);
//  dprintf(1, "HEXDUMP ETHERNET FRAME\n");
//  hexdump(&frame, sizeof(frame), sizeof(frame));
  memset(&dest_addr, 0, sizeof(dest_addr));
  dest_addr.sll_family = AF_PACKET;
  dest_addr.sll_protocol = htons(ETH_P_ARP);
  dest_addr.sll_ifindex = inquisitor->ifr.ifr_ifindex;
  dest_addr.sll_hatype = htons(ARPHRD_ETHER);
  dest_addr.sll_halen = ETH_ALEN;
  dest_addr.sll_pkttype = PACKET_OTHERHOST;
  memcpy(dest_addr.sll_addr, packet.ar_tha, ETH_ALEN);
  dprintf(1, "DECDUMP DEST_ADDR\n");
  decdump(&dest_addr, sizeof(dest_addr), sizeof(dest_addr));
  byte_sent = sendto(inquisitor->sock, &frame, sizeof(frame), 0, (struct sockaddr *) &dest_addr, sizeof(dest_addr));
  if (byte_sent == -1) {
    dprintf(1, "sendto error: %s, file: %s, line: %d: \n", strerror(errno), __FILE__, __LINE__ - 2);
    return (EXIT_FAILURE);
  }
  dprintf(1, "byte_sent = %ld\n", byte_sent);
  return (EXIT_SUCCESS);
}