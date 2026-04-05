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

## 2024-05-30 - [Python Hashlib Streamed In-Place Prepending]
**Learning:** Appending headers/hashes to a large file by reading the entire payload into a variable (e.g. `in_bytes = f.read()`) creates immense memory bloat (O(N) memory complexity), which can cause the process to hit OOM on systems building large firmware images.
**Action:** When a Python script needs to prepend a dynamically calculated hash (like SHA-256) to a large payload, avoid reading the whole file. Instead, save the output file pointer (`hash_pos = out_f.tell()`), write a placeholder of the exact hash size (`b'\0' * 32`), iteratively read the input in chunks to both calculate the hash and write the chunks directly to the output file, and finally `seek()` back to `hash_pos` to overwrite the placeholder with the computed digest. This brings memory complexity to O(1).
## 2025-01-20 - [Python `shutil.copyfileobj` optimization for I/O bounds]
**Learning:** When appending/prepending headers to large binary files like firmware images in Python scripts, simply reading the entire input file into memory (`in_bytes = f.read()`) and then writing it back (`out_f.write(in_bytes)`) scales linearly in memory size O(N) and can cause high overhead and memory exhaustion.
**Action:** Use `shutil.copyfileobj(in_f, out_f)` to stream the contents of the file directly without reading the entire file contents into memory, reducing memory complexity to O(1) and performing the copy faster since it is optimized at the C level.
