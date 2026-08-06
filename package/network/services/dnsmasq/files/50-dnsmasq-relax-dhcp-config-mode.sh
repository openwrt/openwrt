#!/bin/sh

# odhcpd uci_load()s /etc/config/dhcp directly and runs unprivileged;
# INSTALL_CONF shipped it 0600, which locked it out.
chmod 0644 /etc/config/dhcp

exit 0
