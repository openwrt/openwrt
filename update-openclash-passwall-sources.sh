#!/usr/bin/env bash
set -Eeuo pipefail

export LC_ALL=C

ROOT="${OPENWRT_ROOT:-$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)}"
BACKUP_ROOT="${E87N_UPDATE_BACKUP_ROOT:-$ROOT/.e87n-updater-backups}"
MODE=all
MUTATED=0
STAGE=
BACKUP=

usage() {
	cat <<'EOF'
Usage: ./update-openclash-passwall-sources.sh [--all|--core-only|--recover]

  --all        Update OpenClash, Passwall, Passwall packages and alpha core.
  --core-only  Update only the OpenClash alpha core recipe.
  --recover    Restore the newest updater backup and reinstall pinned feeds.
EOF
}

fail() {
	printf 'ERROR: %s\n' "$*" >&2
	exit 1
}

require_command() {
	command -v "$1" >/dev/null 2>&1 || fail "required command is missing: $1"
}

case "${1:-}" in
	''|--all) MODE=all ;;
	--core-only) MODE=core ;;
	--recover) MODE=recover ;;
	-h|--help) usage; exit 0 ;;
	*) usage >&2; exit 2 ;;
esac
[ "$#" -le 1 ] || { usage >&2; exit 2; }

cd "$ROOT"
[ -f include/toplevel.mk ] || fail "not an OpenWrt source tree: $ROOT"
[ -f .config.e87n ] || fail 'missing .config.e87n'
[ -f package/openclash-core-meta/Makefile ] || fail 'missing OpenClash core package'

for command_name in git curl python3 sha256sum tar file flock; do
	require_command "$command_name"
done

mkdir -p "$BACKUP_ROOT"
exec 9>"$BACKUP_ROOT/.update.lock"
flock -n 9 || fail 'another E87N third-party updater is already running'

TRACKED_FILES=(
	feeds.conf.default
	package/openclash-core-meta/Makefile
	files/usr/share/passwall/app.sh
	files/etc/init.d/passwall
	files/etc/hotplug.d/iface/98-passwall
	e87n-upstream-state.txt
)

restore_backup() {
	local source_backup=$1 relative
	[ -d "$source_backup/files" ] || fail "invalid backup: $source_backup"
	for relative in "${TRACKED_FILES[@]}"; do
		[ -f "$source_backup/files/$relative" ] || \
			fail "backup is missing $relative"
		mkdir -p "$(dirname "$ROOT/$relative")"
		cp -a "$source_backup/files/$relative" "$ROOT/$relative"
	done
}

if [ "$MODE" = recover ]; then
	latest_backup=$(find "$BACKUP_ROOT" -mindepth 1 -maxdepth 1 -type d \
		-name '20????????????-*' -printf '%T@ %p\n' 2>/dev/null | \
		sort -nr | awk 'NR == 1 { sub(/^[^ ]+ /, ""); print }')
	[ -n "$latest_backup" ] || fail "no updater backup exists under $BACKUP_ROOT"
	restore_backup "$latest_backup"
	./scripts/feeds update passwall_packages passwall_luci openclash
	./scripts/feeds install -a
	printf 'E87N_THIRDPARTY_RECOVER=PASS backup=%s\n' "$latest_backup"
	exit 0
fi

state_value() {
	sed -n "s/^$1=//p" "$ROOT/e87n-upstream-state.txt" | head -n1
}

state_matches_current_files() {
	local feed_oc feed_pw feed_pkg core_commit core_version core_sha
	feed_oc=$(sed -n 's|^src-git openclash .*\^\([0-9a-f]\{40\}\)$|\1|p' feeds.conf.default)
	feed_pw=$(sed -n 's|^src-git passwall_luci .*\^\([0-9a-f]\{40\}\)$|\1|p' feeds.conf.default)
	feed_pkg=$(sed -n 's|^src-git passwall_packages .*\^\([0-9a-f]\{40\}\)$|\1|p' feeds.conf.default)
	core_commit=$(sed -n 's/^OPENCLASH_CORE_COMMIT:=//p' package/openclash-core-meta/Makefile)
	core_version=$(sed -n 's/^OPENCLASH_CORE_VERSION:=//p' package/openclash-core-meta/Makefile)
	core_sha=$(sed -n 's/^  HASH:=//p' package/openclash-core-meta/Makefile)
	[ "$feed_oc" = "$(state_value openclash_feed_commit)" ] &&
		[ "$feed_pw" = "$(state_value passwall_luci_feed_commit)" ] &&
		[ "$feed_pkg" = "$(state_value passwall_packages_feed_commit)" ] &&
		[ "$core_commit" = "$(state_value openclash_core_commit)" ] &&
		[ "$core_version" = "$(state_value openclash_core_version)" ] &&
		[ "$core_sha" = "$(state_value openclash_core_sha256)" ] &&
		[ "$(sha256sum files/usr/share/passwall/app.sh | awk '{print $1}')" = \
			"$(state_value passwall_app_sha256)" ] &&
		[ "$(sha256sum files/etc/init.d/passwall | awk '{print $1}')" = \
			"$(state_value passwall_init_sha256)" ] &&
		[ "$(sha256sum files/etc/hotplug.d/iface/98-passwall | awk '{print $1}')" = \
			"$(state_value passwall_hotplug_sha256)" ]
}

