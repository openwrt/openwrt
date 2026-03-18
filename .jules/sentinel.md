## 2025-02-18 - Insecure Temporary File Creation in t_misc.c
**Vulnerability:** Predictable temporary file creation using `sprintf(dotpath, "/tmp/rnd.%d", getpid())` followed by `creat()` in `t_fshash()`.
**Learning:** This classic pattern (CWE-377) allows local attackers to pre-create symlinks at predictable paths, potentially causing arbitrary file overwrite or manipulation. `getpid()` is easily predictable.
**Prevention:** Always use `mkstemp()` to securely and atomically create temporary files with random names and safe permissions (O_EXCL). Use `fstat(fd, ...)` on the resulting file descriptor instead of `stat(path, ...)` to avoid TOCTOU race conditions.
