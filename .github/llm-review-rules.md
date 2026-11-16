# LLM review rules — openwrt/openwrt

Project-specific patterns to flag, even when other in-tree files still
use the deprecated form. The LLM review routine reads this at session
start.

## Deprecated device-tree patterns

- **LED label syntax.** `label = "green:status";` --> use
  `color = <LED_COLOR_ID_GREEN>;` + `function = LED_FUNCTION_STATUS;`
  (constants from `include/dt-bindings/leds/common.h`; pick the
  `LED_COLOR_ID_<COLOR>` and `LED_FUNCTION_<FUNC>` matching the old
  `<color>:<function>` string, falling back to `function = "<func>";`
  if no matching `LED_FUNCTION_*` constant exists).
- **MAC from MTD.** `mediatek,mtd-eeprom = <&factory 0xNNNN>;` --> use
  `nvmem-cells = <&macaddr_factory_NN>;` +
  `nvmem-cell-names = "mac-address";`.

## Patch regeneration

OpenWrt patches are quilt-managed and **not** refreshed with
`git format-patch`. When a patch's hunk headers, fuzz, or context
need to be regenerated, the project-specific commands are:

- `target/linux/<platform>/patches-<X.Y>/...` --> `make
  target/linux/refresh` (with the matching target selected in
  `.config`)
- `target/linux/generic/backport-<X.Y>/...`,
  `target/linux/generic/pending-<X.Y>/...`, and
  `target/linux/generic/hack-<X.Y>/...` --> same `make
  target/linux/refresh`. Note these live under
  `target/linux/generic/`, not under each `<platform>/`.
- `package/<area>/<pkg>/patches/...` --> `make
  package/<pkg>/refresh` (e.g. `package/kernel/mac80211/patches/...`
  --> `make package/mac80211/refresh`)

If a patch's metadata is wrong, recommend the matching `make ...
refresh` command, not `git format-patch`.

## New device support — required commit-message sections

A PR is adding support for a new device when it adds a new `.dts` /
`.dtsi` file under `target/linux/<plat>/dts/` and a new
`define Device/<vendor>_<model>` block under any `.mk` file in
`target/linux/<plat>/image/` (the block can live in
`<subtarget>.mk`, in a shared `common-<vendor>.mk` /
`generic-<vendor>.mk` include, or in another `.mk` in that
directory). The commit message introducing the device must contain
three sections, even if short:

1. **Hardware specification** — SoC, RAM (size + chip if known),
   Flash (type + size + chip), WiFi (band + chains + chip), Ethernet
   (port count + speed + switch chip), LEDs/Buttons (count + type),
   UART (header location, pinout, baud rate), Power (voltage +
   current).
2. **Flash instructions** — at minimum one fully-described install
   path: which image to use, how to put the device into a
   flashable state (TFTP recovery, U-Boot menu, OEM web UI,
   serial-console method), and a reverting-to-stock note when a
   procedure exists. Verbatim button/IP/hostname strings, not
   hand-wavy descriptions.
3. **MAC address layout** — where each interface's MAC comes from
   (factory partition + offset, board_data text, derived from
   label, etc.), one line per interface (LAN/WAN/2.4G/5G/...).
   Use `xx:xx:xx` for the unit-specific bytes.

Reference well-formed examples (use `git show <sha>` to inspect):

- `986ca4c887f4088b6fbc703faa88884350e9274f` — terse, well-structured.
- `a2dcbd79a4460617bd42151555448f8bca8ca7be` — fuller, with LED layout
  and notes; flash instructions cover U-Boot and OEM-dashboard paths.

Flag missing or empty sections. Don't dictate exact wording — both
references differ significantly in tone and length. The bar is
"could a stranger flash this device from the commit message alone?".
Don't apply this rule to backports, refactors, DTS-only fixes, or
PRs that only add a hardware variant of an existing device (e.g.
`DEVICE_ALT0_*` only) — the original commit already carries the
spec.

## Backports / cherry-picks

PRs targeting `openwrt-NN.NN` branches or titled `[X.Y] ...` are
backports. Their diffs should match the upstream commit on `main`
verbatim. Code-style or design issues that already exist on the
upstream commit belong on a fix-to-main PR, not on the backport —
flag only deviations introduced by the cherry-pick itself, plus
the missing `(cherry picked from commit <sha>)` trailer.
`git cherry-pick -x` adds the trailer automatically.
