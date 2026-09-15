#!/usr/bin/env python3

import argparse
import socket
import subprocess
import time

from ftplib import FTP, error_perm
from os import stat

POLL_INTERVAL = 0.1  # seconds, retry cadence for the ICMP wait loop
PING_TIMEOUT = 0.2  # seconds, bound on a single ICMP echo attempt
EVA_PORT_ATTEMPT_TIMEOUT = 0.6  # seconds, per TCP connect attempt (a refused port answers almost instantly regardless of this value; it only matters if the port is silently dropping packets)
EVA_PORT_RETRY_INTERVAL = 0.5  # seconds, pause between TCP connect attempts (a refused connection returns immediately, so this is what actually paces the retry counter)
EVA_WINDOW_WARNING_AFTER = 120  # seconds since device came online; still closed after this long means it was likely missed
CONNECT_TIMEOUT = 5  # seconds, for the actual FTP control connection/login


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


def wait_for_device(ip, interval=POLL_INTERVAL):
	"""Poll ICMP until the device answers at all (EVA or the main firmware)."""
	attempts = 0
	while True:
		attempts += 1
		if ping_once(ip):
			print("device: online  (retry: %d)          " % attempts)
			return
		print("device: offline (retry: %d)" % attempts, end="\r", flush=True)
		time.sleep(interval)


def wait_for_eva_port(ip, port=21, interval=EVA_PORT_RETRY_INTERVAL, attempt_timeout=EVA_PORT_ATTEMPT_TIMEOUT,
                       warning_after=EVA_WINDOW_WARNING_AFTER):
	"""Poll ip:port until it accepts a TCP connection (EVA's FTP window is open)."""
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
			print("eva (ftp): closed (retry: %d)" % attempts, end="\r", flush=True)
			if not warned and time.monotonic() - start > warning_after:
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
		raise
	except (ConnectionRefusedError, error_perm):
		print("connect: rejected")
		raise
	except OSError as e:
		print("connect: rejected (%s)" % e)
		raise
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
wait_for_device(args.ip)
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
