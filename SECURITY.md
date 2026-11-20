# Security Policy

OpenWrt security maintenance relies on volunteers. You can help by submitting
pull requests to fix vulnerabilities or backport security fixes.

See also the [OpenWrt security documentation](https://openwrt.org/docs/guide-developer/security).

## Supported versions

Security fixes cover the main development branch and the latest stable release
series. A stable series reaches End of Life when the next major release is
published and receives no further security fixes. Use the latest minor release
or a recent snapshot; not every security fix receives a CVE or advisory.

## Reporting vulnerabilities

You MUST use public issues or better a public pull request if:

- The vulnerability is already public.
- The fix only requires updating packages or backporting a public patch.
- The attack requires physical access.

You SHOULD use public issues or better a public pull request if the security issue:

- Affects a rarely used package.
- Requires an unusual configuration.

Report other, non-public vulnerabilities privately through GitHub Security
Advisories in the affected repository:

- [Main OpenWrt repository](https://github.com/openwrt/openwrt/security/advisories/new)
- [LuCI](https://github.com/openwrt/luci/security/advisories/new)
- [Packages](https://github.com/openwrt/packages/security/advisories/new)

Alternatively, email [contact@openwrt.org](mailto:contact@openwrt.org).

Include the affected version, configuration, and steps to reproduce.
Priority goes to vulnerabilities
affecting default configurations, popular packages, or commonly used settings.
