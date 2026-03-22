## 2025-02-18 - Insecure Temporary File Creation in t_misc.c
**Vulnerability:** Predictable temporary file creation using `sprintf(dotpath, "/tmp/rnd.%d", getpid())` followed by `creat()` in `t_fshash()`.
**Learning:** This classic pattern (CWE-377) allows local attackers to pre-create symlinks at predictable paths, potentially causing arbitrary file overwrite or manipulation. `getpid()` is easily predictable.
**Prevention:** Always use `mkstemp()` to securely and atomically create temporary files with random names and safe permissions (O_EXCL). Use `fstat(fd, ...)` on the resulting file descriptor instead of `stat(path, ...)` to avoid TOCTOU race conditions.
## 2024-05-24 - Buffer Overflow in EAD Client Username
**Vulnerability:** The `ead-client` uses `strcpy` to copy a user-provided command-line argument (`username`) into a fixed-size buffer (`char username[32]`) in the `ead_msg_user` struct, leading to a stack-smashing buffer overflow if the username exceeds 31 characters.
**Learning:** Older C components might still rely on unbounded string operations for data that comes from local process arguments, trusting local input.
**Prevention:** Always use `strncpy` or `strlcpy` and explicitly null-terminate the buffer when copying arbitrary user input (even local CLI arguments) into fixed-size message structs.
## 2024-05-24 - Out-of-bounds write in ead.c message decryption
**Vulnerability:** In `package/network/services/ead/src/ead.c`, `handle_send_cmd` decrypts a message from the network and subtracts a header size to get `datalen`. Without an upper bound check, `datalen` could exceed the fixed buffer size (`msgbuf`), allowing `cmd->data[datalen] = 0;` to write a null byte out-of-bounds, potentially causing a crash or remote code execution.
**Learning:** Decrypted network payloads must always have their length validated against an upper limit before being used as an index, especially when writing to static buffers in C network services.
**Prevention:** Always add explicit upper bounds checks on decrypted data lengths (e.g., `if (datalen > 1024) return false;`) immediately after lower bounds checks.
## 2024-05-24 - Buffer Overflow in ead-client cmd Argument
**Vulnerability:** A heap/BSS buffer overflow existed in `package/network/services/ead/src/ead-client.c`'s `send_command` due to unchecked `strlen(command)` when calculating the message size for `ead_encrypt_message`. Though `strncpy` capped copying to 1024 bytes, a lack of null-termination and passing `strlen(command)` to encryption allowed out-of-bounds reads/writes if the command string exceeded 1024 bytes.
**Learning:** Even when bounded string copying functions (`strncpy`) are used, if the original unbounded length (`strlen`) is reused for subsequent buffer sizing or cryptographic operations on fixed-size buffers, it can lead to memory corruption out-of-bounds of the allocated structure (`struct ead_msg` / `msgbuf`).
**Prevention:** Always calculate and store the bounded length into a local variable *first*, then strictly use that bounded length for all subsequent buffer allocations, string copies (ensuring explicit null-termination), and size calculations within the function block.
