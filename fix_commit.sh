git commit --amend --message="🛡️ Sentinel: Fix buffer overflow and alignment in nvram.c

Replaced unbounded strcpy calls in _nvram_realloc with strlcpy and added explicit
SIZE_MAX bounds checking for memory allocation sizes to prevent potential heap buffer
overflows and TOCTOU vulnerabilities. Also updated pointer to integer casts from
(int) to (uintptr_t) in nvram_commit to ensure proper memory alignment calculation
and prevent compiler warnings on 64-bit platforms.

Signed-off-by: google-labs-jules[bot] <161369871+google-labs-jules[bot]@users.noreply.github.com>"
