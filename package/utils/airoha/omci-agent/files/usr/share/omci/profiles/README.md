# OMCI profile scripts

Each executable `*.sh` profile supports two operations:

- `describe`: prints one JSON object consumed by the LuCI profile page.
- `apply`: validates its inputs, writes `/etc/config/omci`, commits UCI, and prints a JSON result.

The RPC helper passes form inputs as environment variables. An input named
`gpon_sn` is available to the script as `OMCI_INPUT_GPON_SN`; `slid` is exposed
as `OMCI_INPUT_SLID`. Input names are restricted to lowercase letters, digits,
and underscores. Profile scripts must validate every value before inserting it
into a UCI batch.
