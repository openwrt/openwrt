# Main makefile for the packages
include $(TOPDIR)/rules.mk

package-y:=openwrt
package-$(BR2_PACKAGE_BRIDGE) += bridge
package-$(BR2_PACKAGE_BUSYBOX) += busybox
package-$(BR2_PACKAGE_DROPBEAR) += dropbear
package-$(BR2_PACKAGE_DNSMASQ) += dnsmasq
package-$(BR2_PACKAGE_EBTABLES) += ebtables
package-$(BR2_PACKAGE_ARPTABLES) += arptables
package-$(BR2_PACKAGE_IPTABLES) += iptables
package-$(BR2_PACKAGE_PPP) += ppp
package-$(BR2_PACKAGE_LZO) += lzo
package-$(BR2_PACKAGE_ZLIB) += zlib
package-$(BR2_PACKAGE_MATRIXSSL) += matrixssl
package-$(BR2_PACKAGE_NCURSES) += ncurses
package-$(BR2_PACKAGE_OPENSSL) += openssl
package-$(BR2_PACKAGE_OPENVPN) += openvpn
package-$(BR2_PACKAGE_PCRE) += pcre
package-$(BR2_PACKAGE_POPT) += popt
package-$(BR2_PACKAGE_PPTP) += pptp
package-$(BR2_PACKAGE_PPTPD) += pptpd
package-$(BR2_PACKAGE_IPROUTE2) += iproute2
package-$(BR2_PACKAGE_HASERL) += haserl
package-$(BR2_PACKAGE_MICROPERL) += microperl
package-$(BR2_PACKAGE_LIBELF) += libelf
package-$(BR2_PACKAGE_STRACE) += strace
package-$(BR2_PACKAGE_TCPDUMP) += tcpdump

all: compile install
clean: $(patsubst %,%-clean,$(package-) $(package-y) $(package-m)) linux-clean
compile: $(patsubst %,%-compile,$(package-y) $(package-m))
install: $(patsubst %,%-install,$(package-y))

tcpdump-compile: libpcap-compile
dropbear-compile: zlib-compile
openssl-compile: zlib-compile
openvpn-compile: openssl-compile
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

