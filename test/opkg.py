import unittest
import sys
import lib.baseTest


class TestOpkg(lib.baseTest.BaseTest):
    def test_install_perf3(self):
        self.dut.send_cmd("opkg update")


if __name__ == "__main__":
    args, argv = lib.baseTest.parse_args()  # run this first

    sys.argv[:] = argv
    unittest.main()
