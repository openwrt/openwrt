#!/usr/bin/env python3

import argparse
import pathlib

from ftplib import FTP
from os import stat


def check_for_tftpy():
    import importlib.util
    found_tftpy = importlib.util.find_spec("tftpy") is not None
    assert found_tftpy == True, 'Please install tftpy: pip install tftpy'


def start_tftp_server(ramdisk_filepath: pathlib.Path):
    import shutil
    import os.path
    from tftpy import TftpServer
    from threading import Thread

    for device in ['1200', '4040', '7530']:
        shutil.copyfile(ramdisk_filepath, os.path.join(
            tmpdir.name, f"FRITZ{device}.bin"))

    new_tftp_server = TftpServer(tmpdir.name)
    Thread(target=new_tftp_server.listen, daemon=True).start()
    return new_tftp_server


parser = argparse.ArgumentParser(
    description='Tool to boot AVM EVA ramdisk images on ipq40xx systems (Fritz!Box 1200, 4040, 7520 and 7530).')
parser.add_argument('uboot_image', type=pathlib.Path,
                    help='Location of the u-boot image. Can be found at https://downloads.openwrt.org/snapshots/targets/ipq40xx/generic/u-boot-fritz...')
parser.add_argument('-i', '--ramdisk-image', type=pathlib.Path,
                    help='Launch a TFTP server and host the image. Requires tftpy installed.', required=False)
parser.add_argument('--ip', type=str, help='IP-address to transfer the u-boot image to',
                    default='192.168.178.1', required=False)
args = parser.parse_args()

if args.ramdisk_image:
    check_for_tftpy()
    import tempfile
    # automatically cleaned up after process termination
    tmpdir = tempfile.TemporaryDirectory()
    try:
        tftp_server = start_tftp_server(args.ramdisk_image)
    except PermissionError:
        print("Error: Permission denied to launch tftp server on port 69. Run as sudo?")
        raise
    print("Verify that you have the IP 192.168.1.70 assigned to your interface.")
else:
    print("Make sure you are running a TFTP server with the ramdisk image on 192.168.1.70.")

size = stat(args.uboot_image).st_size
# arbitrary size limit, to prevent the address calculations from overflows etc.
assert size < 0x2000000

addr = size
haddr = 0x85000000

with FTP(args.ip, 'adam2', 'adam2') as ftp:

    def adam(cmd):
        print(f"> {cmd}")
        resp = ftp.sendcmd(cmd)
        print(f"< {resp}")
        assert resp[0:3] == "200"

    ftp.set_pasv(True)
    # The following parameters allow booting the avm recovery system with this
    # script.
    adam(f'SETENV memsize 0x{addr:08x}')
    adam(f'SETENV kernel_args_tmp mtdram1=0x{haddr:08x},0x88000000')
    adam('MEDIA SDRAM')
    with open(args.uboot_image, "rb") as img:
        ftp.storbinary(f'STOR 0x{haddr:08x} 0x88000000', img)

if args.ramdisk_image:
    from time import sleep
    sleep(15)  # it takes ~8 seconds to initiate download via TFTP
    tftp_server.stop()  # gracefully shutdown after transfer is finished
