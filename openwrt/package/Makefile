# Main makefile for the packages
include $(TOPDIR)/rules.mk

package-y:=openwrt
package-$(BR2_PACKAGE_AICCU) += aiccu
package-$(BR2_PACKAGE_ARPTABLES) += arptables
package-$(BR2_PACKAGE_ARPWATCH) += arpwatch
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
package-$(BR2_PACKAGE_GLIB) += glib
package-$(BR2_PACKAGE_GMP) += gmp
package-$(BR2_PACKAGE_HASERL) += haserl
package-$(BR2_PACKAGE_IPTABLES) += iptables
package-$(BR2_PACKAGE_IPROUTE2) += iproute2
package-$(BR2_PACKAGE_LCD4LINUX) += lcd4linux
package-$(BR2_PACKAGE_LIBELF) += libelf
package-$(BR2_PACKAGE_LZO) += lzo
package-$(BR2_PACKAGE_MATRIXSSL) += matrixssl
package-$(BR2_PACKAGE_MICROPERL) += microperl
package-$(BR2_PACKAGE_NCURSES) += ncurses
package-$(BR2_PACKAGE_NFSD) += nfs-server
package-$(BR2_PACKAGE_NOCATSPLASH) += nocatsplash
package-$(BR2_PACKAGE_NTPCLIENT) += ntpclient
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
package-$(BR2_PACKAGE_SER) += ser
package-$(BR2_PACKAGE_SETSERIAL) += setserial
package-$(BR2_PACKAGE_STRACE) += strace
package-$(BR2_PACKAGE_TCPDUMP) += tcpdump
package-$(BR2_PACKAGE_WIRELESS_TOOLS) += wireless-tools
package-$(BR2_PACKAGE_WOL) += wol
package-$(BR2_PACKAGE_ZLIB) += zlib
package-$(BR2_PACKAGE_DHCPFWD) += dhcp-forwarder
package-$(BR2_PACKAGE_LIBNET) += libnet
package-$(BR2_PACKAGE_LIBMYSQLCLIENT) += mysql
package-$(BR2_PACKAGE_LIBPQ) += postgresql

all: compile install
clean: $(patsubst %,%-clean,$(package-) $(package-y) $(package-m)) linux-clean
compile: $(patsubst %,%-compile,$(package-y) $(package-m))
install: $(patsubst %,%-install,$(package-y))

ifeq ($(BR2_PACKAGE_OPENVPN_LZO),y)
openvpn-compile: lzo-compile
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

$(patsubst %,%-prepare,$(package-y) $(package-m)): linux-install

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

