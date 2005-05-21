# Main makefile for the packages
include $(TOPDIR)/rules.mk

package-:=tcp_wrappers
package-y:=openwrt
package-$(BR2_PACKAGE_AICCU) += aiccu
package-$(BR2_PACKAGE_ARPTABLES) += arptables
package-$(BR2_PACKAGE_ARPWATCH) += arpwatch
package-$(BR2_PACKAGE_ASTERISK) += asterisk
package-$(BR2_PACKAGE_BRIDGE) += bridge
package-$(BR2_PACKAGE_BUSYBOX) += busybox
package-$(BR2_PACKAGE_BWM) += bwm
package-$(BR2_PACKAGE_CHILLISPOT) += chillispot
package-$(BR2_PACKAGE_CIFSMOUNT) += cifsmount
package-$(BR2_PACKAGE_CUPS) += cups
package-$(BR2_PACKAGE_DROPBEAR) += dropbear
package-$(BR2_PACKAGE_DNSMASQ) += dnsmasq
package-$(BR2_PACKAGE_EBTABLES) += ebtables
package-$(BR2_PACKAGE_EZIPUPDATE) += ez-ipupdate
package-$(BR2_PACKAGE_FPING) += fping
package-$(BR2_PACKAGE_FPROBE) += fprobe
package-$(BR2_PACKAGE_FUSE) += fuse
package-$(BR2_PACKAGE_GLIB) += glib
package-$(BR2_PACKAGE_HASERL) += haserl
package-$(BR2_PACKAGE_IPKG) += ipkg
package-$(BR2_PACKAGE_IPTABLES) += iptables
package-$(BR2_PACKAGE_IPROUTE2) += iproute2
package-$(BR2_PACKAGE_KISMET) += kismet
package-$(BR2_PACKAGE_L2TPD) += l2tpd
package-$(BR2_PACKAGE_LCD4LINUX) += lcd4linux
package-$(BR2_PACKAGE_LIBELF) += libelf
package-$(BR2_PACKAGE_GMP) += gmp
package-$(BR2_PACKAGE_LZO) += lzo
package-$(BR2_PACKAGE_LIBPTHREAD) += libpthread
package-$(BR2_PACKAGE_LIBUSB) += libusb
package-$(BR2_PACKAGE_MATRIXSSL) += matrixssl
package-$(BR2_PACKAGE_MARADNS) += maradns
package-$(BR2_PACKAGE_MICROPERL) += microperl
package-$(BR2_PACKAGE_MONIT) += monit
package-$(BR2_PACKAGE_NCURSES) += ncurses
package-$(BR2_PACKAGE_NET_SNMP) += net-snmp
package-$(BR2_PACKAGE_NFS_SERVER) += nfs-server
package-$(BR2_PACKAGE_NMAP) += nmap
package-$(BR2_PACKAGE_NOCATSPLASH) += nocatsplash
package-$(BR2_PACKAGE_NTPCLIENT) += ntpclient
package-$(BR2_PACKAGE_OLSRD) += olsrd
package-$(BR2_PACKAGE_OPENLDAP) += openldap
package-$(BR2_PACKAGE_OPENSSL) += openssl
package-$(BR2_PACKAGE_OPENSWAN) += openswan
package-$(BR2_PACKAGE_OPENNTPD) += openntpd
package-$(BR2_PACKAGE_OPENVPN) += openvpn
package-$(BR2_PACKAGE_PCRE) += pcre
package-$(BR2_PACKAGE_POPT) += popt
package-$(BR2_PACKAGE_PORTMAP) += portmap
package-$(BR2_PACKAGE_PPP) += ppp
package-$(BR2_PACKAGE_PPTP) += pptp
package-$(BR2_PACKAGE_PPTPD) += pptpd
package-$(BR2_PACKAGE_QUAGGA) += quagga
package-$(BR2_PACKAGE_RADVD) += radvd
package-$(BR2_PACKAGE_ROBOCFG) += robocfg
package-$(BR2_PACKAGE_SCREEN) += screen
package-$(BR2_PACKAGE_SDK) += sdk
package-$(BR2_PACKAGE_SER) += ser
package-$(BR2_PACKAGE_SETSERIAL) += setserial
package-$(BR2_PACKAGE_SHAT) += shat
package-$(BR2_PACKAGE_SHFS) += shfs
package-$(BR2_PACKAGE_SNORT) += snort
package-$(BR2_PACKAGE_SPEEX) += speex
package-$(BR2_PACKAGE_STRACE) += strace
package-$(BR2_PACKAGE_TCPDUMP) += tcpdump
package-$(BR2_PACKAGE_TINC) += tinc
package-$(BR2_PACKAGE_TTCP) += ttcp
package-$(BR2_PACKAGE_UCLIBCXX) += uclibc++
package-$(BR2_PACKAGE_USBUTILS) += usbutils
package-$(BR2_PACKAGE_WIRELESS_TOOLS) += wireless-tools
package-$(BR2_PACKAGE_WOL) += wol
package-$(BR2_PACKAGE_ZLIB) += zlib
package-$(BR2_PACKAGE_DHCP_FORWARDER) += dhcp-forwarder
package-$(BR2_PACKAGE_LIBGD) += libgd
package-$(BR2_PACKAGE_LIBNET) += libnet
package-$(BR2_PACKAGE_MYSQL) += mysql
package-$(BR2_PACKAGE_LIBPCAP) += libpcap
package-$(BR2_PACKAGE_LIBPNG) += libpng
package-$(BR2_PACKAGE_POSTGRESQL) += postgresql
package-$(BR2_PACKAGE_CYRUS_SASL) += cyrus-sasl
package-$(BR2_PACKAGE_LIBTOOL) += libtool
package-$(BR2_PACKAGE_LIBOSIP2) += libosip2
package-$(BR2_PACKAGE_SIPROXD) += siproxd
package-$(BR2_PACKAGE_SIPSAK) += sipsak
package-$(BR2_PACKAGE_FREERADIUS) += freeradius
package-$(BR2_PACKAGE_HTPDATE) += htpdate
package-$(BR2_PACKAGE_HOWL) += howl
package-$(BR2_PACKAGE_LIGHTTPD) += lighttpd
package-$(BR2_PACKAGE_OSIRIS) += osiris
package-$(BR2_PACKAGE_PMACCT) += pmacct
package-$(BR2_PACKAGE_ULOGD) += ulogd
package-$(BR2_PACKAGE_VSFTPD) += vsftpd
package-$(BR2_PACKAGE_XINETD) += xinetd

