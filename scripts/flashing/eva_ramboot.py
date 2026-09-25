#!/usr/bin/env python3

import argparse
import shutil
import socket
import subprocess
import time

from ftplib import FTP, error_perm
from os import stat

POLL_INTERVAL = 0.1  # seconds, retry cadence for the ICMP wait loop
# seconds, bound on a single ICMP echo attempt. These devices are normally
# flashed over a direct cable, where a healthy reply usually arrives in
# <=1ms - so 200ms already leaves generous margin; raising it would mostly
# just lengthen the wait on a genuinely absent device. It also directly eats
# into the ICMP wait loop's cadence, and EVA's FTP window is short (e.g. on
# a FRITZ!Box it's only open for about 5 seconds after boot) - bumping this
# to 1-2s would risk missing that window entirely instead of just being
# slightly slower to notice the device is up.
PING_TIMEOUT = 0.2
EVA_PORT_ATTEMPT_TIMEOUT = 0.6  # seconds, per TCP connect attempt (a refused port answers almost instantly regardless of this value; it only matters if the port is silently dropping packets)
EVA_PORT_RETRY_INTERVAL = 0.5  # seconds, pause between TCP connect attempts (a refused connection returns immediately, so this is what actually paces the retry counter)
EVA_WINDOW_WARNING_AFTER = 120  # seconds since device came online; still closed after this long means it was likely missed
CONNECT_TIMEOUT = 5  # seconds, for the actual FTP control connection/login
# seconds, upper bound for the two wait loops below. How long a given
# device takes to come back up (or open EVA's FTP port) varies too much to
# pick a tight limit, but a genuinely unreachable device/wrong IP should
# still give up eventually instead of hanging forever.
MAX_WAIT_SECONDS = 600

PING_INSTALL_HINT = """\
'ping' was not found on this system.

Install it with:
  Windows        - included by default; if missing, your install is
                   broken (repair/reinstall Windows)
  macOS          - included by default; if missing, reinstall the Xcode
                   Command Line Tools: xcode-select --install
  Debian/Ubuntu  - sudo apt install iputils-ping
  Fedora/RHEL    - sudo dnf install iputils
  Alpine         - apk add iputils   (BusyBox's built-in ping usually
                   already covers this)
  other minimal/embedded distros - install whichever package provides
                   'busybox' or 'iputils' ping.
"""


def ping_once(ip, timeout=PING_TIMEOUT):
	"""Single ICMP echo request; True if the host replies (L3 reachability only)."""
	try:
		return subprocess.run(
			["ping", "-c", "1", ip],
			stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL,
			timeout=timeout,
		).returncode == 0
	except subprocess.TimeoutExpired:
		return False


def prompt_skip_or_install(tool, install_hint):
	"""A required tool is missing; ask whether to skip the checks that
	depend on it (and connect blindly instead) or show install steps.
	Returns True if the user chose to skip; otherwise prints install
	instructions and exits."""
	print("'%s' was not found on this system (checked via PATH)." % tool)
	print("Without it this script can't tell when the device is reachable")
	print("- it would have to guess the timing blindly instead.")
	try:
		answer = input("Skip the %s check and try connecting blindly? [y/N] " % tool).strip().lower()
	except EOFError:
		answer = ""
	if answer in ("y", "yes"):
		return True
	print()
	print(install_hint)
	raise SystemExit(1)


def choose_reachability_check():
	"""Decide how to test whether the device is reachable. Currently
	always ICMP ping, which is available almost everywhere by default;
	prompts the user if it's missing. Returns None if the user chose to
	skip the reachability check entirely.

	NOTE: arping (ARP, layer 2) was evaluated as a lower-latency
	alternative for directly-connected devices - it doesn't depend on
	ICMP being allowed - but needs root/CAP_NET_RAW to send raw ARP
	frames, and its command-line flags differ between the common
	implementations (iputils-arping, Thomas Habets' arping, BusyBox's
	arping), which needs more testing across platforms than there was
	time for here. Left as a possible follow-up rather than added
	half-verified."""
	if shutil.which("ping") is not None:
		return ping_once

	if prompt_skip_or_install("ping", PING_INSTALL_HINT):
		return None


