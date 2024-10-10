# ucode library for creating and using read-only hashtable files

This allows fast lookup on files with lots of entries. Read-only hash table
files support json compatible data with objects that can selectively be turned
into hash tables for fast lookup.
These files can be loaded via mmap, to make it easy to share them across
processes and not have to keep contents in RAM at all times.
Any values inside the file are de-duplicated at creation time and referenced
via offsets.

## Create hashtable files

```javascript
let obj = {
	// example data
	hashtbl1: {
		"foo": "bar",
		//...
	},
	hashtbl2: {
		"foo": "bar1",
		//...
	}
};

let uht = require("uht");
uht.mark_hashtable(obj.hashtbl1);
uht.mark_hashtable(obj.hashtbl2);
uht.save("filename.bin", obj);
```

## Use hashtable files

```javascript
// Load File
let uht = require("uht");
let file = uht.open("filename.bin");

// get outer object (not marked as hashtable)
let data = file.get();

// look up value from inner object
let val = file.get(data.hashtbl1, "foo");
warn(`value: ${val}\n`); // returns "value: bar"
```


## API:

### `uht.save(filename, value)`

Saves a ucode value as a uht file. Supports any json compatible datatype
(though most frequently used with an object).
By default, no hashtables are embedded. Any object intended to be used as
hashtable for faster lookup needs to be marked with `uht.mark_hashtable(obj)`
Arbitrary nesting is supported.

### `uht.mark_hashtable(obj)`

Mark an object for hashtable. This adds a key as marker, by default `"##hash": true`.

### `uht.set_hashtable_key(key)`

Set the marker key used to mark objects as hashtables. Must be used before
writing or dumping hashtables.

### `ht = uht.open(filename)`

Open a uht binary file and return a resource.
Returns null on failure.

### `ht.get(val, key, dump)`

When used without arguments, it returns the outermost value of the uht file
(typically an object). If dump is true, embedded hashtables are returned
as ucode objects. This is useful for dumping or rewriting into a new file after
modifications.
val can be used to reference embedded hashtables (obtained by previous `get()`
calls with `dump=false`).
If `key` is given, it performs a hashtable lookup and returns the result.
`val` may only be `null`, if the outer object is itself a hash table.