DEV_LIBS:=tcp_wrappers glib ncurses openssl pcre popt zlib libnet libpcap mysql postgresql iptables matrixssl lzo gmp fuse portmap libelf uclibc++ speex libpng libgd
DEV_LIBS_COMPILE:=$(patsubst %,%-compile,$(DEV_LIBS))
SDK_DEFAULT_PACKAGES:=busybox dnsmasq iptables wireless-tools dropbear bridge ipkg ppp
SDK_DEFAULT_COMPILE:=$(patsubst %,%-compile,$(SDK_DEFAULT_PACKAGES))

all: compile install
clean: $(patsubst %,%-clean,$(package-) $(package-y) $(package-m)) linux-clean
compile: $(patsubst %,%-compile,$(package-y) $(package-m))
install: $(patsubst %,%-install,$(package-y))

ifeq ($(BR2_PACKAGE_OPENVPN_LZO),y)
openvpn-compile: lzo-compile
endif
asterisk-compile: openssl-compile
ifneq ($(BR2_PACKAGE_ASTERISK_SPEEX),)
asterisk-compile: speex-compile
endif
ifneq ($(BR2_PACKAGE_ASTERISK_PGSQL),)
asterisk-compile: postgresql-compile
endif
ifneq ($(BR2_PACKAGE_ASTERISK_MYSQL),)
asterisk-compile: mysql-compile
endif
libgd-compile: libpng-compile
screen-compile: ncurses-compile
openswan-compile: gmp-compile
nocatsplash-compile: glib-compile
arpwatch-compile: libpcap-compile
tcpdump-compile: libpcap-compile
dropbear-compile: zlib-compile
openssl-compile: zlib-compile
openvpn-compile: openssl-compile
nfs-server-compile: portmap-compile
portmap-compile: tcp_wrappers-compile
lcd4linux-compile: ncurses-compile
libnet-compile: libpcap-compile
mysql-compile: ncurses-compile zlib-compile
postgresql-compile: zlib-compile
nmap-compile: uclibc++-compile pcre-compile libpcap-compile
kismet-compile: uclibc++-compile libpcap-compile ncurses-compile
tinc-compile: zlib-compile openssl-compile lzo-compile
fprobe-compile: libpcap-compile
cyrus-sasl-compile: openssl-compile
openldap-compile: cyrus-sasl-compile openssl-compile
siproxd-compile: libosip2-compile
sipsak-compile: openssl-compile
lighttpd-compile: openssl-compile pcre-compile
osiris-compile: openssl-compile
freeradius-compile: libtool-compile mysql-compile openldap-compile openssl-compile postgresql-compile

pmacct-compile: libpcap-compile
ifneq ($(BR2_PACKAGE_PMACCT_MYSQL),)
pmacct-compile: mysql-compile
endif
ifneq ($(BR2_PACKAGE_PMACCT_PGSQL),)
pmacct-compile: postgresql-compile
endif

snort-compile: libnet-compile libpcap-compile pcre-compile
ifeq ($(BR2_PACKAGE_SNORT_MYSQL),y)
snort-compile: mysql-compile
endif
ifeq ($(BR2_PACKAGE_SNORT_PGSQL),y)
snort-compile: postgresql-compile
endif
ifeq ($(BR2_PACKAGE_SNORT_ENABLE_INLINE),y)
snort-compile: iptables-compile
endif

ulogd-compile: iptables-compile
ifneq ($(BR2_PACKAGE_ULOGD_MYSQL),)
ulogd-compile: mysql-compile
endif
ifneq ($(BR2_PACKAGE_ULOGD_PGSQL),)
ulogd-compile: postgresql-compile
endif

sdk-compile: $(DEV_LIBS_COMPILE) $(SDK_DEFAULT_COMPILE) openwrt-install
$(patsubst %,%-prepare,$(package-y) $(package-m) $(package-)): linux-install

%-prepare:
	@[ -f $(STAMP_DIR)/.$@ ] || $(MAKE) -C $(patsubst %-prepare,%,$@) prepare
	@touch $(STAMP_DIR)/.$@

%-compile: %-prepare 
	@[ -f $(STAMP_DIR)/.$@ ] || $(MAKE) -C $(patsubst %-compile,%,$@) compile
	@touch $(STAMP_DIR)/.$@

%-install: %-compile
	@[ -f $(STAMP_DIR)/.$@ ] || $(MAKE) -C $(patsubst %-install,%,$@) install
	@touch $(STAMP_DIR)/.$@

%-clean:
	@$(MAKE) -C $(patsubst %-clean,%,$@) clean
	@rm -f $(STAMP_DIR)/.$(patsubst %-clean,%,$@)-*

