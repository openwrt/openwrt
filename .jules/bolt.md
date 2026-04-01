## 2024-05-24 - [Avoid `email.parser` for large package indexes]
**Learning:** `email.parser.Parser` performs full RFC 822/2822 compliance checks which adds massive overhead. When parsing tens of thousands of machine-generated opkg package index blocks with predictable `Key: Value` line formats, standard string splitting and `.startswith()` checks provide a ~14x speedup.
**Action:** When extracting a few specific headers from a trusted and uniform block format instead of parsing arbitrary emails, avoid `email.parser.Parser` and use fast native python string operations instead. Make sure to use `.strip()` when parsing values to correctly handle `\r\n` line endings.

## 2024-03-15 - [Python regex optimization in JSON info file script]
**Learning:** Pre-compiling regexes outside of loops in Python utility scripts (`scripts/`) is a safe micro-optimization that yields measurable speedups (~50% faster matching) and avoids repeatedly hitting Python's internal regex cache.
**Action:** When working on Python scripts that iterate over many files or lines and use `re.match` or `re.search` with a static pattern, pre-compile the pattern using `re.compile()` outside the loop.

## 2024-05-25 - [Python XOR Performance Optimization]
**Learning:** Python's native `bytearray` generator expressions (`bytearray(a ^ b for a, b in zip(...))`) are notoriously slow for large binaries like firmware because they execute at the interpreter level per byte. Conversely, Python 3's arbitrary-precision integers allow for extremely fast, C-level bitwise operations (`int.from_bytes(data) ^ int.from_bytes(key)`) without overflow limits.
**Action:** When implementing cryptographic or bitwise operations (like XOR) over large byte arrays in Python scripts, construct a full-length repeating key, convert both payload and key to large integers via `int.from_bytes()`, perform the bitwise operation natively, and convert back using `.to_bytes()`. This consistently yields ~8-10x performance improvements.

## 2024-05-26 - Removed unused email.parser import
**Learning:** The `email.parser` library in Python adds significant startup overhead (over 1s in some environments) simply by being imported, pulling in a large chunk of standard library infrastructure. In utility scripts like `make-index-json.py`, which are invoked frequently and explicitly optimize away from using `email.parser` for performance reasons, leaving the unused import negates part of the performance win.
**Action:** Always verify that optimized code paths also remove any unused heavy dependencies from the import block, as Python module loading can be a hidden bottleneck for frequently run CLI scripts.
## 2024-05-15 - Unused `email.parser` import overhead in CLI scripts
**Learning:** The `email.parser` module is a "heavy" standard library module that can add significant startup overhead (e.g., ~0.3s to ~1.5s depending on system). Importing it in Python utility scripts, even if unused, unnecessarily penalizes script execution time.
**Action:** Always audit Python CLI scripts for unused heavy standard library imports (especially `email.parser`) and remove them to optimize startup times for scripts that are frequently invoked in build processes or loops.

## 2024-06-25 - Python chunked file I/O optimization
**Learning:** While `iter(lambda: f.read(CHUNK_SIZE), b"")` is a common Python pattern for reading files in chunks, it introduces lambda invocation overhead for every chunk. A standard `while True:` loop is measurably faster (approx ~5-10% faster for large files) by avoiding this overhead.
**Action:** When writing scripts that need to hash or process large binary files in chunks, prefer the explicit `while True:` and `f.read()` pattern over the lambda-based iterator approach to maximize performance, especially since file I/O operations are common in build scripts.

## 2024-05-27 - [Python Hashlib Chunked File I/O for Large Binaries]
**Learning:** Loading large files entirely into memory to calculate hashes (e.g. `hashlib.md5(f.read())`) causes huge memory spikes and slows down execution due to memory allocation overhead, especially in firmware packaging scripts dealing with large images.
**Action:** Always use a chunked reading approach in a `while True` loop with `f.read(65536)` and `hash.update(chunk)` when hashing large binary files like firmware images.

## 2024-05-28 - [Python dict get with default vs `in` check for list iterations]
**Learning:** When parsing large JSON index files containing lists of dictionaries (like APK package indexes), iterating over a list field by calling `package.get("tags", [])` forces the creation of a new empty list on every miss. A simple `if "tags" in package:` check avoids this allocation entirely and is measurably faster. Additionally, using string slicing (`tag[19:]`) instead of `tag.split("=")[-1]` for a known prefix is considerably faster.
**Action:** In high-volume parsing loops over dictionaries (e.g., thousands of packages), avoid using `.get(key, [])` if the key is frequently missing. Use an explicit `if key in dict:` check instead. For extracting values past known string prefixes, use slicing (e.g., `s[len(prefix):]`) instead of `.split()`.

## 2024-05-29 - [Avoid reading entire large files into memory in Sercomm payload script]
**Learning:** Reading massive binary files directly into memory before hashing (e.g. `f.read(in_size)`) causes huge memory allocations that scale linearly with the file size, making $O(N)$ memory usage and unnecessary GC sweeps.
**Action:** Always process large binary files using a chunked approach inside a `while True:` loop (e.g., `f.read(65536)`). If a file must be written with a prepended hash of its contents, initially inject a byte placeholder, stream the incoming data chunks to the output, and seek back to insert the finalized hash when finished, keeping memory usage constant $O(1)$.
