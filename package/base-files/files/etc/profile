#!/bin/sh
[ -f /etc/banner ] && cat /etc/banner

export PATH=/bin:/sbin:/usr/bin:/usr/sbin
export HOME=$(grep -e "^${USER:-root}:" /etc/passwd | cut -d ":" -f 6)
export HOME=${HOME:-/root}
export PS1='\u@\h:\w\$ '

[ -x /bin/more ] || alias more=less
[ -x /usr/bin/vim ] && alias vi=vim || alias vim=vi

[ -z "$KSH_VERSION" -o \! -s /etc/mkshrc ] || . /etc/mkshrc

[ -x /sbin/arp ] || arp() { cat /proc/net/arp; }
[ -z /bin/ldd ] || ldd() { LD_TRACE_LOADED_OBJECTS=1 $*; }
