#!/bin/sh
iptables -I FORWARD 1 -m conntrack --ctstate NEW,RELATED,ESTABLISHED -j FLOWOFFLOAD --hw
