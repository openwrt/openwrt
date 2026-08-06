#!/bin/sh

# /etc/config/dhcp shipped 0600 while every generated UCI config lands 0644,
# which locked it away from the unprivileged readers of it (odhcpd, LuCI).
chmod 0644 /etc/config/dhcp

exit 0
