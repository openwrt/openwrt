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
package-$(BR2_PACKAGE_FUSE) += fuse
package-$(BR2_PACKAGE_GLIB) += glib
package-$(BR2_PACKAGE_GMP) += gmp
package-$(BR2_PACKAGE_HASERL) += haserl
package-$(BR2_PACKAGE_IPTABLES) += iptables
package-$(BR2_PACKAGE_IPROUTE2) += iproute2
package-$(BR2_PACKAGE_KISMET) += kismet
package-$(BR2_PACKAGE_L2TPD) += l2tpd
package-$(BR2_PACKAGE_LCD4LINUX) += lcd4linux
package-$(BR2_PACKAGE_LIBELF) += libelf
package-$(BR2_PACKAGE_LIBGCC) += libgcc
package-$(BR2_PACKAGE_LIBPTHREAD) += libpthread
package-$(BR2_PACKAGE_LIBUSB) += libusb
package-$(BR2_PACKAGE_LZO) += lzo
package-$(BR2_PACKAGE_MATRIXSSL) += matrixssl
package-$(BR2_PACKAGE_MICROPERL) += microperl
package-$(BR2_PACKAGE_MONIT) += monit
package-$(BR2_PACKAGE_NCURSES) += ncurses
package-$(BR2_PACKAGE_NFSD) += nfs-server
package-$(BR2_PACKAGE_NMAP) += nmap
package-$(BR2_PACKAGE_NOCATSPLASH) += nocatsplash
package-$(BR2_PACKAGE_NTPCLIENT) += ntpclient
package-$(BR2_PACKAGE_OLSRD) += olsrd
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
package-$(BR2_PACKAGE_SDK) += sdk
package-$(BR2_PACKAGE_SER) += ser
package-$(BR2_PACKAGE_SETSERIAL) += setserial
package-$(BR2_PACKAGE_SHFS) += shfs
package-$(BR2_PACKAGE_SNORT) += snort
package-$(BR2_PACKAGE_SPEEX) += speex
package-$(BR2_PACKAGE_STRACE) += strace
package-$(BR2_PACKAGE_TCPDUMP) += tcpdump
package-$(BR2_PACKAGE_TINC) += tinc
package-$(BR2_PACKAGE_UCLIBCXX) += uclibc++
package-$(BR2_PACKAGE_USBUTILS) += usbutils
package-$(BR2_PACKAGE_WIRELESS_TOOLS) += wireless-tools
package-$(BR2_PACKAGE_WOL) += wol
package-$(BR2_PACKAGE_ZLIB) += zlib
package-$(BR2_PACKAGE_DHCPFWD) += dhcp-forwarder
package-$(BR2_PACKAGE_LIBNET) += libnet
package-$(BR2_PACKAGE_LIBMYSQLCLIENT) += mysql
package-$(BR2_PACKAGE_LIBPCAP) += libpcap
package-$(BR2_PACKAGE_LIBPQ) += postgresql

DEV_LIBS:=tcp_wrappers glib ncurses openssl pcre popt zlib libnet libpcap mysql postgresql iptables matrixssl lzo gmp fuse portmap libelf uclibc++
DEV_LIBS_COMPILE:=$(patsubst %,%-compile,$(DEV_LIBS))
SDK_DEFAULT_PACKAGES:=busybox dnsmasq iptables wireless-tools dropbear bridge
SDK_DEFAULT_COMPILE:=$(patsubst %,%-compile,$(SDK_DEFAULT_PACKAGES))

all: compile install
clean: $(patsubst %,%-clean,$(package-) $(package-y) $(package-m)) linux-clean
compile: $(patsubst %,%-compile,$(package-y) $(package-m))
install: $(patsubst %,%-install,$(package-y))

ifeq ($(BR2_PACKAGE_OPENVPN_LZO),y)
openvpn-compile: lzo-compile
endif
ifneq ($(BR2_PACKAGE_ASTERISK_SPEEX),)
asterisk-compile: speex-compile
endif
ifneq ($(BR2_PACKAGE_ASTERISK_PGSQL),)
asterisk-compile: postgresql-compile
endif
ifneq ($(BR2_PACKAGE_ASTERISK_MYSQL),)
asterisk-compile: mysql-compile
endif
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
kismet-compile: uclibc++-compile libpcap-compile
tinc-compile: zlib-compile openssl-compile lzo-compile

snort-compile: libnet-compile libpcap-compile pcre-compile
ifeq ($(BR2_PACKAGE_SNORT_MYSQL),y)
snort-compile: mysql-compile
endif
ifeq ($(BR2_PACKAGE_SNORT_PGSQL),y)
snort-compile: postgresql-compile
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

