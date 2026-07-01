import unittest
import argparse
import importlib
import sys


class BaseTest(unittest.TestCase):
    def test_upper(self):
        self.assertEqual("foo".upper(), "FOO")


def parse_args():
    parser_dut = argparse.ArgumentParser(add_help=False)
    parser_dut.add_argument("-dut", default="qemu")

    parser_dut_args = parser_dut.parse_known_args()[0]

    dut_module = importlib.import_module("lib.dut." + parser_dut_args.dut)
    dut_class = getattr(dut_module, parser_dut_args.dut.capitalize())
    dut = dut_class()

    parser = argparse.ArgumentParser()
    parser.add_argument("-dut", default="qemu")

    dut.args_parser(parser)

    ns, args = parser.parse_known_args()
    dut.args_set(args)

    return ns, sys.argv[:1] + args


if __name__ == "__main__":
    args, argv = parse_args()  # run this first

    sys.argv[:] = argv
    unittest.main()
