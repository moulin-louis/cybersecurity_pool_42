#include "../inc/inquisitor.h"

void process_payload(t_inquisitor *inquisitor, const uint8_t *packet) {
  string command;

  struct ip *ipHeader = (struct ip *)(packet + 14);
  struct tcphdr *tcpHeader = (struct tcphdr *)(packet + 14 + ipHeader->ip_hl * 4);
  string payload((char *)(packet + 14 + ipHeader->ip_hl * 4 + tcpHeader->th_off * 4));
  const char *srcIP = inet_ntoa(ipHeader->ip_src);
  if (strcmp(srcIP, (char *)(inquisitor->ip_src)) && strcmp(srcIP, (char *)inquisitor->ip_target) )
    return;
  command = payload.substr(0, payload.find(' '));
  if (verbose) {
    if (strncmp(command.c_str(), "USER", 4) == 0 ||
        strncmp(command.c_str(), "PASS", 4) == 0 ||
        strncmp(command.c_str(), "CDUP", 4) == 0 ||
        strncmp(command.c_str(), "LIST", 4) == 0 ||
        strncmp(command.c_str(), "RETR", 4) == 0 ||
        strncmp(command.c_str(), "STOR", 4) == 0 ||
        strncmp(command.c_str(), "DELE", 4) == 0 ||
        strncmp(command.c_str(), "RNFR", 4) == 0 ||
        strncmp(command.c_str(), "RNTO", 4) == 0 ||
        strncmp(command.c_str(), "SIZE", 4) == 0 ||
        strncmp(command.c_str(), "SYST", 4) == 0 ||
        strncmp(command.c_str(), "NOOP", 4) == 0 ||
        strncmp(command.c_str(), "ABOR", 4) == 0 ||
        strncmp(command.c_str(), "QUIT", 4) == 0 ||
        strncmp(command.c_str(), "CWD", 3) == 0 ||
        strncmp(command.c_str(), "PWD", 3) == 0 ||
        strncmp(command.c_str(), "MKD", 3) == 0 ||
        strncmp(command.c_str(), "RMD", 3) == 0) {
      dprintf(1, GREEN "LOG: Command: [%s]\n", payload.c_str());
    }
    return;
  }
  if (strncmp(command.c_str(), "RETR", 4) == 0 || strncmp(command.c_str(), "STOR", 4) == 0) {
    dprintf(1, GREEN "LOG: file name: %s" RESET, payload.substr(5, payload.length() - 4).c_str());
  }
}

void capture_packet(t_inquisitor *inquisitor) {
  char errbuff[PCAP_ERRBUF_SIZE];
  ssize_t retval;
  struct pcap_pkthdr header;
  const uint8_t *packet;
  pcap_t *handle = pcap_open_live("eth0", BUFSIZ, 1, 500, errbuff);

  if (!handle)
    error("pcap_opien_live", errbuff, __FILE__, __LINE__, __func__);
  g_handler = handle;
  struct bpf_program fp;
  string filter = "dst port 21 and inbound";
  retval = pcap_compile(handle, &fp, filter.c_str(), 0, PCAP_NETMASK_UNKNOWN);
  if (retval == -1)
    error("pcap_compile", pcap_geterr(handle), __FILE__, __LINE__, __func__);

  retval = pcap_setfilter(handle, &fp);
  if (retval == -1)
    error("pcap_setfilter", pcap_geterr(handle), __FILE__, __LINE__, __func__);

  retval = pcap_setnonblock(handle, 1, errbuff);
  if (retval == -1)
    error("pcap_setnonblock", errbuff, __FILE__, __LINE__, __func__);
  while (status_loop) {
    packet = pcap_next(handle, &header);
    if (packet == nullptr) {
      usleep(500);
      continue;
    }
    process_payload(inquisitor, packet);
  }
  pcap_close(handle);
}