dirty_targets=0
for relative in "${TRACKED_FILES[@]}"; do
	git diff --quiet -- "$relative" || dirty_targets=1
	git diff --cached --quiet -- "$relative" || dirty_targets=1
done
if [ "$dirty_targets" -eq 1 ]; then
	state_matches_current_files || \
		fail 'tracked updater targets contain changes not recorded in e87n-upstream-state.txt'
	printf 'Continuing from a previously updater-managed source state.\n'
fi

timestamp=$(date +%Y%m%d%H%M%S)
BACKUP="$BACKUP_ROOT/${timestamp}-$$"
mkdir -p "$BACKUP/files"
for relative in "${TRACKED_FILES[@]}"; do
	mkdir -p "$BACKUP/files/$(dirname "$relative")"
	cp -a "$ROOT/$relative" "$BACKUP/files/$relative"
done

cleanup() {
	local rc=$?
	trap - EXIT
	if [ -n "$STAGE" ] && [ -d "$STAGE" ]; then
		rm -rf -- "$STAGE"
	fi
	if [ "$rc" -ne 0 ] && [ "$MUTATED" -eq 1 ]; then
		printf '\n===== automatic rollback =====\n' >&2
		restore_backup "$BACKUP"
		./scripts/feeds update passwall_packages passwall_luci openclash \
			>/dev/null 2>&1 || true
		./scripts/feeds install -a >/dev/null 2>&1 || true
		printf 'E87N_THIRDPARTY_ROLLBACK=PASS backup=%s\n' "$BACKUP" >&2
	fi
	exit "$rc"
}
trap cleanup EXIT

STAGE=$(mktemp -d "$ROOT/.e87n-thirdparty-stage.XXXXXXXX")

resolve_head() {
	local url=$1 branch=$2 commit
	commit=$(git ls-remote "$url" "refs/heads/$branch" | awk 'NR == 1 { print $1 }')
	[[ "$commit" =~ ^[0-9a-f]{40}$ ]] || \
		fail "cannot resolve $url branch $branch"
	printf '%s\n' "$commit"
}

replace_feed_pin() {
	python3 - "$ROOT/feeds.conf.default" "$1" "$2" "$3" <<'PY'
import pathlib
import re
import sys

path = pathlib.Path(sys.argv[1])
name, url, commit = sys.argv[2:]
text = path.read_text(encoding="utf-8")
pattern = re.compile(rf"^src-git[ \t]+{re.escape(name)}[ \t]+\S+$", re.MULTILINE)
replacement = f"src-git {name} {url}^{commit}"
updated, count = pattern.subn(replacement, text)
if count != 1:
    raise SystemExit(f"feed pin count for {name} is {count}, expected 1")
path.write_text(updated, encoding="utf-8", newline="\n")
PY
}

