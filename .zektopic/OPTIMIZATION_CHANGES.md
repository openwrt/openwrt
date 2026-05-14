# Script Optimizations (Bolt Series)

This document catalogs all script-level optimizations applied in the Zektopic fork.

## 1. belkin-header.py — Streamed I/O

**File:** `scripts/belkin-header.py`

**Change:** Replaced full-file `.read()` (loading entire image into memory — 100+ MB peaks) with chunked/streamed I/O using a configurable buffer size (~12 MB peak).

**Impact:** Memory usage reduced ~8x. Enables processing of large firmware images on memory-constrained build hosts and CI runners.

## 2. cameo-imghdr.py — Struct Packing

**File:** `scripts/cameo-imghdr.py`

**Change:** Replaced individual field assignment with `struct.pack()` for header generation, reducing temporary object allocation and string concatenation.

**Impact:** Reduced allocation overhead and improved throughput for batch image processing.

## 3. netgear-encrypted-factory.py — Chunked Read

**File:** `scripts/netgear-encrypted-factory.py`

**Change:** Replaced full-file read with chunked processing for image encryption/decryption.

**Impact:** Lower peak memory usage, faster processing of large factory images.

## 4. b43-fwsquash.py — Set Operations

**File:** `scripts/b43-fwsquash.py`

**Change:** Replaced linear search for intersection checks with Python set operations.

**Impact:** Reduced time complexity from O(n*m) to O(min(n,m)) for firmware identifier lookups, significantly improving performance on large firmware bundles.

## 5. dl_cleanup.py — Dictionary Grouping

**File:** `scripts/dl_cleanup.py`

**Change:** Used `defaultdict(list)` for grouping download entries instead of manual dictionary initialization with conditional checks.

**Impact:** Cleaner code, reduced boilerplate, and improved maintainability.

## 6. make-index-json.py — String Slicing

**File:** `scripts/make-index-json.py`

**Change:** Used string slicing and `str.join()` instead of repeated string concatenation for JSON index generation.

**Impact:** More efficient string handling for large package indices.

## 7. sercomm-payload.py — CSV Parsing

**File:** `scripts/sercomm-payload.py`

**Change:** Replaced manual string splitting with proper CSV parsing for structured payload data.

**Impact:** Correct handling of edge cases (quoted fields, escaped characters). More maintainable.

## 8. sercomm-kernel-header — CRC32 Chunking

**File:** `scripts/sercomm-kernel-header`

**Change:** Applied chunked CRC32 computation instead of single-pass on entire kernel image.

**Impact:** Reduced peak memory usage for large kernel images.

## 9. cameo-tag.py — Header Generation

**File:** `scripts/cameo-tag.py`

**Change:** Restructured header generation to use direct byte manipulation instead of intermediate string representations.

**Impact:** Faster processing, lower memory overhead.

## 10. crc32-imaging-tag — Binary Header

**File:** `scripts/crc32-imaging-tag`

**Change:** Chunked CRC computation and direct binary header writing.

**Impact:** Reduced memory usage for image tag generation.

## 11. binary header generation scripts

**File:** Various `scripts/*.py`, `scripts/*.sh`

**Change:** Consistent application of chunked I/O pattern across all firmware header generation tools.

**Impact:** Uniform memory-efficient pattern across the entire firmware packaging pipeline.
