#!/bin/sh
[ -f /etc/banner ] && cat /etc/banner
[ -e /tmp/.failsafe ] && cat /etc/banner.failsafe

export PATH=/usr/sbin:/usr/bin:/sbin:/bin
export HOME=$(grep -e "^${USER:-root}:" /etc/passwd | cut -d ":" -f 6)
export HOME=${HOME:-/root}
export PS1='\u@\h:\w\$ '

[ -x /bin/more ] || alias more=less
[ -x /usr/bin/vim ] && alias vi=vim || alias vim=vi

[ -z "$KSH_VERSION" -o \! -s /etc/mkshrc ] || . /etc/mkshrc

[ -x /usr/bin/arp ] || arp() { cat /proc/net/arp; }
[ -x /usr/bin/ldd ] || ldd() { LD_TRACE_LOADED_OBJECTS=1 $*; }

[ -n "$FAILSAFE" ] || {
	for FILE in /etc/profile.d/*.sh; do
		[ -e "$FILE" ] && . "$FILE"
	done
	unset FILE
}
