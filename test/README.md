# OpenWrt Testing

This allows to run automated tests in OpenWrt.

This will use existing images and start them in qemu. Later it should also be supported to run the tests on real hardware or in docker container with OpenWrt.

Requirements:
* An already build OpenWrt image
* Python 3.8 or more recent

Use this command to run tests on malta BE image:
```python3 lib/baseTest.py -dut qemu malta be```
