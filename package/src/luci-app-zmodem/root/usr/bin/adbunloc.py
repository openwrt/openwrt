import logging
import os
import argparse
import sys
import hashlib

def generateUnlockKey(sn):
    """
    @param sn: the serial number to generate an unlock key for
    """
    salt = "$1${0}$".format(sn).encode('utf-8')  # Convert salt to bytes
    key = "SH_adb_quectel".encode('utf-8')  # Convert key to bytes
    hashed = hashlib.sha512(salt + key).hexdigest()
    return hashed[12:27]

def main():
    key = input("请输入模块解锁请求码: ")
    c = generateUnlockKey(key)
    print('ADB解锁码：{0}'.format(c))

if __name__ == "__main__":
    logging.basicConfig(format='[%(levelname)s] %(message)s', level=logging.ERROR)
    try:
        main()
    except Exception as e:
        logging.error(e)
