#!/usr/bin/env python3
"""Static release contract for luci-app-fanctrol 3.2.14."""

from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
PACKAGE = "luci-app-fanctrol"


def make_block(text: str, suffix: str) -> str:
    name = re.escape(f"Package/{PACKAGE}/{suffix}")
    match = re.search(rf"(?ms)^define {name}\n(.*?)\nendef$", text)
    if not match:
        raise AssertionError(f"missing Makefile block: {suffix}")
    return match.group(1).replace("$$", "$")


def main() -> None:
    makefile = (ROOT / "Makefile").read_text(encoding="utf-8")
    assert "PKG_VERSION:=3.2.14" in makefile
    assert "PKG_RELEASE:=7" in makefile

    preinst = make_block(makefile, "preinst")
    postinst = make_block(makefile, "postinst")
    prerm = make_block(makefile, "prerm")

    assert "fancontrol-upgrade-backup" in preinst
    assert "FANCONTROL_MAINTAINER_CONFIG" in preinst
    assert "restore_preserved_config" in preinst
    assert "FANCONTROL_TEST_NO_GUARD" in preinst
    assert "FANCONTROL_WATCH_LIMIT" not in preinst
    assert 'rm -f "$config_check" "$config_file-opkg" "$config_backup"' in preinst
    assert not re.search(r"(?m)^[\t ]*ubus\b", preinst)
    assert "/etc/init.d/" not in preinst
    assert "killall" not in preinst
    assert "config_file.maintainer" not in preinst
    staged_match = re.search(
        r"(?ms)<<'FANCONTROL_MAINTAINER_CONFIG'\n(.*?)\nFANCONTROL_MAINTAINER_CONFIG",
        preinst,
    )
    assert staged_match
    staged_default = (staged_match.group(1) + "\n").encode("utf-8")
    assert staged_default == (ROOT / "root/etc/config/fancontrol").read_bytes()

    assert "IPKG_INSTROOT" in postinst
    assert (ROOT / "root/etc/uci-defaults/99-fancontrol-config").is_file()
    assert not (ROOT / "po").exists()
    assert "luci-templatecache" in postinst
    assert '"$config_file-opkg.backup" "$pending_file"' in postinst
    success_cleanup = re.search(
        r'if \[ ! -e "\$config_defaults" \] && \[ -s "\$config_file" \]; then\n(.*?)\nfi',
        postinst,
        re.DOTALL,
    )
    assert success_cleanup and '"$pending_file"' in success_cleanup.group(1)
    assert not re.search(r"(?m)^[\t ]*ubus\b", postinst)
    for operation in (
        'fancontrol" start',
        'fancontrol" stop',
        'fancontrol" restart',
        'fancontrol" running',
    ):
        assert operation not in postinst
    assert not re.search(r"(?m)^[\t ]*(?:exit|return)[\t ]+1(?:[\t ]|$)", postinst)
    assert postinst.rstrip().endswith("exit 0")

    assert not re.search(r"(?m)^[\t ]*ubus\b", prerm)
    assert "/etc/init.d/" not in prerm
    assert prerm.rstrip().endswith("exit 0")

    builder = (ROOT / "tools/build_ipk.py").read_text(encoding="utf-8")
    for token in (
        '"./postinst-pkg"',
        '"./prerm-pkg"',
        "default_postinst",
        "default_prerm",
        "SOURCE_ROOT_NAME = PACKAGE_NAME",
        'f"{PACKAGE_NAME}-{version}-{release}-source.zip"',
        'ROOT / "tools/test_release_3_2_14.py"',
        'ROOT / "tools/test_fan_power_states.py"',
        'ROOT / "tools/test_upgrade_paths.py"',
    ):
        assert token in builder
    assert (ROOT / "tools/test_upgrade_paths.py").is_file()

    controller = (ROOT / "luasrc/controller/fancontrol.lua").read_text(
        encoding="utf-8"
    )
    view = (ROOT / "luasrc/view/fancontrol/main.htm").read_text(
        encoding="utf-8"
    )
    po = (ROOT / "i18n-src/fancontrol.zh-cn.po").read_text(encoding="utf-8")
    daemon = (ROOT / "root/usr/bin/fancontrol").read_text(encoding="utf-8")
    init = (ROOT / "root/etc/init.d/fancontrol").read_text(encoding="utf-8")
    assert controller.count("notify_daemon()") == 4
    assert "manualDirty" in view
    assert "trap 'request_control_cycle' HUP" in daemon
    assert "CONTROL_INTERVAL='1'" in daemon
    assert daemon.count('sleep "$CONTROL_INTERVAL" 9>&- &') == 1
    assert '    sleep "$CONTROL_INTERVAL"\n' not in daemon
    assert daemon.count('wait "$WAIT_PID" 2>/dev/null') == 2
    assert "WAKE_PENDING=1" in daemon
    assert daemon.count("stop_control_wait") >= 4
    assert '[ -r "/proc/$WAIT_PID/status" ] || return 0' in daemon
    assert '[ "$key" = "PPid:" ] || continue' in daemon
    assert "wait_status=$?" in daemon
    assert 'if [ "$wait_status" -ne 0 ]; then' in daemon
    assert daemon.count("sleep 1") == 1
    assert view.count("scheduleStatusPolling(1000);") == 3
    assert "scheduleStatusPolling(100);" not in view
    assert view.count("scheduleStatusPolling(0);") == 1
    assert view.count("apiUrl('get_status')") == 1
    assert "<strong><%:Enable%></strong>" not in view
    assert 'aria-label="<%:Enable Fan Control%>"' in view
    assert 'fancontrol.css?v=20260813r6' in view
    assert '<small><%:Control Temperature%></small>' in view
    assert "var tempSourceKeys = ['cpu', 'phy', 'nvme1', 'nvme2'];" in view
    for generic_msgid in ('CPU', 'Wi-Fi', '5G Modem (Cached)', 'NVMe'):
        assert f'msgid "{generic_msgid}"' not in po
    assert "modem5g" not in view
    assert "cfg.temp_sources || 'cpu'" in view
    assert "fetchJson(apiUrl('set_config'), { method: 'POST', body: body })" in view
    assert "updateTemperatureReadings(data.sources, data.control_source);" in view

    source_selects = re.findall(
        r'(?ms)<select\b[^>]*class="temperature-source-select"[^>]*>(.*?)</select>',
        view,
    )
    assert len(source_selects) == 1
    source_options = re.findall(
        r'<option\s+value="([^"]+)"[^>]*>(.*?)</option>', source_selects[0]
    )
    assert source_options == [
        ("cpu", "<%:Fan Source Processor%>"),
        ("phy", "<%:Fan Source Ethernet PHY%>"),
        ("nvme1", "<%:Fan Source Solid State Drive 1%>"),
        ("nvme2", "<%:Fan Source Solid State Drive 2%>"),
    ]
    assert 'aria-label="<%:Choose Temperature Sources%>" disabled' in view
    assert '<option value="cpu" disabled>' in source_selects[0]
    assert '<option value="phy" hidden disabled>' in source_selects[0]
    assert '<option value="nvme1" hidden disabled>' in source_selects[0]
    assert '<option value="nvme2" hidden disabled>' in source_selects[0]
    assert re.search(r'<option\s+value="phy"', source_selects[0])
    source_select_tag = re.search(
        r'<select\b[^>]*class="temperature-source-select"[^>]*>', view
    )
    assert source_select_tag and not re.search(r"\bmultiple(?:\s|=|>)", source_select_tag.group(0))
    for legacy_token in (
        'class="source-trigger"',
        'class="source-select"',
        "source-chevron",
        "temperature-source-list",
        "temperature-source-row",
        "temperature-source-toggle",
        "setSourceListOpen",
    ):
        assert legacy_token not in view

    source_change = re.search(
        r"(?ms)var sourceSelect = \$\('\.temperature-source-select'\);"
        r"(.*?)\n\s*\$all\('\.mode-card'\)",
        view,
    )
    assert source_change
    source_change_body = source_change.group(1)
    assert "sourceSelect.addEventListener('change'" in source_change_body
    assert "tempSources = [selected];" in source_change_body
    assert "saveTemperatureSources(previousSources);" in source_change_body
    assert "var tempSourceOptions = {};" in view
    assert "var unavailable = state !== 'ok';" in view
    assert "var hideUnavailable = key !== selected && unavailable;" in view
    assert "option.disabled = unavailable;" in view
    assert "option.hidden = true;" in view
    assert "select.removeChild(option);" in view
    assert "option.parentNode !== select" in view
    assert "select.insertBefore(option, before);" in view
    assert "select.appendChild(option);" not in view
    assert "var tempSourceStatusReady = false;" in view
    assert "tempSourceStatusReady = true;" in view
    assert "select.disabled = !pageReady || saveBusy || !tempSourceStatusReady;" in view
    source_save = re.search(
        r"(?ms)^\s*function saveTemperatureSources\(previousSources\) \{"
        r"(.*?)^\s*function saveCurrentCurveToPreset\(\)",
        view,
    )
    assert source_save
    source_save_body = source_save.group(1)
    assert "body.append('temp_sources', tempSources[0] || 'cpu');" in source_save_body
    assert "tempSources.join" not in source_save_body
    assert "tempSources = previousSources.slice();" in source_save_body
    assert "committedTempSources = previousSources.slice();" in source_save_body
    assert "renderTemperatureSources();" in source_save_body
    assert "Failed to save temperature sources." in source_save_body
    assert "data.applied === false" in source_save_body
    assert "immediate apply was not acknowledged" in source_save_body
    css = (ROOT / "htdocs/luci-static/resources/view/fancontrol.css").read_text(
        encoding="utf-8"
    )
    assert ".enable-switch strong" not in css
    assert re.search(
        r"(?ms)^\.control-split \{.*?"
        r"grid-template-columns: repeat\(2, minmax\(0, 1fr\)\);.*?"
        r"gap: 12px;.*?padding: 12px;",
        css,
    )
    assert re.search(
        r"(?ms)^#fancontrol-root\.layout-mobile \.control-split \{.*?"
        r"grid-template-columns: minmax\(0, 1fr\);.*?"
        r"gap: 10px;.*?padding: 10px;",
        css,
    )
    assert '<div class="panel-title stage-title" aria-hidden="true"></div>' in view
    assert '<%:CPU Fan%>' not in view
    assert 'class="control-half source-half"' in view
    assert 'source-summary' not in view
    assert 'source-count' not in view
    select_css = re.search(
        r"(?ms)^#fancontrol-root \.temperature-source-select \{(.*?)^\}", css
    )
    assert select_css
    select_css_body = select_css.group(1)
    for declaration in (
        "display: block;",
        "width: 100%;",
        "max-width: 100%;",
        "min-width: 0;",
        "height: 44px;",
        "min-height: 44px;",
    ):
        assert declaration in select_css_body
    assert "appearance" not in select_css_body
    assert "background-image" not in select_css_body
    assert re.search(
        r"(?ms)^\.fancontrol-page-title,\n"
        r"\.fancontrol-page-description,\n"
        r"#fancontrol-root,\n"
        r"\.fan-toast \{.*?"
        r'font-family: "Segoe UI", "Microsoft YaHei", Arial, sans-serif;.*?'
        r"font-variant: normal;",
        css,
    )
    assert 'class="fancontrol-page-title"' in view
    assert 'class="cbi-map-descr fancontrol-page-description"' in view
    option_css = re.search(
        r"(?ms)^#fancontrol-root \.temperature-source-select option \{(.*?)^\}", css
    )
    assert option_css
    for declaration in (
        "font-family: inherit;",
        "font-size: inherit;",
        "font-weight: inherit;",
        "font-style: normal;",
        "font-variant: inherit;",
    ):
        assert declaration in option_css.group(1)
    assert 'font-variant-numeric: tabular-nums;' in css
    assert 'font-feature-settings: "tnum" 1;' in css
    assert "ctx.font = (rect.width < 420 ? '11px' : '12px') + ' Arial';" not in view
    assert "window.getComputedStyle(root).fontFamily" in view
    assert "ctx.font = '400 '" in view
    for legacy_selector in (
        ".source-trigger",
        ".source-trigger-label",
        ".source-select",
        ".source-chevron",
        ".temperature-source-list",
        ".temperature-source-row",
        ".temperature-source-toggle",
        ".source-copy",
        ".source-state",
        ".source-meta",
        ".source-reading",
    ):
        assert legacy_selector not in css
    assert re.search(
        r"(?ms)^#fancontrol-root\.layout-mobile \.temperature-source-select \{.*?"
        r"width: 100%;.*?max-width: 100%;.*?min-width: 0;.*?height: 44px;",
        css,
    )
    assert re.search(
        r"(?ms)^\.stage-title \{.*?width: 0;.*?height: 0;.*?margin: 0;",
        css,
    )
    assert re.search(r"(?ms)^\.stage-title::after \{.*?display: none;", css)
    assert re.search(
        r"(?ms)^\s*#fancontrol-root:not\(\.layout-ready\) \.switch-half \{.*?"
        r"border: 1px solid var\(--line\);",
        css,
    )
    assert controller.count(
        "if not ok then write_uci_error(uci, err); return end\n"
        "        notify_daemon()"
    ) == 2
    assert "local applied = notify_daemon()" in controller
    assert 'applied = applied' in controller
    reload_match = re.search(
        r"(?ms)^reload_service\(\)\n\{(.*?)^\}", init
    )
    assert reload_match and "restart" not in reload_match.group(1)
    assert "set_fan_percent 100" in daemon
    assert "set_fan_percent 0" in daemon
    assert 'case "$enabled" in' in daemon
    assert re.search(r"(?ms)^\s*0\)\n\s*speed=0\n", daemon)
    assert "1) ;;" in daemon
    assert 'SELECTED_SOURCES_VALID=0' in daemon
    assert 'error="所选温度来源全部不可用，已使用全速保护"' in daemon
    assert '[ "$enabled" = "1" ] || speed=100' not in daemon
    assert 'case "${1-}" in' in daemon
    assert "--check)" in daemon
    assert "controller_pid_matches()" in daemon
    assert "controller_lock_is_live()" in daemon
    assert "acquire_controller_lock()" in daemon
    assert "LOCK_FILE=/var/run/fancontrol.lock" in daemon
    assert 'exec 8<"$LOCK_FILE"' in daemon
    assert daemon.count("[ -x /usr/bin/flock ] || return 1") == 2
    assert "/usr/bin/flock -n 9" in daemon
    assert 'sleep "$CONTROL_INTERVAL" 9>&- &' in daemon
    assert "fancontrol controller is already running" in daemon
    assert 'if [ "${1-}" = "--off" ]; then' in daemon
    assert "turn_fan_off" in daemon
    assert "set_failsafe_full_speed" in daemon
    assert "cleanup()" in daemon and "set_failsafe_full_speed" in daemon
    assert "if turn_fan_off; then" in daemon
    assert "set_failsafe_full_speed\n    exit 1" in daemon
    assert "/usr/bin/fancontrol --off" not in controller
    assert "/usr/bin/fancontrol --stop" not in controller
    assert "/etc/init.d/fancontrol disable" not in controller
    assert controller.count("/etc/init.d/fancontrol enable") == 2
    assert '"$PROG" --off' not in init
    assert '"$PROG" --stop' in init
    assert "service_stopped()" not in init
    assert "config_get_bool enabled" not in init
    assert "procd_open_instance" in init
    assert "procd_open_instance sensors" not in init
    assert "procd_open_instance controller" in init
    assert 'option temp_sources \'cpu\'' in makefile
    defaults = (ROOT / "root/etc/uci-defaults/99-fancontrol-config").read_text(
        encoding="utf-8"
    )
    assert "repair_service_registration=0" in defaults
    assert '"$init_script" enable' in defaults
    assert '"$init_script" running' in defaults
    assert '"$init_script" start' in defaults
    assert defaults.count("finish_success") == 6
    assert 'option temp_sources \'cpu\'' in (ROOT / "root/etc/config/fancontrol").read_text(encoding="utf-8")
    assert 'TEMP_SOURCE_IDS = { "cpu", "phy", "nvme1", "nvme2" }' in controller
    assert "normalize_temp_sources" in controller
    assert 'temp_sources = canonical_temp_source(uci_get(uci, "temp_sources", "cpu"))' in controller
    assert 'runtime_source("phy")' in controller
    assert 'runtime_source("nvme1")' in controller
    assert 'runtime_source("nvme2")' in controller
    assert "select_control_temperature" in daemon
    assert "SOURCE_PHY_STATE" in daemon
    assert "SOURCE_NVME1_STATE" in daemon
    assert "SOURCE_NVME2_STATE" in daemon
    assert 'BOARD_NAME="$(cat /tmp/sysinfo/board_name 2>/dev/null)"' in daemon
    assert "MODEM_CACHE_FILE" not in daemon
    assert "iwpriv" not in daemon
    assert not (ROOT / "root/usr/bin/fancontrol-sensors").exists()
    assert "/etc/init.d/fancontrol enable >/dev/null 2>&1 || :" in postinst
    assert "风扇已关闭并保存。" in view
    assert (ROOT / "root/etc/config/fancontrol").read_bytes() == (
        ROOT / "root/usr/share/fancontrol/fancontrol.default"
    ).read_bytes()

    expected_curve = (
        "20:20,25:20,30:20,35:20,40:20,45:40,50:59,55:79,"
        "60:100,65:100,70:100,75:100,80:100,85:100,90:100,"
        "95:100,100:100,105:100,110:100"
    )
    curve_targets = {
        "root/etc/config/fancontrol": 2,
        "root/usr/share/fancontrol/fancontrol.default": 2,
        "root/usr/bin/fancontrol": 1,
        "luasrc/view/fancontrol/main.htm": 1,
        "tools/render_preview.py": 1,
    }
    assert sum(
        (ROOT / relative).read_text(encoding="utf-8").count(expected_curve)
        for relative in curve_targets
    ) == sum(curve_targets.values())
    for relative, expected in curve_targets.items():
        assert (ROOT / relative).read_text(encoding="utf-8").count(
            expected_curve
        ) == expected

    for path in ROOT.rglob("*"):
        if not path.is_file() or "__pycache__" in path.parts:
            continue
        if path.suffix in {".lmo", ".pyc"}:
            continue
        payload = path.read_bytes()
        assert not payload.startswith(b"\xef\xbb\xbf"), f"UTF-8 BOM: {path}"
        assert b"\r" not in payload, f"non-LF line ending: {path}"

    print("PASS: luci-app-fanctrol 3.2.14 release contract")


if __name__ == "__main__":
    main()
