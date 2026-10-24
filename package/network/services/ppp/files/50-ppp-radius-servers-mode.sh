#!/bin/sh

# The RADIUS secret is read by uid ppp; INSTALL_CONF ships it 0600 root:root.
[ -f /etc/ppp/radius/servers ] || exit 0
chgrp ppp /etc/ppp/radius/servers
chmod 0640 /etc/ppp/radius/servers

exit 0
