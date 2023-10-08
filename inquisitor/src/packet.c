#include "../inc/inquisitor.h"

static void init_dest(struct sockaddr_ll *dest, t_inquisitor *inquisitor) {
  memset(dest, 0, sizeof(*dest));
  dest->sll_family = AF_PACKET;
  dest->sll_ifindex = (int32_t) inquisitor->index;
  dest->sll_hatype = htons(ARPHRD_ETHER);
  dest->sll_halen = ETH_ALEN;
  dest->sll_pkttype = PACKET_OTHERHOST;
}

void transmit_packet(t_inquisitor *inquisitor, ethernet_frame *frame, uint32_t dest, ssize_t len_packet) {
  struct sockaddr_ll dest_addr;
  init_dest(&dest_addr, inquisitor);
  memcpy(frame->src_addr, inquisitor->ifr.ifr_hwaddr.sa_data, 6);
  switch (dest) {
    case 1:
      memcpy(frame->dest_addr, inquisitor->mac_src_byte_arr, 6);
      break;
    case 2:
      memcpy(frame->dest_addr, inquisitor->mac_target_byte_arr, 6);
      break;
    default:
      dprintf(2, YELLOW "WARNING: Error packet_nbr %s" RESET, __func__);
      if (g_sock)
        close(g_sock);
      exit(EXIT_FAILURE);
  }
  dest_addr.sll_protocol = htons(*(uint16_t *)&frame->ethertype);
  ssize_t retval = sendto(inquisitor->sock, frame, len_packet, 0, (struct sockaddr *)&dest_addr, sizeof (dest_addr));
  if (retval == -1)
    error("sendto", NULL, __FILE__, __LINE__, __func__);
  dprintf(1, GREEN "LOG: %ld: %ld bytes transmited to %02x:%02x:%02x:%02x:%02x:%02x\n" RESET, gettime(), retval,
      (uint8_t) frame->dest_addr[0],
      (uint8_t) frame->dest_addr[1],
      (uint8_t) frame->dest_addr[2],
      (uint8_t) frame->dest_addr[3],
      (uint8_t) frame->dest_addr[4],
      (uint8_t) frame->dest_addr[5]);
}

void handle_packet(t_inquisitor *inquisitor, ethernet_frame *frame, ssize_t len_packet) {
  if (ntohs(*(uint16_t *)&frame->ethertype) == ETH_P_ARP) {
    if (memcmp(((t_packet *)(frame->data))->ar_sha, inquisitor->mac_target_byte_arr, 6) == 0)
      send_fake_arp_packet(inquisitor, 1);
    else if (memcmp(((t_packet *)(frame->data))->ar_sha, inquisitor->mac_src_byte_arr, 6) == 0)
      send_fake_arp_packet(inquisitor, 2);
  } else if (ntohs(*(uint16_t *)&frame->ethertype) == ETH_P_IPV6) {
    dprintf(1, YELLOW "WARNING: IPV6 Packet received, cant process it.\n" RESET);
  } else {
    if (memcmp(frame->src_addr, inquisitor->mac_target_byte_arr, 6) == 0)
      transmit_packet(inquisitor, frame, 1, len_packet);
    else if (memcmp(frame->src_addr, inquisitor->mac_src_byte_arr, 6) == 0)
      transmit_packet(inquisitor, frame, 2, len_packet);
    else {
      dprintf(1, YELLOW "WARNING: Unknown Packet Source, type: 0x%02X\n\n" RESET, ntohs(*(uint16_t *)&frame->ethertype));
//      hexdump(frame, len_packet, 4);
    }
  }
}