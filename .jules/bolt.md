## 2024-05-24 - [Avoid `email.parser` for large package indexes]
**Learning:** `email.parser.Parser` performs full RFC 822/2822 compliance checks which adds massive overhead. When parsing tens of thousands of machine-generated opkg package index blocks with predictable `Key: Value` line formats, standard string splitting and `.startswith()` checks provide a ~14x speedup.
**Action:** When extracting a few specific headers from a trusted and uniform block format instead of parsing arbitrary emails, avoid `email.parser.Parser` and use fast native python string operations instead. Make sure to use `.strip()` when parsing values to correctly handle `\r\n` line endings.

## 2024-03-15 - [Python regex optimization in JSON info file script]
**Learning:** Pre-compiling regexes outside of loops in Python utility scripts (`scripts/`) is a safe micro-optimization that yields measurable speedups (~50% faster matching) and avoids repeatedly hitting Python's internal regex cache.
**Action:** When working on Python scripts that iterate over many files or lines and use `re.match` or `re.search` with a static pattern, pre-compile the pattern using `re.compile()` outside the loop.

## 2024-05-25 - [Python XOR Performance Optimization]
**Learning:** Python's native `bytearray` generator expressions (`bytearray(a ^ b for a, b in zip(...))`) are notoriously slow for large binaries like firmware because they execute at the interpreter level per byte. Conversely, Python 3's arbitrary-precision integers allow for extremely fast, C-level bitwise operations (`int.from_bytes(data) ^ int.from_bytes(key)`) without overflow limits.
**Action:** When implementing cryptographic or bitwise operations (like XOR) over large byte arrays in Python scripts, construct a full-length repeating key, convert both payload and key to large integers via `int.from_bytes()`, perform the bitwise operation natively, and convert back using `.to_bytes()`. This consistently yields ~8-10x performance improvements.

## 2024-05-26 - [Python `sum(iter(buf))` vs `sum(buf)` Performance]
**Learning:** `sum(iter(buf))` forces Python to create an iterator for the byte string and processes each byte individually inside the Python VM. By comparison, `sum(buf)` passes the entire byte sequence directly to the underlying C implementation, yielding faster and more memory-efficient execution without unnecessary type conversions.
**Action:** When calculating a simple additive checksum in Python scripts for OpenWrt tools (like `cameo-imghdr.py`), prefer using `sum(buf)` directly rather than wrapping it in an iterator or using list comprehensions.

## 2024-05-26 - Removed unused email.parser import
**Learning:** The `email.parser` library in Python adds significant startup overhead (over 1s in some environments) simply by being imported, pulling in a large chunk of standard library infrastructure. In utility scripts like `make-index-json.py`, which are invoked frequently and explicitly optimize away from using `email.parser` for performance reasons, leaving the unused import negates part of the performance win.
**Action:** Always verify that optimized code paths also remove any unused heavy dependencies from the import block, as Python module loading can be a hidden bottleneck for frequently run CLI scripts.
## 2024-05-15 - Unused `email.parser` import overhead in CLI scripts
**Learning:** The `email.parser` module is a "heavy" standard library module that can add significant startup overhead (e.g., ~0.3s to ~1.5s depending on system). Importing it in Python utility scripts, even if unused, unnecessarily penalizes script execution time.
**Action:** Always audit Python CLI scripts for unused heavy standard library imports (especially `email.parser`) and remove them to optimize startup times for scripts that are frequently invoked in build processes or loops.
## 2025-01-20 - Optimize parse_opkg in make-index-json.py
**Learning:** For extremely large text payloads like OPKG index files, the `splitlines()` function on each chunk is still relatively slow because it allocates many intermediate string objects in memory. Using `str.find` to pinpoint exactly where the lines start and end avoids allocating memory for the rest of the file contents.
**Action:** When parsing well-structured, multi-line blocks where only a few lines are needed, prefer `str.find()` over splitting all lines to dramatically reduce memory allocation and increase performance (~2.8x speedup).

## 2024-06-25 - Python chunked file I/O optimization
**Learning:** While `iter(lambda: f.read(CHUNK_SIZE), b"")` is a common Python pattern for reading files in chunks, it introduces lambda invocation overhead for every chunk. A standard `while True:` loop is measurably faster (approx ~5-10% faster for large files) by avoiding this overhead.
**Action:** When writing scripts that need to hash or process large binary files in chunks, prefer the explicit `while True:` and `f.read()` pattern over the lambda-based iterator approach to maximize performance, especially since file I/O operations are common in build scripts.

## 2024-05-27 - [Python Hashlib Chunked File I/O for Large Binaries]
**Learning:** Loading large files entirely into memory to calculate hashes (e.g. `hashlib.md5(f.read())`) causes huge memory spikes and slows down execution due to memory allocation overhead, especially in firmware packaging scripts dealing with large images.
**Action:** Always use a chunked reading approach in a `while True` loop with `f.read(65536)` and `hash.update(chunk)` when hashing large binary files like firmware images.

