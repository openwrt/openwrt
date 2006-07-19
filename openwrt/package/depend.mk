amwall-compile: libamsel-compile
arpd-compile: libpcap-compile libdnet-compile libevent-compile
arpwatch-compile: libpcap-compile
atftp-compile: readline-compile ncurses-compile
avahi-compile: libdaemon-compile expat-compile libgdbm-compile
bind-compile: openssl-compile
bitchx-compile: ncurses-compile
bitlbee-compile: libiconv-compile openssl-compile glib-compile
cbtt-compile: mysql-compile zlib-compile
clinkc-compile: expat-compile
curl-compile: openssl-compile zlib-compile
cyrus-sasl-compile: openssl-compile
deco-compile: ncurses-compile
dhcp6-compile: ncurses-compile
dsniff-compile: libnids-compile openssl-compile libgdbm-compile
elinks-compile: openssl-compile
freetype-compile: zlib-compile
fprobe-compile: libpcap-compile
gdbserver-compile: ncurses-compile
gmediaserver-compile: id3lib-compile libupnp-compile
gnutls-compile: libgcrypt-compile opencdk-compile libtasn1-compile
gpsd-compile: uclibc++-compile
icecast-compile: curl-compile libvorbisidec-compile libxml2-compile libxslt-compile
id3lib-compile: uclibc++-compile zlib-compile
iftop-compile: libpcap-compile libpthread-compile ncurses-compile
ipcad-compile: libpcap-compile
irssi-compile: glib-compile ncurses-compile
iperf-compile: uclibc++-compile
iptables-snmp-compile: net-snmp-compile
iptraf-compile: ncurses-compile
ipsec-tools-compile: openssl-compile
jamvm-compile: libffi-sable-compile zlib-compile sablevm-classpath-compile
httping-compile: openssl-compile
kismet-compile: uclibc++-compile libpcap-compile ncurses-compile
l2tpns-compile: libcli-compile
less-compile: ncurses-compile
lcd4linux-compile: ncurses-compile
libgcrypt-compile: libgpg-error-compile
libgd-compile: libpng-compile jpeg-compile
libid3tag-compile: zlib-compile
libnet-compile: libpcap-compile
libnids-compile: libnet-compile
libpng-compile: zlib-compile
libvorbis-compile: libogg-compile
libxml2-compile: zlib-compile
libxslt-compile: libxml2-compile
lighttpd-compile: openssl-compile pcre-compile libxml2-compile sqlite-compile
logrotate-compile: popt-compile
madplay-compile: libid3tag-compile libmad-compile
matrixtunnel-compile: matrixssl-compile
miax-compile: bluez-libs-compile
miredo-compile: uclibc++-compile
monit-compile: openssl-compile
mt-daapd-compile: howl-compile libgdbm-compile libid3tag-compile
mtr-compile: ncurses-compile
mutt-compile: ncurses-compile openssl-compile
mysql-compile: ncurses-compile zlib-compile readline-compile
nano-compile: ncurses-compile
net-snmp-compile: libelf-compile
nfs-server-compile: portmap-compile
nmap-compile: uclibc++-compile pcre-compile libpcap-compile
nocatsplash-compile: glib-compile
opencdk-compile: libgcrypt-compile
openh323-compile: pwlib-compile
openldap-compile: cyrus-sasl-compile openssl-compile
openssh-compile: zlib-compile openssl-compile
openssl-compile: zlib-compile
openswan-compile: gmp-compile
osiris-compile: openssl-compile
palantir-compile: jpeg-compile
peercast-compile: uclibc++-compile
peerguardian-compile: libpthread-compile
portmap-compile: tcp_wrappers-compile
postgresql-compile: zlib-compile
ppp-compile: linux-atm-compile libpcap-compile
privoxy-compile: pcre-compile
ptunnel-compile: libpcap-compile
pwlib-compile: libpthread-compile
quagga-compile: readline-compile ncurses-compile
raddump-compile: openssl-compile libpcap-compile
radiusclient-ng-compile: openssl-compile
rarpd-compile: libnet-compile
ifneq ($(BR2_PACKAGE_LIBRRD),)
rrdcollect-compile: rrdtool-compile
endif
ifneq ($(BR2_PACKAGE_LIBRRD1),)
rrdcollect-compile: rrdtool1-compile
endif
rrdtool-compile: cgilib-compile freetype-compile libart-compile libpng-compile
rrdtool1-compile: zlib-compile
rsync-compile: popt-compile
scanlogd-compile: libpcap-compile libnids-compile libnet-compile
scdp-compile: libnet-compile
screen-compile: ncurses-compile
sipp-compile: ncurses-compile uclibc++-compile libpthread-compile
siproxd-compile: libosip2-compile
sipsak-compile: openssl-compile
socat-compile: openssl-compile
sqlite-compile: ncurses-compile readline-compile
sqlite2-compile: ncurses-compile readline-compile
squid-compile: openssl-compile
ssltunnel-compile: openssl-compile ppp-compile
syslog-ng-compile: libol-compile
tcpdump-compile: libpcap-compile
tinc-compile: zlib-compile openssl-compile liblzo-compile
tor-compile: libevent-compile openssl-compile zlib-compile
usbutils-compile: libusb-compile
vim-compile: ncurses-compile
vncrepeater-compile: uclibc++-compile
vnc-reflector-compile: jpeg-compile zlib-compile
vpnc-compile: libgcrypt-compile libgpg-error-compile
vtun-compile: zlib-compile openssl-compile liblzo-compile
weechat-compile: ncurses-compile gnutls-compile lua-compile
wificonf-compile: wireless-tools-compile nvram-compile
wiviz-compile: libpcap-compile
wknock-compile: libpcap-compile
wpa_supplicant-compile: openssl-compile
wx200d-compile: postgresql-compile
xsupplicant-compile: openssl-compile

