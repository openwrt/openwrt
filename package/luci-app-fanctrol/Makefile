include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-fanctrol
PKG_VERSION:=3.2.0
PKG_RELEASE:=1

LUCI_TITLE:=适配BPI-R3-MINI风扇专用
LUCI_DEPENDS:=+luci-compat +kmod-hwmon-pwmfan
LUCI_PKGARCH:=all

define Package/luci-app-fanctrol/conffiles
/etc/config/fancontrol
endef

define Package/luci-app-fanctrol/preinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] && exit 0

# Older releases tracked these files as conffiles.  Keep placeholders present
# while opkg validates the installed package during an in-place upgrade.
[ -e /etc/fanvall ] || printf '9\n' > /etc/fanvall
[ -e /etc/fanvallv.conf ] || printf 'CPU温度\n' > /etc/fanvallv.conf

[ -x /etc/init.d/Minifanctrl ] && /etc/init.d/Minifanctrl stop >/dev/null 2>&1
[ -x /etc/init.d/Minifanctrl ] && /etc/init.d/Minifanctrl disable >/dev/null 2>&1
[ -x /etc/init.d/fanspeed ] && /etc/init.d/fanspeed stop >/dev/null 2>&1
[ -x /etc/init.d/fanspeed ] && /etc/init.d/fanspeed disable >/dev/null 2>&1
killall fancts.sh bpir3_fanspeed >/dev/null 2>&1 || true
[ -x /etc/init.d/fancontrol ] && /etc/init.d/fancontrol stop >/dev/null 2>&1
exit 0
endef

define Package/luci-app-fanctrol/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] && exit 0

chmod 0755 /etc/init.d/fancontrol /usr/bin/fancontrol

# 3.0.x exposed the electrically reversed cooling states directly.  Migrate
# its presets once so existing conffiles immediately get conventional 0-100%
# semantics after upgrade.
if [ "$$(uci -q get fancontrol.settings.direction_normalized)" != "1" ]; then
	quiet='20:20,25:20,30:20,35:20,40:20,45:20,50:20,55:20,60:20,65:20,70:20,75:20,80:20,85:20,90:20,95:20,100:20,105:20,110:20'
	balanced='20:20,25:20,30:20,35:20,40:20,45:30,50:40,55:50,60:60,65:70,70:80,75:90,80:100,85:100,90:100,95:100,100:100,105:100,110:100'
	performance='20:100,25:100,30:100,35:100,40:100,45:100,50:100,55:100,60:100,65:100,70:100,75:100,80:100,85:100,90:100,95:100,100:100,105:100,110:100'
	mode="$$(uci -q get fancontrol.settings.mode)"
	uci set fancontrol.settings.curve_silent="$$quiet"
	uci set fancontrol.settings.curve_balanced="$$balanced"
	uci set fancontrol.settings.curve_performance="$$performance"
	case "$$mode" in
		0) uci set fancontrol.settings.curve="$$quiet" ;;
		2) uci set fancontrol.settings.curve="$$performance" ;;
		*) uci set fancontrol.settings.curve="$$balanced" ;;
	esac
	uci set fancontrol.settings.direction_normalized='1'
	uci commit fancontrol
fi

rm -f /tmp/luci-indexcache
rm -rf /tmp/luci-modulecache
/etc/init.d/fancontrol enable >/dev/null 2>&1
/etc/init.d/fancontrol restart >/dev/null 2>&1
exit 0
endef

define Package/luci-app-fanctrol/prerm
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] && exit 0
[ -x /etc/init.d/fancontrol ] && /etc/init.d/fancontrol stop >/dev/null 2>&1
[ "$${PKG_UPGRADE}" = "1" ] || /etc/init.d/fancontrol disable >/dev/null 2>&1
exit 0
endef

include $(TOPDIR)/feeds/luci/luci.mk

# call BuildPackage - OpenWrt buildroot signature