## 2024-05-28 - [Python dict get with default vs `in` check for list iterations]
**Learning:** When parsing large JSON index files containing lists of dictionaries (like APK package indexes), iterating over a list field by calling `package.get("tags", [])` forces the creation of a new empty list on every miss. A simple `if "tags" in package:` check avoids this allocation entirely and is measurably faster. Additionally, using string slicing (`tag[19:]`) instead of `tag.split("=")[-1]` for a known prefix is considerably faster.
**Action:** In high-volume parsing loops over dictionaries (e.g., thousands of packages), avoid using `.get(key, [])` if the key is frequently missing. Use an explicit `if key in dict:` check instead. For extracting values past known string prefixes, use slicing (e.g., `s[len(prefix):]`) instead of `.split()`.
## 2024-04-02 - [Make OPKG Parsing 2x Faster]
**Learning:** In Python, string `.find()` followed by string slicing is significantly faster and more memory-efficient than calling `.split("\n")` within loops because it avoids repeated creation and teardown of list structures. Finding `\n` anchors correctly bypasses `.startswith` issues on inner chunks.
**Action:** When extracting multiple text fields from a large multi-line string block, avoid `.splitlines()` or `.split("\n")` per block. Instead, use `str.find()` with slice indexing `str[start:end]` to extract targets in-place.

## 2024-05-29 - [Avoid reading entire large files into memory in Sercomm payload script]
**Learning:** Reading massive binary files directly into memory before hashing (e.g. `f.read(in_size)`) causes huge memory allocations that scale linearly with the file size, making $O(N)$ memory usage and unnecessary GC sweeps.
**Action:** Always process large binary files using a chunked approach inside a `while True:` loop (e.g., `f.read(65536)`). If a file must be written with a prepended hash of its contents, initially inject a byte placeholder, stream the incoming data chunks to the output, and seek back to insert the finalized hash when finished, keeping memory usage constant $O(1)$.

## 2024-05-30 - [Python Hashlib Streamed In-Place Prepending]
**Learning:** Appending headers/hashes to a large file by reading the entire payload into a variable (e.g. `in_bytes = f.read()`) creates immense memory bloat (O(N) memory complexity), which can cause the process to hit OOM on systems building large firmware images.
**Action:** When a Python script needs to prepend a dynamically calculated hash (like SHA-256) to a large payload, avoid reading the whole file. Instead, save the output file pointer (`hash_pos = out_f.tell()`), write a placeholder of the exact hash size (`b'\0' * 32`), iteratively read the input in chunks to both calculate the hash and write the chunks directly to the output file, and finally `seek()` back to `hash_pos` to overwrite the placeholder with the computed digest. This brings memory complexity to O(1).

## 2025-01-20 - [Python `shutil.copyfileobj` optimization for I/O bounds]
**Learning:** When appending/prepending headers to large binary files like firmware images in Python scripts, simply reading the entire input file into memory (`in_bytes = f.read()`) and then writing it back (`out_f.write(in_bytes)`) scales linearly in memory size O(N) and can cause high overhead and memory exhaustion.
**Action:** Use `shutil.copyfileobj(in_f, out_f)` to stream the contents of the file directly without reading the entire file contents into memory, reducing memory complexity to O(1) and performing the copy faster since it is optimized at the C level.

## 2025-01-20 - [Optimize dictionary population in dl_cleanup.py]
**Learning:** Using `in dict.keys()` within a loop is an anti-pattern that creates unnecessary view objects on every iteration, leading to reduced performance.
**Action:** Use `dict.setdefault()` or `collections.defaultdict` when grouping items into lists within dictionaries to improve performance and code readability.

## 2025-01-20 - [Python CRC32 Chunked File I/O for Large Binaries]
**Learning:** Reading a large file into memory entirely to compute its CRC32 checksum (`in_bytes = f.read(in_size); crc = binascii.crc32(in_bytes)`) creates massive memory bloat, causing O(N) memory complexity and potentially crashing the script on systems building large firmware images.
**Action:** Use a chunked reading approach inside a `while True:` loop (e.g., `chunk = f.read(65536)`) and calculate the CRC incrementally by passing the previous CRC value into the function call (e.g., `crc = binascii.crc32(chunk, crc)`). This brings memory complexity down to O(1).

## 2025-01-20 - [Python CRC32 Chunked File I/O in sercomm-kernel-header]
**Learning:** In `scripts/sercomm-kernel-header.py`, reading entire kernel and rootfs files into memory with `.read()` and `.read(rootfs_size)` causes excessive memory bloat (O(N) complexity). Memory limit issues can easily happen on large firmware images.
**Action:** Use an incremental CRC approach over chunked reads for large files (`crc = binascii.crc32(chunk, crc)`). This maintains O(1) memory usage, improving efficiency and memory footprint.