def wait_for_device(ip, check, interval=POLL_INTERVAL, timeout=MAX_WAIT_SECONDS):
	"""Poll `check` until the device answers at all (EVA or the main
	firmware), giving up after `timeout` seconds."""
	print("Waiting for the device to respond (giving up after %d minutes if it never does)..." % (timeout // 60))
	attempts = 0
	start = time.monotonic()
	while True:
		attempts += 1
		if check(ip):
			print("device: online  (retry: %d)          " % attempts)
			return
		if time.monotonic() - start > timeout:
			raise SystemExit(
				"device: no response after %d minutes - giving up. Check cabling/IP and try again."
				% (timeout // 60)
			)
		print("device: offline (retry: %d)" % attempts, end="\r", flush=True)
		time.sleep(interval)


def wait_for_eva_port(ip, port=21, interval=EVA_PORT_RETRY_INTERVAL, attempt_timeout=EVA_PORT_ATTEMPT_TIMEOUT,
                       warning_after=EVA_WINDOW_WARNING_AFTER, timeout=MAX_WAIT_SECONDS):
	"""Poll ip:port until it accepts a TCP connection (EVA's FTP window is
	open), warning once the window has likely been missed, and giving up
	entirely after `timeout` seconds."""
	print("Waiting for EVA's FTP port to open (will warn after %ds, give up after %d minutes)..."
	      % (warning_after, timeout // 60))
	attempts = 0
	warned = False
	start = time.monotonic()
	while True:
		attempts += 1
		try:
			with socket.create_connection((ip, port), timeout=attempt_timeout):
				print("eva (ftp): open   (retry: %d)          " % attempts)
				return
		except OSError:
			elapsed = time.monotonic() - start
			if elapsed > timeout:
				raise SystemExit(
					"eva (ftp): still closed after %d minutes - giving up. Power-cycle the device and try again."
					% (timeout // 60)
				)
			print("eva (ftp): closed (retry: %d)" % attempts, end="\r", flush=True)
			if not warned and elapsed > warning_after:
				print("\nWARNING: still closed after %.0fs - EVA's boot window is likely over, power-cycle the device again." % warning_after)
				warned = True
			time.sleep(interval)


def connect_ftp(ip, user='adam2', passwd='adam2', timeout=CONNECT_TIMEOUT):
	"""Open the FTP control connection and log in, reporting a clear outcome."""
	ftp = FTP(timeout=timeout)
	try:
		ftp.connect(ip)
		ftp.login(user, passwd)
	except (socket.timeout, TimeoutError):
		print("connect: timeout")
		raise SystemExit(1)
	except (ConnectionRefusedError, error_perm):
		print("connect: rejected")
		raise SystemExit(1)
	except OSError as e:
		print("connect: rejected (%s)" % e)
		raise SystemExit(1)
	print("connect: succeeded")
	# The connect timeout above must not limit the actual image transfer, which
	# can legitimately take much longer than a login handshake.
	ftp.sock.settimeout(None)
	ftp.timeout = None
	return ftp

parser = argparse.ArgumentParser(description='Tool to boot AVM EVA ramdisk images.')
parser.add_argument('ip', type=str, help='IP-address to transfer the image to')
parser.add_argument('image', type=str, help='Location of the ramdisk image')
parser.add_argument('--offset', type=lambda x: int(x,0), help='Offset to load the image to in hex format with leading 0x. Only needed for non-lantiq devices.')
args = parser.parse_args()

size = stat(args.image).st_size
# arbitrary size limit, to prevent the address calculations from overflows etc.
assert size < 0x2000000

if args.offset:
	addr = size
	haddr = args.offset
else:
	# We need to align the address.
	# A page boundary seems to be sufficient on 7362sl and 7412
	addr = ((0x8000000 - size) & ~0xfff)
	haddr = 0x80000000 + addr

img = open(args.image, "rb")
device_check = choose_reachability_check()
if device_check is not None:
	wait_for_device(args.ip, check=device_check)
else:
	print("device: skipping reachability check as requested")
wait_for_eva_port(args.ip)
ftp = connect_ftp(args.ip)

def adam(cmd):
	print("> %s"%(cmd))
	resp = ftp.sendcmd(cmd)
	print("< %s"%(resp))
	assert resp[0:3] == "200"

ftp.set_pasv(True)
# The following parameters allow booting the avm recovery system with this
# script.
adam('SETENV memsize 0x%08x'%(addr))
adam('SETENV kernel_args_tmp mtdram1=0x%08x,0x88000000'%(haddr))
adam('MEDIA SDRAM')
ftp.storbinary('STOR 0x%08x 0x88000000'%(haddr), img)
img.close()
ftp.close()

print()
print("Ramdisk transferred successfully - the device should now be booting the")
print("temporary OpenWrt system from RAM (usually reachable at 192.168.1.1 after")
print("a short delay).")
print("Continue per your device's OpenWrt flashing documentation: copy the")
print("regular sysupgrade image to the device (e.g. via scp) and run sysupgrade")
print("to install it permanently, e.g.:")
print("  scp <sysupgrade-image>.bin root@192.168.1.1:/tmp/")
print("  ssh root@192.168.1.1 sysupgrade -n /tmp/<sysupgrade-image>.bin")
