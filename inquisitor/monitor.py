from scapy.all import *
from scapy.layers.inet import TCP
from scapy.layers.l2 import ARP


def packet_callback(packet_recv):
    if not packet_recv.haslayer(ARP):
        if packet_recv.haslayer(TCP):
            payload = packet_recv[TCP].payload.show()
            if payload is not None:
                print(packet_recv[TCP].payload.show())


if __name__ == "__main__":

    IP_SERVER, MAC_SERVER, IP_CLIENT, MAC_CLIENT = os.getenv('IP_SRC'), os.getenv('MAC_SRC'), os.getenv(
        'IP_TARGET'), os.getenv('MAC_TARGET')
    sniff(prn=packet_callback, store=0)
