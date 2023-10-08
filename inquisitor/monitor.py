import os
import logging
from scapy.all import *
from scapy.layers.inet import IP, TCP, UDP
from scapy.layers.l2 import Ether

logging.basicConfig(level=logging.DEBUG)


MY_IP = '172.16.238.30'
MY_MAC = '02:42:ac:11:00:04'


def packet_callback(packet_recv):
    try:
        if packet_recv.haslayer(IP):
            ip_src = packet_recv[IP].src
            ip_dst = packet_recv[IP].dst

            logging.info(f"Original packet: {packet_recv.summary()}")

            if ip_src == IP_CLIENT and ip_dst == IP_SERVER:
                print('Packet from IP_TARGET to IP_SRC')
                packet_recv[IP].src = IP_SERVER
                packet_recv[IP].dst = IP_CLIENT
                packet_recv[Ether].src = MAC_SERVER
                packet_recv[Ether].dst = MAC_CLIENT
            elif ip_src == IP_SERVER and ip_dst == IP_CLIENT:
                print('Packet from IP_SRC to IP_TARGET')
                packet_recv[IP].src = IP_CLIENT
                packet_recv[IP].dst = IP_SERVER
                packet_recv[Ether].src = MAC_CLIENT
                packet_recv[Ether].dst = MAC_SERVER

            del packet_recv[IP].chksum
            if packet_recv.haslayer(TCP):
                del packet_recv[TCP].chksum
            elif packet_recv.haslayer(UDP):
                del packet_recv[UDP].chksum

            logging.info(f"Modified packet: {packet_recv.summary()}")
            sendp(packet_recv)
    except Exception as err:
        logging.error(f"Error processing packet: {err}")


if __name__ == "__main__":

    IP_SERVER, MAC_SERVER, IP_CLIENT, MAC_CLIENT = os.getenv('IP_SRC'), os.getenv('MAC_SRC'), os.getenv('IP_TARGET'), os.getenv('MAC_TARGET')
    try:
        sniff(prn=packet_callback, store=0)
    except Exception as e:
        logging.error(f"Error sniffing packets: {e}")
