#!/bin/bash
# route del default gw 192.168.10.1 dev eth1
echo 1 > /proc/sys/net/ipv4/ip_forward
iptables -t nat -A POSTROUTING -s 192.168.10.0/24 -o eth0 -j MASQUERADE
echo "nameserver 8.8.8.8" > /etc/resolv.conf
echo "nameserver 202.205.16.4" >> /etc/resolv.conf
service dnsmasq start
