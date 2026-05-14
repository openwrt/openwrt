# Security Changes (Sentinel Series)

This document catalogs all security hardening applied in the Zektopic fork that are not present in upstream OpenWrt.

## 1. WireGuard Command Injection Fix

**Files:** `package/network/utils/wireguard-tools/files/wireguard.uc`

**Vulnerability:** The wireguard shell/ucode script passed user-controlled interface names and configuration values directly into shell commands without sanitization, enabling command injection via crafted interface names.

**Fix:** Replaced string-based command construction with array-based exec() calls. Added a regex-based interface name allowlist (`^[a-zA-Z0-9_\-\.@]+$`) to reject invalid interface names before any operations.

## 2. ead (Encrypted Archive Daemon) Buffer Overflows

**Files:** `target/linux/generic/pending-6.12/*.patch` (ead-related patches)
**Files:** `target/linux/generic/hack-6.12/*.patch` (ead-related patches)

**Vulnerability:** Multiple buffer overflow vulnerabilities in the ead userspace helper and kernel module.

**Fix:** Replaced unbounded string operations (`strcpy`) with bounds-checked variants (`strscpy`, `strncpy`). Added length validation before buffer operations.

**CVE Classification (analogous):** CWE-120 (Buffer Copy without Checking Size of Input)

## 3. provision.uc Command Injection

**Files:** `package/system/provision/files/provision.uc`

**Vulnerability:** User-controlled input was passed to shell execution without sanitization.

**Fix:** Applied input validation and switched to safe execution patterns with array-based argument passing.

## 4. button-hotplug strcpy Fixes

**Files:** `target/linux/generic/pending-6.12/*.patch` (button-hotplug related)

**Vulnerability:** The kernel button-hotplug module used `strcpy()` for copying input strings into fixed-size buffers, risking kernel stack overflow.

**Fix:** Replaced all `strcpy()` calls with `strscpy()` which provides bounded copying and returns the number of bytes written.

## 5. trelay TOCTOU and Buffer Overflow

**Files:** `package/network/services/trelay/`

**Vulnerability:** Time-of-check-time-of-use (TOCTOU) race condition combined with buffer overflow in the trelay daemon's packet handling.

**Fix:** Added proper bounds checking and eliminated the race window.

## 6. nvram.c Alignment and Overflow

**Files:** `target/linux/generic/pending-6.12/*.patch` (nvram related)

**Vulnerability:** Misaligned memory access and buffer overflow in NVRAM handling code used by multiple MIPS platforms.

**Fix:** Added alignment-safe accessors and buffer size validation.

## 7. ead-client Bounds Checks

**Files:** `package/network/services/ead/` (userspace client)

**Vulnerability:** Missing input length validation in ead-client could lead to buffer over-read/over-write.

**Fix:** Added explicit bounds checks on all receive buffers before processing.

## 8. tinysrp Multiple Fixes

**Files:** `package/libs/tinysrp/`

**Vulnerability:** Multiple issues in the SRP (Secure Remote Password) library implementation.

**Fix:** Applied upstream-recommended patches for integer overflow and buffer handling.
