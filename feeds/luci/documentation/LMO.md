LMO is a simple binary format to pack language strings into a more efficient form. Although it's suitable to store any kind of key-value table, it's only used for the LuCI *.po based translation system at the moment. The abbreviation "LMO" stands for "Lua Machine Objects" in the style of the GNU gettext *.mo format.


# Format Specification

A LMO file is divided into two parts: the payload and the index lookup table.
All segments of the file are 4 Byte aligned to ease reading and processing of the format.
Only unsigned 32bit integers are used and stored in network byte order, so an implementation has to use htonl() to properly read them.

Schema:
	
	<file:
	  <payload:
	    <entry #1: 4 byte aligned data>
	
	    <entry #2: 4 byte aligned data>
	
	    ...
	
	    <entry #N: 4 byte aligned data>
	  >
	
	  <index table:
	    <entry #1:
	      <uint32_t: hash of the first key>
	      <uint32_t: hash of the first value>
	      <uint32_t: file offset of the first value>
	      <uint32_t: length of the first value>
	    >
	
	    <entry #2:
	      <uint32_t: hash of the second key>
	      <uint32_t: hash of the second value>
	      <uint32_t: file offset of the second value>
	      <uint32_t: length of the second value>
	    >
	
	    ...
	
	    <entry #N:
	      <uint32_t: hash of the Nth key>
	      <uint32_t: hash of the Nth value>
	      <uint32_t: file offset of the Nth value>
	      <uint32_t: length of the Nth value>
	    >
	  >
	
	  <uint32_t: offset of the begin of index table>
	>
	


# Processing

In order to process a LMO file, an implementation would have to do the following steps:

## Read Index

1. Locate and open the archive file
1. Seek to end of file - 4 bytes (sizeof(uint32_t))
1. Read 32bit index offset and swap from network to native byte order
1. Seek to index offset, calculate index length: filesize - index offset - 4
1. Initialize a linked list for index table entries
1. Read each index entry until the index length is reached, read and byteswap 4 * uint32_t for each step
1. Seek to begin of file

## Read Entry

1. Calculate the unsigned 32bit hash of the entries key value (see "Hash Function" section below)
1. Obtain the archive index
1. Iterate through the linked index list, perform the following steps for each entry:
  1. Compare the entry hash value with the calculated hash from step 1
  2. If the hash values are equal proceed with step 4
  3. Select the next entry and repeat from step 3.1
1. Seek to the file offset specified in the selected entry
1. Read as much bytes as specified in the entry length into a buffer
1. Return the buffer value

# Hash Function

The current LuCI-LMO implementation uses the "Super Fast Hash" function which was kindly put in the public domain by it's original author. See http://www.azillionmonkeys.com/qed/hash.html for details. Below is the C-Implementation of this function:

	
	#if (defined(__GNUC__) && defined(__i386__))
	#define sfh_get16(d) (*((const uint16_t *) (d)))
	#else
	#define sfh_get16(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
						   +(uint32_t)(((const uint8_t *)(d))[0]) )
	#endif
	
	uint32_t sfh_hash(const char * data, int len)
	{
		uint32_t hash = len, tmp;
		int rem;
	
		if (len <= NULL) return 0;
	
		rem = len & 3;
		len >>= 2;
	
		/* Main loop */
		for (;len > 0; len--) {
			hash  += sfh_get16(data);
			tmp    = (sfh_get16(data+2) << 11) ^ hash;
			hash   = (hash << 16) ^ tmp;
			data  += 2*sizeof(uint16_t);
			hash  += hash >> 11;
		}
	
		/* Handle end cases */
		switch (rem) {
			case 3: hash += sfh_get16(data);
				hash ^= hash << 16;
				hash ^= data[sizeof(uint16_t)] << 18;
				hash += hash >> 11;
				break;
			case 2: hash += sfh_get16(data);
				hash ^= hash << 11;
				hash += hash >> 17;
				break;
			case 1: hash += *data;
				hash ^= hash << 10;
				hash += hash >> 1;
		}
	
		/* Force "avalanching" of final 127 bits */
		hash ^= hash << 3;
		hash += hash >> 5;
		hash ^= hash << 4;
		hash += hash >> 17;
		hash ^= hash << 25;
		hash += hash >> 6;
	
		return hash;
	}
	

# Reference Implementation

A reference implementation can be found here:
http://luci.subsignal.org/trac/browser/luci/trunk/libs/lmo/src

The lmo_po2lmo.c executable implements a *.po to *.lmo conversation utility and lmo_lookup.c is a simple *.lmo test utility.
Lua bindings for lmo are defined in lmo_lualib.c and associated headers.