## 2026-04-11 - [string.join deprecation and performance issue]
**Learning:** In Python 3, string.join() was removed. The native ''.join(iterable) is the correct alternative, and also yields a performance improvement.
**Action:** Replace string.join with ''.join in the codebase where encountered.

## 2026-04-18 - [Use enumerate instead of range(len) for list iteration]
**Learning:** In Python, using `range(len(list))` or `range(0, len(list))` to iterate over a list and access its elements by index is an anti-pattern. Index lookups (`list[i]`) are slower than using `enumerate(list)` which yields the index and the item directly without the overhead of an extra lookup.
**Action:** When an item must be found and removed from a list (or when both index and item are needed), use `for i, item in enumerate(list):` with `del list[i]` and `break` instead of `range(len(list))` to avoid unidiomatic index lookups, thereby improving performance and code readability.
## 2024-05-30 - [Optimize memory usage in cameo-tag.py]
**Learning:** Reading a large binary file entirely into memory using `.read(READ_UNTIL_EOF)` to calculate a checksum (e.g. `sum()`) causes massive memory bloat (O(N) complexity).
**Action:** Use a chunked reading approach in a `while True:` loop and calculate the sum incrementally (e.g., `checksum = (checksum + sum(chunk)) % (1<<32)`). This brings memory complexity to O(1).

## 2024-05-27 - [Python Binary Header Construction: `struct.pack` vs Multiple Appends]
**Learning:** Constructing binary headers by sequentially appending to a `bytearray` and using multiple `to_bytes()` calls is slow. Packing the entire contiguous header struct using a single `struct.pack()` format string shifts the byte-assembly loop to C, significantly reducing interpreter overhead.
**Action:** When creating fixed-size headers, always use a single `struct.pack()` with appropriate format characters (e.g., `>IIH33s21sI`) rather than chaining individual `to_bytes` operations or custom string padding functions.
## YYYY-MM-DD - [Optimize Python script dictionary lookup]
**Learning:** In Python 3 scripts, avoid using `setdefault()` in performance-sensitive dictionary groupings because it evaluates the default argument on every call, creating unnecessary intermediate objects (like empty lists). Using `collections.defaultdict(list)` avoids this per-iteration allocation and speeds up grouping.
**Action:** Replace `dict.setdefault(key, []).append(val)` with `collections.defaultdict(list)` in scripts like `scripts/dl_cleanup.py`.

## 2024-05-02 - Optimize b43-fwsquash.py performance
**Learning:** Checking for intersections between two collections inside a loop using nested loops or helper functions creates an O(N*M) bottleneck, which is particularly evident in firmware selection tools.
**Action:** Pre-convert the static or command-line-provided parameters into `set` objects once at the script's entry point, and use the `set.isdisjoint()` method inside the loop to achieve O(min(N, M)) time complexity. Replace list aggregations in the cleanup loop with a set using `.add()` to ensure O(1) lookup.

## $(date +%Y-%m-%d) - [Python Binary Header Construction: Combine `struct.pack` calls]
**Learning:** Constructing binary headers by executing multiple sequential `struct.pack()` and `file.write()` calls is inefficient in Python. Consolidating them into a single `struct.pack()` with a compound format string significantly reduces function call and interpreter overhead.
**Action:** When generating fixed-size binary headers, group all fields into a single format string (e.g., `!I20s16sBBBBII10s2x`) and pass the corresponding arguments to a single `struct.pack()` call, then perform a single `file.write()`.

## 2024-05-13 - [Python Header Prepends with CRC]
**Learning:** When generating a binary header that contains the file size and CRC of a large payload, reading the whole payload into memory to compute the values and prepend the header results in O(N) memory complexity and huge memory usage spikes for large files.
**Action:** Use a placeholder for the header, stream the payload in chunks (`f.read(65536)`) to both calculate the CRC/size and write the chunks to the output, then `seek(0)` and overwrite the placeholder with the final computed header. This keeps memory usage strictly O(1) and is significantly faster and safer.

## 2024-05-31 - [Python Chunked Streaming in tplink-mkimage-2022]
**Learning:** In firmware image manipulation tools like `scripts/tplink-mkimage-2022.py`, reading entire image sections into memory via `.read(section['size'])` and storing them in variables before writing causes massive O(N) memory overhead and potential OOM errors for multi-megabyte firmware components like the rootfs.
**Action:** Use a chunked reading approach (`while bytes_left > 0: chunk = f.read(min(65536, bytes_left))`) or `shutil.copyfileobj()` when transferring binary payload sections directly between input and output files to maintain strictly O(1) memory overhead.
