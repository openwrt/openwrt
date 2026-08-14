include $(TOPDIR)/rules.mk

PKG_NAME:=luci-app-fanctrol
PKG_VERSION:=3.2.14
PKG_RELEASE:=8

LUCI_TITLE:=Fan Control for EdgePi E87N
LUCI_DEPENDS:=+luci-compat +kmod-hwmon-pwmfan
LUCI_PKGARCH:=all
PKG_PROVIDES:=luci-i18n-fanctrol-zh-cn

define Package/luci-app-fanctrol/conffiles
/etc/config/fancontrol
endef

# Preserve the exact active UCI file before opkg performs conffile handling.
# This hook deliberately does not stop services or call ubus. Service lifecycle
# is owned by OpenWrt's default_prerm/default_postinst wrappers.
define Package/luci-app-fanctrol/preinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] && exit 0

config_file="$${FANCONTROL_CONFIG_FILE:-/etc/config/fancontrol}"
config_save="$${FANCONTROL_CONFIG_SAVE:-/etc/.fancontrol-upgrade-backup}"
default_file="$${FANCONTROL_DEFAULT_FILE:-/usr/share/fancontrol/fancontrol.default}"
config_backup="$$config_file-opkg.backup"
pending_file="$${FANCONTROL_PENDING_FILE:-/tmp/fancontrol-package.pending}"
preserve_source=''

# Do not overwrite a valid recovery copy with the temporary maintainer default
# left by an interrupted older upgrade.
[ ! -e "$$config_save" ] || [ -s "$$config_save" ] || rm -f "$$config_save"
if [ -s "$$config_file" ]; then
	if [ ! -s "$$config_save" ] || [ ! -r "$$default_file" ] || \
	   ! cmp -s "$$config_file" "$$default_file"; then
		preserve_source="$$config_file"
	fi
elif [ ! -s "$$config_save" ] && [ -s "$$config_backup" ]; then
	preserve_source="$$config_backup"
fi

if [ -n "$$preserve_source" ]; then
	umask 077
	rm -f "$$config_save.new"
	cp "$$preserve_source" "$$config_save.new" 2>/dev/null && \
		chmod 0600 "$$config_save.new" 2>/dev/null && \
		cmp -s "$$preserve_source" "$$config_save.new" && \
		mv -f "$$config_save.new" "$$config_save" || {
		rm -f "$$config_save.new"
		echo "fancontrol: failed to preserve existing configuration" >&2
		exit 1
	}
fi

restore_preserved_config()
{
	[ -s "$$config_save" ] || return 0
	restore_file="$$config_file.restore.$$$$"
	rm -f "$$restore_file"
	cp "$$config_save" "$$restore_file" 2>/dev/null && \
		chmod 0644 "$$restore_file" 2>/dev/null && \
		mv -f "$$restore_file" "$$config_file" 2>/dev/null && \
		cmp -s "$$config_save" "$$config_file"
}

# Once the maintainer default is staged, a detached guard restores the
# saved bytes if opkg exits before postinst completes the transaction.
guard_token="preinst-$$$$"
installer_pid="$$PPID"
if [ "$${FANCONTROL_TEST_NO_GUARD:-0}" != '1' ]; then
	printf '%s\n' "$$guard_token" > "$$pending_file" 2>/dev/null || {
		echo "fancontrol: cannot start configuration upgrade guard" >&2
		exit 1
	}
	(
		while [ "$$(cat "$$pending_file" 2>/dev/null)" = "$$guard_token" ]; do
			if ! kill -0 "$$installer_pid" 2>/dev/null; then
				restore_preserved_config || :
				rm -f "$$pending_file"
				break
			fi
			sleep 1
		done
	) </dev/null >/dev/null 2>&1 &
fi

