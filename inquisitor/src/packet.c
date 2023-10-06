#include "../inc/inquisitor.h"

static void init_dest_sock(struct sockaddr_ll *dest, t_inquisitor *inquisitor) {
  memset(dest, 0, sizeof(*dest));
  dest->sll_family = AF_PACKET;
  dest->sll_protocol = htons(ETH_P_IP);
  dest->sll_ifindex = (int32_t) inquisitor->index;
  dest->sll_hatype = htons(ARPHRD_ETHER);
  dest->sll_halen = ETH_ALEN;
  dest->sll_pkttype = PACKET_OTHERHOST;
}

void transmit_packet(t_inquisitor *inquisitor, ethernet_frame *frame, uint32_t dest) {
  struct sockaddr_ll dest_addr;
  memset(&dest_addr, 0, sizeof(dest_addr));
  init_dest_sock(&dest_addr, inquisitor);
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
  dest_addr.sll_protocol = *(uint16_t *)&frame->ethertype;
  ssize_t retval = sendto(inquisitor->sock, frame, sizeof(*frame), 0, (struct sockaddr *)&dest_addr, sizeof (dest_addr));
  if (retval == -1)
    error("sendto", NULL, __FILE__, __LINE__, __func__);
  dprintf(1, GREEN "%ld bytes transmited to ", retval);
  dprintf(1, "%02x:%02x:%02x:%02x:%02x:%02x\n" RESET,
          (uint8_t) frame->dest_addr[0],
          (uint8_t) frame->dest_addr[1],
          (uint8_t) frame->dest_addr[2],
          (uint8_t) frame->dest_addr[3],
          (uint8_t) frame->dest_addr[4],
          (uint8_t) frame->dest_addr[5]);
}

void handle_packet(t_inquisitor *inquisitor, ethernet_frame *frame, uint16_t ethertype) {
  switch (ethertype) {
    case ETH_P_ARP:
      if (memcmp(((t_packet *)(frame->data))->ar_sha, inquisitor->mac_target_byte_arr, 6) == 0)
        send_fake_arp_packet(inquisitor, 1);
      else if (memcmp(((t_packet *)(frame->data))->ar_sha, inquisitor->mac_src_byte_arr, 6) == 0)
        send_fake_arp_packet(inquisitor, 2);
      break;
    default:
      if (memcmp(frame->src_addr, inquisitor->mac_target_byte_arr, 6) == 0)
        transmit_packet(inquisitor, frame, 1);
      else if (memcmp(frame->src_addr, inquisitor->mac_src_byte_arr, 6) == 0)
        transmit_packet(inquisitor, frame, 2);
      else
        dprintf(1, YELLOW "WARNING: Unknown Packet Source\n\n" RESET);
      break;
  }
}