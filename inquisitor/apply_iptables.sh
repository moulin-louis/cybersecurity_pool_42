#!/bin/bash
sleep 15


echo "Test 1"
iptables -A INPUT -s $IP_SRC -d $IP_TARGET -j DROP
echo "Test 2"
iptables -A INPUT -s $IP_TARGET -d $IP_SRC -j DROP
echo "Test 3"
iptables -A INPUT -m mac --mac-source $MAC_SRC -j DROP
echo "Test 4"
iptables -A INPUT -m mac --mac-source $MAC_TARGET -j DROP
echo "Test 5"
iptables -A FORWARD -s $IP_SRC -d $IP_TARGET -j DROP
echo "Test 6"
iptables -A FORWARD -s $IP_TARGET -d $IP_SRC -j DROP
echo "Test 7"
iptables -A FORWARD -m mac --mac-source $MAC_SRC -j DROP
echo "Test 8"
iptables -A FORWARD -m mac --mac-source $MAC_TARGET -j DROP
echo "Test 9"
iptables -A OUTPUT -s $IP_SRC -d $IP_TARGET -j DROP
echo "Test 10"
iptables -A OUTPUT -s $IP_TARGET -d $IP_SRC -j DROP
echo "IPTABLES SET" > /result


tail -f