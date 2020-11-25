LAR is a simple archive format to pack multiple lua source files and arbitrary other resources into a single file.


# Format Specification

A LAR archive file is divided into two parts: the payload and the index lookup table.
All segments of the archive are 4 Byte aligned to ease reading and processing of the format.
All integers are stored in network byte order, so an implementation has to use htonl() and htons() to properly read them.

Schema:
	
	<payload:
	  <member:
	    <N*4 bytes: path of file #1>
	    <N*4 bytes: data of file #1>
	  >
	  
	  <member:
	    <N*4 bytes: path of file #2>
	    <N*4 bytes: data of file #2>
	  >
	
	  ...
	
	  <member:
	    <N*4 bytes: path of file #N>
	    <N*4 bytes: data of file #N>
	  >
	>
	
	<index table:
	  <entry:
	    <uint32: offset for path of file #1> <uint32: length for path of file #1>
	    <uint32: offset for data of file #1> <uint32: length for data of file #1>
	    <uint16: type of file #1> <uint16: flags of file #1>
	  >
	
	  <entry:
	    <uint32: offset for path of file #2> <uint32: length for path of file #2>
	    <uint32: offset for data of file #2> <uint32: length for data of file #2>
	    <uint16: type of file #2> <uint16: flags of file #2>
	  >
	
	  ...
	
	  <entry:
	    <uint32: offset for path of file #N> <uint32: length for path of file #N>
	    <uint32: offset for data of file #N> <uint32: length for data of file #N>
	    <uint16: type of file #N> <uint16: flags of file #N>
	  >
	>
	
	<uint32: offset for begin of index table>
	


# Processing

In order to process an LAR archive, an implementation would have to do the following steps:

## Read Index

1. Locate and open the archive file
1. Seek to end of file - 4 bytes
1. Read 32bit index offset and swap from network to native byte order
1. Seek to index offset, calculate index length: filesize - index offset - 4
1. Initialize a linked list for index table entries
1. Read each index entry until the index length is reached, read and byteswap 4 * 32bit int and 2 * 16bit int
1. Seek to begin of file

## Read Member

1. Read the archive index
1. Iterate through the linked index list, perform the following steps for each entry
1. Seek to the specified file path offset
1. Read as much bytes as specified in the file path length into a buffer
1. Compare the contents of the buffer against the path of the searched member
1. If buffer and searched path are equal, seek to the specified file data offset
1. Read data until the file data length is reached, return
1. Select the next index table entry and repeat from step 3, if there is no next entry then return

# Reference implementation

A reference implementation can be found here:
http://luci.subsignal.org/trac/browser/luci/trunk/contrib/lar

The lar.pl script is a simple packer for LAR archives and cli.c provides a utility to list and dump packed LAR archives.
