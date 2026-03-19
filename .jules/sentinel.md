## 2025-02-18 - Insecure Temporary File Creation in t_misc.c
**Vulnerability:** Predictable temporary file creation using `sprintf(dotpath, "/tmp/rnd.%d", getpid())` followed by `creat()` in `t_fshash()`.
**Learning:** This classic pattern (CWE-377) allows local attackers to pre-create symlinks at predictable paths, potentially causing arbitrary file overwrite or manipulation. `getpid()` is easily predictable.
**Prevention:** Always use `mkstemp()` to securely and atomically create temporary files with random names and safe permissions (O_EXCL). Use `fstat(fd, ...)` on the resulting file descriptor instead of `stat(path, ...)` to avoid TOCTOU race conditions.
## 2024-05-24 - Buffer Overflow in EAD Client Username
**Vulnerability:** The `ead-client` uses `strcpy` to copy a user-provided command-line argument (`username`) into a fixed-size buffer (`char username[32]`) in the `ead_msg_user` struct, leading to a stack-smashing buffer overflow if the username exceeds 31 characters.
**Learning:** Older C components might still rely on unbounded string operations for data that comes from local process arguments, trusting local input.
**Prevention:** Always use `strncpy` or `strlcpy` and explicitly null-terminate the buffer when copying arbitrary user input (even local CLI arguments) into fixed-size message structs.
