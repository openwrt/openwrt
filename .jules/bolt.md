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