update_core_recipe() {
	local core_repo core_commit version_url archive_url core_version core_id
	local archive extracted archive_sha makefile
	core_repo='https://github.com/vernesong/OpenClash.git'
	core_commit=$(resolve_head "$core_repo" core)
	version_url="https://raw.githubusercontent.com/vernesong/OpenClash/$core_commit/master/core_version"
	archive_url="https://raw.githubusercontent.com/vernesong/OpenClash/$core_commit/master/meta/clash-linux-arm64.tar.gz"
	core_version=$(curl -fL --retry 3 --connect-timeout 20 "$version_url" | sed -n '1p' | tr -d '\r')
	[[ "$core_version" =~ ^alpha-g[0-9A-Za-z._-]+$ ]] || \
		fail "unexpected OpenClash alpha core version: $core_version"
	core_id=${core_version#alpha-g}

	archive="$STAGE/clash-linux-arm64.tar.gz"
	extracted="$STAGE/core"
	mkdir -p "$extracted"
	curl -fL --retry 3 --connect-timeout 20 -o "$archive" "$archive_url"
	tar -tzf "$archive" | grep -Eq '(^|/)clash$' || \
		fail 'OpenClash alpha archive does not contain clash'
	tar -xzf "$archive" -C "$extracted"
	core_binary=$(find "$extracted" -type f -name clash -print -quit)
	[ -n "$core_binary" ] && [ -x "$core_binary" ] || \
		fail 'OpenClash alpha core binary is missing or not executable'
	file -L "$core_binary" | grep -Eq 'ELF 64-bit LSB executable.*ARM aarch64.*statically linked' || \
		fail 'OpenClash alpha core is not a static AArch64 executable'
	archive_sha=$(sha256sum "$archive" | awk '{print $1}')

	makefile="$ROOT/package/openclash-core-meta/Makefile"
	MUTATED=1
	python3 - "$makefile" "0.0.0_alpha~$core_id" "$core_version" "$core_commit" "$archive_sha" <<'PY'
import pathlib
import re
import sys

path = pathlib.Path(sys.argv[1])
pkg_version, core_version, core_commit, archive_sha = sys.argv[2:]
text = path.read_text(encoding="utf-8")
replacements = (
    (r"^PKG_VERSION:=.*$", f"PKG_VERSION:={pkg_version}"),
    (r"^PKG_RELEASE:=.*$", "PKG_RELEASE:=1"),
    (r"^OPENCLASH_CORE_VERSION:=.*$", f"OPENCLASH_CORE_VERSION:={core_version}"),
    (r"^OPENCLASH_CORE_COMMIT:=.*$", f"OPENCLASH_CORE_COMMIT:={core_commit}"),
    (r"^  HASH:=.*$", f"  HASH:={archive_sha}"),
)
for pattern, replacement in replacements:
    text, count = re.subn(pattern, replacement, text, flags=re.MULTILINE)
    if count != 1:
        raise SystemExit(f"core recipe anchor {pattern!r} count is {count}")
path.write_text(text, encoding="utf-8", newline="\n")
PY

	OPENCLASH_CORE_COMMIT=$core_commit
	OPENCLASH_CORE_VERSION=$core_version
	OPENCLASH_CORE_SHA256=$archive_sha
	printf 'OpenClashAlphaCore=%s commit=%s sha256=%s\n' \
		"$core_version" "$core_commit" "$archive_sha"
}

apply_openclash_menu_priority() {
	local controller=$1
	python3 - "$controller" <<'PY'
import pathlib
import re
import sys

path = pathlib.Path(sys.argv[1])
text = path.read_text(encoding="utf-8")
pattern = re.compile(r'(_\("OpenClash"\),\s*)-?\d+(\))')
updated, count = pattern.subn(r'\g<1>-5\2', text)
if count != 1:
    raise SystemExit(f"OpenClash menu anchor count is {count}, expected 1")
path.write_text(updated, encoding="utf-8", newline="\n")
PY
	grep -Fq '_("OpenClash"), -5)' "$controller" || \
		grep -Fq '_('"'"'OpenClash'"'"'), -5)' "$controller" || \
		fail 'OpenClash menu priority -5 validation failed'
}

apply_passwall_api_compatibility() {
	python3 - "$1" <<'PY'
import pathlib
import re
import sys

path = pathlib.Path(sys.argv[1])
text = path.read_text(encoding="utf-8")
eager = re.findall(r'^\s*cbi\s*=\s*require\s*"luci\.cbi"\s*$', text, re.MULTILINE)
if len(eager) > 1:
    raise SystemExit("ambiguous eager luci.cbi imports")
if eager:
    text, removed = re.subn(r'^cbi = require "luci\.cbi"\n', '', text, count=1, flags=re.MULTILINE)
    if removed != 1:
        raise SystemExit("unknown eager luci.cbi import form")
    for signature in (
        'function set_default_cbi()',
        'function return_map(map)',
        'function luci_types(id, m, s, type_name, option_prefix)',
    ):
        if text.count(signature) != 1:
            raise SystemExit(f"api.lua anchor count is not one: {signature}")
        text = text.replace(signature, signature + '\n\tlocal cbi = require "luci.cbi"', 1)
if re.search(r'^\s*cbi\s*=\s*require\s*"luci\.cbi"\s*$', text, re.MULTILINE):
    raise SystemExit("eager luci.cbi import remains")
path.write_text(text, encoding="utf-8", newline="\n")
PY
}

apply_passwall_socks_compatibility() {
	python3 - "$1" <<'PY'
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
text = path.read_text(encoding="utf-8")
for variable in ('tcp_node', 'udp_node', 'TCP_NODE', 'UDP_NODE'):
    text = text.replace(
        f'if is_socks_wrap "${variable}"; then',
        f'if [ "$(config_get_type "${variable}")" = "socks" ]; then',
    )
    text = text.replace(f'${{{variable}#Socks_}}', f'${{{variable}}}')
if 'is_socks_wrap' in text or '#Socks_' in text:
    raise SystemExit(f"obsolete Socks wrapper remains in {path.name}")
path.write_text(text, encoding="utf-8", newline="\n")
PY
}

apply_passwall_loop_schedule() {
	python3 - "$1" <<'PY'
import os
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
mode = path.stat().st_mode
lines = path.read_text(encoding="utf-8").splitlines(keepends=True)
patterns = (
    ('\t\tlocal svr_t=$(build_time "$week" "$time")', '\t\telse', '\t\t\tlocal svr_t=$(build_time "$week" "$time")'),
    ('\t\tlocal rule_t=$(build_time "$rules_update_week_mode" "$rules_update_time_mode")', '\t\telse', '\t\t\tlocal rule_t=$(build_time "$rules_update_week_mode" "$rules_update_time_mode")'),
    ('\t\t\tlocal sub_t=$(build_time "$sub_update_week_mode" "$sub_update_time_mode")', '\t\t\telse', '\t\t\t\tlocal sub_t=$(build_time "$sub_update_week_mode" "$sub_update_time_mode")'),
)
for unsafe, else_line, safe in patterns:
    plain = [line.rstrip('\r\n') for line in lines]
    unsafe_count = plain.count(unsafe)
    safe_count = plain.count(safe)
    if unsafe_count == 0 and safe_count == 1:
        continue
    if unsafe_count != 1 or safe_count != 0:
        raise SystemExit(f"ambiguous Passwall schedule shape: {unsafe_count=}, {safe_count=}, {unsafe!r}")
    index = plain.index(unsafe)
    del lines[index]
    plain = [line.rstrip('\r\n') for line in lines]
    candidates = [i for i in range(index, min(index + 12, len(lines))) if plain[i] == else_line]
    if len(candidates) != 1:
        raise SystemExit(f"Passwall schedule else anchor count is {len(candidates)} for {unsafe!r}")
    insert_at = candidates[0] + 1
    lines.insert(insert_at, safe + '\n')
result = ''.join(lines)
for _unsafe, _else_line, safe in patterns:
    if result.splitlines().count(safe) != 1:
        raise SystemExit(f"safe Passwall schedule anchor missing: {safe!r}")
path.write_text(result, encoding="utf-8", newline="\n")
os.chmod(path, mode)
PY
}

apply_offline_rootfs_guard() {
	local init_file=$1 service_name=$2 anchor=$3
	python3 - "$init_file" "$service_name" "$anchor" <<'PY'
import os
import pathlib
import sys

path = pathlib.Path(sys.argv[1])
service, anchor = sys.argv[2:]
mode = path.stat().st_mode
text = path.read_text(encoding="utf-8")
marker = '# CODEX_OFFLINE_ROOTFS_GUARD'
if marker not in text:
    if text.splitlines().count(anchor) != 1:
        raise SystemExit(f"{service} init anchor count is not one: {anchor}")
    block = (
        anchor + '\n\n'
        '# CODEX_OFFLINE_ROOTFS_GUARD: offline assembly only needs init metadata.\n'
        'if [ -n "${IPKG_INSTROOT:-}" ]; then\n'
        '\treturn 0 2>/dev/null || exit 0\n'
        'fi'
    )
    text = text.replace(anchor, block, 1)
safe_early_return = 'if [ -n "${IPKG_INSTROOT:-}" ]; then' in text
safe_wrapped_imports = 'if [ -z "${IPKG_INSTROOT:-}" ]; then' in text
if text.count(marker) != 1 or not (safe_early_return or safe_wrapped_imports):
    raise SystemExit(f"{service} offline-rootfs guard validation failed")
path.write_text(text, encoding="utf-8", newline="\n")
os.chmod(path, mode)
PY
}

update_feed_sources() {
	local openclash_repo passwall_repo packages_repo
	local openclash_commit passwall_commit packages_commit
	local oc_controller oc_init pw_root pw_api pw_nft pw_ipt pw_app pw_init
	openclash_repo='https://github.com/vernesong/OpenClash.git'
	passwall_repo='https://github.com/Openwrt-Passwall/openwrt-passwall.git'
	packages_repo='https://github.com/Openwrt-Passwall/openwrt-passwall-packages.git'
	openclash_commit=$(resolve_head "$openclash_repo" master)
	passwall_commit=$(resolve_head "$passwall_repo" main)
	packages_commit=$(resolve_head "$packages_repo" main)

	MUTATED=1
	replace_feed_pin openclash "$openclash_repo" "$openclash_commit"
	replace_feed_pin passwall_luci "$passwall_repo" "$passwall_commit"
	replace_feed_pin passwall_packages "$packages_repo" "$packages_commit"

	./scripts/feeds update passwall_packages passwall_luci openclash
	./scripts/feeds install -a

	oc_controller="$ROOT/feeds/openclash/luci-app-openclash/luasrc/controller/openclash.lua"
	oc_init="$ROOT/feeds/openclash/luci-app-openclash/root/etc/init.d/openclash"
	pw_root="$ROOT/feeds/passwall_luci/luci-app-passwall"
	pw_api="$pw_root/luasrc/passwall/api.lua"
	pw_nft="$pw_root/root/usr/share/passwall/nftables.sh"
	pw_ipt="$pw_root/root/usr/share/passwall/iptables.sh"
	pw_app="$pw_root/root/usr/share/passwall/app.sh"
	pw_init="$pw_root/root/etc/init.d/passwall"
	for required_file in "$oc_controller" "$oc_init" "$pw_api" "$pw_nft" \
		"$pw_ipt" "$pw_app" "$pw_init"; do
		[ -f "$required_file" ] || fail "updated feed file is missing: $required_file"
	done

	apply_openclash_menu_priority "$oc_controller"
	apply_offline_rootfs_guard "$oc_init" openclash 'USE_PROCD=1'
	apply_passwall_api_compatibility "$pw_api"
	apply_passwall_socks_compatibility "$pw_nft"
	apply_passwall_socks_compatibility "$pw_ipt"
	apply_passwall_loop_schedule "$pw_app"
	apply_offline_rootfs_guard "$pw_init" passwall 'STOP=15'

	install -m 0755 "$pw_app" "$ROOT/files/usr/share/passwall/app.sh"
	install -m 0755 "$pw_init" "$ROOT/files/etc/init.d/passwall"

	sh -n "$ROOT/files/usr/share/passwall/app.sh"
	sh -n "$ROOT/files/etc/init.d/passwall"
	sh -n "$ROOT/files/etc/hotplug.d/iface/98-passwall"
	grep -Fq 'CODEX_PASSWALL_IFUP_DEBOUNCE_V2' \
		"$ROOT/files/etc/hotplug.d/iface/98-passwall" || \
		fail 'E87N Passwall V2 ifup debounce overlay is missing'
	grep -Fq '_("OpenClash"), -5)' "$oc_controller" || \
		fail 'updated OpenClash controller is not priority -5'
	grep -Fq 'CODEX_OFFLINE_ROOTFS_GUARD' "$oc_init" || \
		fail 'updated OpenClash init lacks offline-rootfs guard'

	for package_name in hysteria sing-box xray-core shadowsocks-rust; do
		[ -f "$ROOT/feeds/passwall_packages/$package_name/Makefile" ] || \
			fail "Passwall core recipe is missing: $package_name"
	done

	OPENCLASH_FEED_COMMIT=$openclash_commit
	PASSWALL_LUCI_FEED_COMMIT=$passwall_commit
	PASSWALL_PACKAGES_FEED_COMMIT=$packages_commit
	printf 'OpenClashFeedCommit=%s\nPasswallFeedCommit=%s\nPasswallPackagesCommit=%s\n' \
		"$openclash_commit" "$passwall_commit" "$packages_commit"
}

OPENCLASH_FEED_COMMIT=$(sed -n 's|^src-git openclash .*\^\([0-9a-f]\{40\}\)$|\1|p' feeds.conf.default)
PASSWALL_LUCI_FEED_COMMIT=$(sed -n 's|^src-git passwall_luci .*\^\([0-9a-f]\{40\}\)$|\1|p' feeds.conf.default)
PASSWALL_PACKAGES_FEED_COMMIT=$(sed -n 's|^src-git passwall_packages .*\^\([0-9a-f]\{40\}\)$|\1|p' feeds.conf.default)
OPENCLASH_CORE_COMMIT=$(sed -n 's/^OPENCLASH_CORE_COMMIT:=//p' package/openclash-core-meta/Makefile)
OPENCLASH_CORE_VERSION=$(sed -n 's/^OPENCLASH_CORE_VERSION:=//p' package/openclash-core-meta/Makefile)
OPENCLASH_CORE_SHA256=$(sed -n 's/^  HASH:=//p' package/openclash-core-meta/Makefile)

if [ "$MODE" = all ]; then
	printf '===== update official feeds =====\n'
	update_feed_sources
fi

printf '===== update OpenClash alpha core =====\n'
update_core_recipe

openclash_version=$(sed -n 's/^PKG_VERSION:=//p' feeds/openclash/luci-app-openclash/Makefile 2>/dev/null | head -n1)
passwall_version=$(sed -n 's/^PKG_VERSION:=//p' feeds/passwall_luci/luci-app-passwall/Makefile 2>/dev/null | head -n1)
hysteria_version=$(sed -n 's/^PKG_VERSION:=//p' feeds/passwall_packages/hysteria/Makefile 2>/dev/null | head -n1)
[ -n "$openclash_version" ] || openclash_version=$(state_value openclash_version)
[ -n "$passwall_version" ] || passwall_version=$(state_value passwall_version)
[ -n "$hysteria_version" ] || hysteria_version=$(state_value hysteria_version)
passwall_app_sha=$(sha256sum files/usr/share/passwall/app.sh | awk '{print $1}')
passwall_init_sha=$(sha256sum files/etc/init.d/passwall | awk '{print $1}')
passwall_hotplug_sha=$(sha256sum files/etc/hotplug.d/iface/98-passwall | awk '{print $1}')

cat > "$ROOT/e87n-upstream-state.txt" <<EOF
updated_at=$(date -u +%Y-%m-%dT%H:%M:%SZ)
openclash_feed_commit=$OPENCLASH_FEED_COMMIT
passwall_luci_feed_commit=$PASSWALL_LUCI_FEED_COMMIT
passwall_packages_feed_commit=$PASSWALL_PACKAGES_FEED_COMMIT
openclash_version=$openclash_version
passwall_version=$passwall_version
hysteria_version=$hysteria_version
openclash_core_commit=$OPENCLASH_CORE_COMMIT
openclash_core_version=$OPENCLASH_CORE_VERSION
openclash_core_sha256=$OPENCLASH_CORE_SHA256
passwall_app_sha256=$passwall_app_sha
passwall_init_sha256=$passwall_init_sha
passwall_hotplug_sha256=$passwall_hotplug_sha
EOF

grep -Eq '^openclash_feed_commit=[0-9a-f]{40}$' e87n-upstream-state.txt
grep -Eq '^passwall_luci_feed_commit=[0-9a-f]{40}$' e87n-upstream-state.txt
grep -Eq '^passwall_packages_feed_commit=[0-9a-f]{40}$' e87n-upstream-state.txt
grep -Eq '^openclash_core_commit=[0-9a-f]{40}$' e87n-upstream-state.txt
grep -Eq '^openclash_core_version=alpha-g' e87n-upstream-state.txt
grep -Eq '^openclash_core_sha256=[0-9a-f]{64}$' e87n-upstream-state.txt
grep -Eq '^passwall_app_sha256=[0-9a-f]{64}$' e87n-upstream-state.txt
grep -Eq '^passwall_init_sha256=[0-9a-f]{64}$' e87n-upstream-state.txt
grep -Eq '^passwall_hotplug_sha256=[0-9a-f]{64}$' e87n-upstream-state.txt

MUTATED=0
printf 'E87N_THIRDPARTY_SOURCE_UPDATE=PASS mode=%s backup=%s\n' "$MODE" "$BACKUP"
printf 'Review and commit the tracked changes after a successful local build.\n'
