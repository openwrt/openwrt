## 2024-03-13 - [Fix SSL Certificate Verification in GitHub Archive Downloader]
**Vulnerability:** `scripts/dl_github_archive.py` created an unverified SSL context (`ssl._create_unverified_context()`) when making requests to the GitHub API, disabling SSL/TLS certificate verification. This left the script vulnerable to Man-in-the-Middle (MITM) attacks.
**Learning:** This vulnerability existed likely to bypass SSL errors on developer machines with outdated or missing CA certificates.
**Prevention:** Always use `ssl.create_default_context()` or let the system use its default verified context when making HTTPS requests. Do not disable SSL verification in production scripts, especially when downloading source code archives.
## 2025-05-18 - [CRITICAL] Fix command injection in unet.uc
**Vulnerability:** The network name parameter was only checked for forward slashes before being directly substituted into shell strings, leading to command injection and path traversal risks via `system()` calls.
**Learning:** `ucode` scripts using `system()` with backticks or string concatenation are highly vulnerable to injection if variables are not rigorously validated, especially in user-facing configuration paths.
**Prevention:** Always use regex matching (e.g., `match(name, /[^a-zA-Z0-9_-]/)`) to strictly whitelist input characters for system parameters before interpolating them into shell commands.
## 2024-05-23 - [CRITICAL] Fix command injection in unet.uc via network_keygen config
**Vulnerability:** The `network_keygen` function in `unet.uc` constructed shell command strings without validating configuration variables like `config.rounds` and `config.salt`, leading to a command injection vulnerability. An attacker capable of modifying config JSON files could inject arbitrary commands.
**Learning:** Even internal configuration objects like `config.salt` must be strictly validated before being interpolated into `system()` strings to prevent command injection, especially when restoring configurations from JSON.
**Prevention:** Use strict regex whitelists (e.g., `match(salt, /[^a-fA-F0-9]/)`) to validate integer and hex inputs before using them in shell commands.
## 2024-05-24 - [CRITICAL] Fix buffer overflow in ead client
**Vulnerability:** ead-client.c and t_pw.c used strcpy to copy user input into fixed-size character arrays without checking the length, causing buffer overflows.
**Learning:** Legacy string handling mechanisms that map structures onto network packets or directly copy from command line arguments are vulnerable when using unsafe functions like strcpy.
**Prevention:** Always use safe string operations like strncpy, and ensure that buffers are correctly bounded and null-terminated.
