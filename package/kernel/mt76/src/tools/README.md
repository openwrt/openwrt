# MT76 testmode utility

This utility can be used to perform some support functions required for Rx/Tx calibration, similar to the ATE command set of the SDK driver.
Its main functionality is setting test parameters and dumping statistics. It can also be used to prepare modified EEPROM data for writing into flash.

## Basic syntax

- Set parameters:
	- `mt76-test phy0 set <parameter>=<value>`[...]
- Show current parameter set:
	- `mt76-test phy0 dump`
- Show statistics
	- `mt76-test phy0 dump stats`

## Running tests

The test state is controlled through the `state` parameter. The following state values are supported:

- `off`: Normal operation (default)
- `idle`: Testmode enabled, but no specific test active
- `tx_frames`: Send a number of packets with configurable rate/txpower
- `rx_frames`: Receive packets and show RSSI and packet count/PER

Setting a state activates it even if the value is the same as before. Setting it to `tx_frames` triggers sending packets immediately. Setting `rx_frames` enables receive mode and can also be used to clear rx statistics.

## Notes

To run tests, you first need to disable all normal interfaces, set up a monitor mode interface and configure it to the channel/bandwidth you intend to use.


## Parameters:

| Parameter name | ATE parameter | Description |
|--|--|--|
| `state` | `ATE` | Test state |
| `tx_count` | `ATETXCNT` | Number of packets to send |
| `tx_length` | `ATETXLEN` | Length of packets to send |
| `tx_rate_mode` | `ATETXMODE` | PHY mode (possible values: `cck`, `ofdm`, `ht`, `vht`) |
| `tx_rate_nss` | | Number of spatial streams (VHT only) |
| `tx_rate_idx` | `ATETXMCS` | MCS or legacy rate index |
| `tx_rate_sgi` | `ATETXGI` | Enable short guard interval |
| `tx_rate_ldpc` | `ATETXLDPC` | Enable LDPC |
| `tx_power_control` | `ATETXPOWERCTRL` | Firmware transmit power control feature |
| `tx_power` | `ATETXPOW0-3` | Per-chain half-dBm transmit power, `0` means default value, e.g. `10,0,0,0` |
| `tx_antenna` | `ATETXANT` | Transmit antenna bitmask |
| `freq_offset` | `ATETXFREQOFFSET` | Frequency offset |