asterisk-compile: ncurses-compile openssl-compile
ifneq ($(BR2_PACKAGE_ASTERISK_CHAN_BLUETOOTH),)
asterisk-compile: bluez-libs-compile
endif
ifneq ($(BR2_PACKAGE_ASTERISK_CHAN_H323),)
asterisk-compile: openh323-compile uclibc++-compile
endif
ifneq ($(BR2_PACKAGE_ASTERISK_CODEC_SPEEX),)
asterisk-compile: speex-compile
endif
ifneq ($(BR2_PACKAGE_ASTERISK_PGSQL),)
asterisk-compile: postgresql-compile
endif
ifneq ($(BR2_PACKAGE_ASTERISK_MYSQL),)
asterisk-compile: mysql-compile
endif
ifneq ($(BR2_PACKAGE_ASTERISK_SQLITE),)
asterisk-compile: sqlite2-compile
endif

freeradius-compile: libtool-compile openssl-compile
ifneq ($(BR2_PACKAGE_FREERADIUS_MOD_LDAP),)
freeradius-compile: openldap-compile
endif
ifneq ($(BR2_PACKAGE_FREERADIUS_MOD_SQL_MYSQL),)
freeradius-compile: mysql-compile
endif
ifneq ($(BR2_PACKAGE_FREERADIUS_MOD_SQL_PGSQL),)
freeradius-compile: postgresql-compile
endif

hostapd-compile: wireless-tools-compile
ifneq ($(BR2_PACKAGE_HOSTAPD),)
hostapd-compile: openssl-compile
endif

ifneq ($(BR2_PACKAGE_MINI_HTTPD_MATRIXSSL),)
mini_httpd-compile: matrixssl-compile
endif
ifneq ($(BR2_PACKAGE_MINI_HTTPD_OPENSSL),)
mini_httpd-compile: openssl-compile
endif

ifneq ($(BR2_PACKAGE_MOTION),)
motion-compile: jpeg-compile
endif

ifneq ($(BR2_PACKAGE_MPD_MP3),)
mpd-compile: libid3tag-compile libmad-compile
endif
ifneq ($(BR2_PACKAGE_MPD_OGG),)
mpd-compile: libvorbisidec-compile
endif
ifneq ($(BR2_PACKAGE_MPD_FLAC),)
mpd-compile: flac-compile
endif

ifeq ($(BR2_PACKAGE_LIBOPENSSL),y)
openvpn-compile: openssl-compile
endif
ifeq ($(BR2_PACKAGE_OPENVPN_LZO),y)
openvpn-compile: liblzo-compile
endif

