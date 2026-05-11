import pexpect
import sys
import time

import lib.dut.dut as dut


class Qemu(dut.Dut):
    def __init__(self):
        self.child = None

    def start(self):
        pass

    def stop(self):
        pass

    def send_cmd(self, cmd):
        pass

    def args_parser(self, parser):
        parser.add_argument("target", help="target name")
        parser.add_argument("subtarget", help="sub target name")

    def args_set(self, args):
        self.args = args
