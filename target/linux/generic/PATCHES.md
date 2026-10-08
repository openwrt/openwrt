# OpenWrt kernel patches

All patches must be in a way that they are potentially upstreamable, meaning:

* They must contain a proper subject.
* They must contain a proper commit message explaining what they change and why.
* They must contain a valid Signed-off-by line.

The build system applies the patches in the following order:

1. target/linux/generic/backport-*/
2. target/linux/generic/pending-*/
3. target/linux/generic/hack-*/
4. target/linux/`<target name>`/patches-*/

## OpenWrt generic

The generic target patches are applied to every OpenWrt target before any of the specific target patches are applied.

### backport

The `backport-*` subdirectories contain the kernel patches backported from newer kernels.

In order to save naming space for newer patches to come, when adding a new subset of patches which are related between them the following naming convention shall be used: `NNN-SS-vX.Y-lowercase_shortname.patch`.
For single patches, the following naming convention shall be used instead: `NNN-vX.Y-lowercase_shortname.patch`.

| Item  | Description              |
| ----- |:------------------------:|
| NNN   | Main patch number.       |
| SS    | Subset patch number.     |
| vX.Y  | Upstream kernel version. |

### hack

The `hack-*` subdirectories contain the downstream kernel patches that are less likely to be accepted upstream but are still needed in OpenWrt.
Some of these patches are potentially upstreamable if properly reworked and others aren't simply worth the effort.

All patches should be named `NNN-lowercase_shortname.patch`.

### pending

The `pending-*` subdirectories contain the kernel patches awaiting upstream merge.

All patches should be named `NNN-lowercase_shortname.patch`.

### Patch number guidelines

The following patch numbering guidelines shall be followed:

| NNN  | Description                     |
| ---- |:-------------------------------:|
| 0xx  | -                               |
| 1xx  | -                               |
| 2xx  | Kernel build / config / header  |
| 3xx  | Architecture specific           |
| 4xx  | MTD                             |
| 5xx  | Filesystem                      |
| 6xx  | Generic network                 |
| 7xx  | Network / PHY                   |
| 8xx  | Other drivers                   |
| 9xx  | Uncategorized                   |

## OpenWrt targets

The `patches-*` subdirectories contain the kernel patches applied for every OpenWrt target.

All patches should be named `NNN-lowercase_shortname.patch`.

### Patch number guidelines

The following patch numbering guidelines shall be followed:

| NNN  | Description                     |
| ---- |:-------------------------------:|
| 0xx  | Upstream backports              |
| 1xx  | Awaiting upstream merge         |
| 2xx  | Kernel build / config / header  |
| 3xx  | Architecture specific           |
| 4xx  | MTD                             |
| 5xx  | Filesystem                      |
| 6xx  | Generic network                 |
| 7xx  | Network / PHY                   |
| 8xx  | Other drivers                   |
| 9xx  | Uncategorized                   |