# Present the exact new maintainer default only for opkg's conffile comparison.
# The user's active bytes are already safe in config_save and uci-defaults
# restores them before default_postinst starts the replacement service.
case "$$config_file" in
	*/*) config_dir="$${config_file%/*}" ;;
	*) config_dir='.' ;;
esac
mkdir -p "$$config_dir" || exit 1
config_stage="$$config_file.preinst.$$$$"
config_check="$$config_stage.check"
rm -f "$$config_stage" "$$config_check"
cat > "$$config_stage" <<'FANCONTROL_MAINTAINER_CONFIG'
config fancontrol 'settings'
    option enable '1'
    option thermal_file '/sys/class/thermal/thermal_zone0/temp'
    option fan_file '/var/run/fancontrol.speed'
    option fan_rpm_file '/var/run/fancontrol.rpm'
    option max_speed '100'
    option temp_div '1000'
    option mode '1'
    option temp_sources 'cpu'
    option curve '20:20,25:20,30:20,35:20,40:20,45:40,50:59,55:79,60:100,65:100,70:100,75:100,80:100,85:100,90:100,95:100,100:100,105:100,110:100'
    option curve_silent '20:20,25:20,30:20,35:20,40:20,45:20,50:20,55:20,60:20,65:20,70:20,75:20,80:20,85:20,90:20,95:20,100:20,105:20,110:20'
    option curve_balanced '20:20,25:20,30:20,35:20,40:20,45:40,50:59,55:79,60:100,65:100,70:100,75:100,80:100,85:100,90:100,95:100,100:100,105:100,110:100'
    option curve_performance '20:100,25:100,30:100,35:100,40:100,45:100,50:100,55:100,60:100,65:100,70:100,75:100,80:100,85:100,90:100,95:100,100:100,105:100,110:100'
    option schema_version '2'
FANCONTROL_MAINTAINER_CONFIG
chmod 0644 "$$config_stage" 2>/dev/null && \
	cp "$$config_stage" "$$config_check" 2>/dev/null && \
	mv -f "$$config_stage" "$$config_file" 2>/dev/null && \
	cmp -s "$$config_check" "$$config_file" 2>/dev/null || {
	rm -f "$$config_stage" "$$config_check" "$$pending_file"
	restore_preserved_config || :
	echo "fancontrol: failed to stage maintainer configuration" >&2
	exit 1
}
rm -f "$$config_check" "$$config_file-opkg" "$$config_backup"

exit 0
endef

# OpenWrt executes this as postinst-pkg from default_postinst(). Runtime service
# start/stop and procd registration must not be duplicated here. In particular,
# a temporarily unavailable ubus socket must never make opkg configuration fail.
define Package/luci-app-fanctrol/postinst
#!/bin/sh
[ -n "$${IPKG_INSTROOT}" ] && exit 0

config_file="$${FANCONTROL_CONFIG_FILE:-/etc/config/fancontrol}"
config_save="$${FANCONTROL_CONFIG_SAVE:-/etc/.fancontrol-upgrade-backup}"
config_defaults="$${FANCONTROL_DEFAULTS_SCRIPT:-/etc/uci-defaults/99-fancontrol-config}"
pending_file="$${FANCONTROL_PENDING_FILE:-/tmp/fancontrol-package.pending}"

# File modes are already part of package data. Repair them best-effort for
# legacy installs, but never turn an otherwise unpacked package into an opkg
# configure failure because runtime services or ubus are temporarily offline.
chmod 0755 /etc/init.d/fancontrol /usr/bin/fancontrol 2>/dev/null || :

# Releases before 3.2.13-5 disabled the rc.d link when the WebUI switch was
# turned off. Keep it enabled so start_service can reapply the persisted 0%
# output after every cold boot. Actual start/stop remains default_postinst's job.
/etc/init.d/fancontrol enable >/dev/null 2>&1 || :

# Cache cleanup is safe and contains no ubus/procd operation.
rm -f /tmp/luci-indexcache
rm -rf /tmp/luci-modulecache /tmp/luci-templatecache /tmp/luci-runtimecache

# default_postinst removes the packaged uci-defaults helper only after it exits
# successfully. That is the transaction commit marker: the active conffile is
# now either the preserved user configuration or a valid first-install default.
if [ ! -e "$$config_defaults" ] && [ -s "$$config_file" ]; then
	rm -f "$$config_save" "$$config_file-opkg" \
		"$$config_file-opkg.backup" "$$pending_file"
fi
rm -f "$$config_save.new" "$$config_file.restore" "$$config_file.guard"

# Do not call init start/stop/restart/running and do not call ubus here. The standard
# default_postinst wrapper starts the packaged init script exactly once.
exit 0
endef

# default_prerm stops the packaged init script exactly once after this hook.
# Keeping this hook free of service calls prevents duplicate service-delete
# requests and the resulting harmless-but-noisy "Not found" message.
define Package/luci-app-fanctrol/prerm
#!/bin/sh
exit 0
endef

include $(TOPDIR)/feeds/luci/luci.mk

# call BuildPackage - OpenWrt buildroot signature