php4-compile: openssl-compile zlib-compile
ifneq ($(BR2_PACKAGE_PHP4_MOD_CURL),)
php4-compile: curl-compile
endif
ifneq ($(BR2_PACKAGE_PHP4_MOD_GD),)
php4-compile: libgd-compile libpng-compile
endif
ifneq ($(BR2_PACKAGE_PHP4_MOD_GMP),)
php4-compile: gmp-compile
endif
ifneq ($(BR2_PACKAGE_PHP4_MOD_LDAP),)
php4-compile: openldap-compile
endif
ifneq ($(BR2_PACKAGE_PHP4_MOD_MYSQL),)
php4-compile: mysql-compile
endif
ifneq ($(BR2_PACKAGE_PHP4_MOD_PCRE),)
php4-compile: pcre-compile
endif
ifneq ($(BR2_PACKAGE_PHP4_MOD_PGSQL),)
php4-compile: postgresql-compile
endif
ifneq ($(BR2_PACKAGE_PHP4_MOD_SQLITE),)
php4-compile: sqlite2-compile
endif
ifneq ($(BR2_PACKAGE_PHP4_MOD_XML),)
php4-compile: expat-compile
endif

php5-compile: openssl-compile zlib-compile
ifneq ($(BR2_PACKAGE_PHP5_MOD_CURL),)
php5-compile: curl-compile
endif
ifneq ($(BR2_PACKAGE_PHP5_MOD_GD),)
php5-compile: libgd-compile libpng-compile
endif
ifneq ($(BR2_PACKAGE_PHP5_MOD_GMP),)
php5-compile: gmp-compile
endif
ifneq ($(BR2_PACKAGE_PHP5_MOD_LDAP),)
php5-compile: openldap-compile
endif
ifneq ($(BR2_PACKAGE_PHP5_MOD_MYSQL),)
php5-compile: mysql-compile
endif
ifneq ($(BR2_PACKAGE_PHP5_MOD_PCRE),)
php5-compile: pcre-compile
endif
ifneq ($(BR2_PACKAGE_PHP5_MOD_PGSQL),)
php5-compile: postgresql-compile
endif
ifneq ($(BR2_PACKAGE_PHP5_MOD_SQLITE),)
php5-compile: sqlite2-compile
endif
ifneq ($(BR2_PACKAGE_PHP5_MOD_XML),)
php5-compile: expat-compile
endif

pmacct-compile: libpcap-compile
ifneq ($(BR2_COMPILE_PMACCT_MYSQL),)
pmacct-compile: mysql-compile
endif
ifneq ($(BR2_COMPILE_PMACCT_PGSQL),)
pmacct-compile: postgresql-compile
endif
ifneq ($(BR2_COMPILE_PMACCT_SQLITE),)
pmacct-compile: sqlite-compile
endif

rrs-compile: uclibc++-compile
ifneq ($(BR2_PACKAGE_RRS),)
rrs-compile: openssl-compile
endif

snort-compile: libnet-compile libpcap-compile pcre-compile
ifeq ($(BR2_PACKAGE_SNORT_WITH_MYSQL),y)
snort-compile: mysql-compile
endif
ifeq ($(BR2_PACKAGE_SNORT_WITH_PGSQL),y)
snort-compile: postgresql-compile
endif
ifeq ($(BR2_PACKAGE_SNORT_ENABLE_INLINE),y)
snort-compile: iptables-compile
endif

snort-wireless-compile: libnet-compile libpcap-compile pcre-compile
ifeq ($(BR2_PACKAGE_SNORT_WIRELESS_WITH_MYSQL),y)
snort-wireless-compile: mysql-compile
endif
ifeq ($(BR2_PACKAGE_SNORT_WIRELESS_WITH_PGSQL),y)
snort-wireless-compile: postgresql-compile
endif
ifeq ($(BR2_PACKAGE_SNORT_WIRELESS_ENABLE_INLINE),y)
snort-wireless-compile: iptables-compile
endif

ulogd-compile: iptables-compile
ifneq ($(BR2_PACKAGE_ULOGD_MOD_MYSQL),)
ulogd-compile: mysql-compile
endif
ifneq ($(BR2_PACKAGE_ULOGD_MOD_PCAP),)
ulogd-compile: libpcap-compile
endif
ifneq ($(BR2_PACKAGE_ULOGD_MOD_PGSQL),)
ulogd-compile: postgresql-compile
endif
ifneq ($(BR2_PACKAGE_ULOGD_MOD_SQLITE),)
ulogd-compile: sqlite-compile
endif
