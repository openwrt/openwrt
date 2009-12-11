/*
    lzma2eva - convert lzma-compressed file to AVM EVA bootloader format
    Copyright (C) 2007  Enrik Berkhan <Enrik.Berkhan@inka.de>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h> /* crc32 */


#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))

#define checksum_add32(csum, data) \
  do { \
    csum += (((data) >> 0)  & 0x000000FF); \
    csum += (((data) >> 8)  & 0x000000FF); \
    csum += (((data) >> 16) & 0x000000FF); \
    csum += (((data) >> 24) & 0x000000FF); \
  } while (0)

void
usage(void)
{
  fprintf(stderr, "usage: lzma2eva <loadadddr> <entry> <lzmafile> <evafile>\n");
  exit(1);
}

void
pexit(const char *msg)
{
  perror(msg);
  exit(1);
}

/* Read an 8bit value */
static int fread_8(uint8_t *buf, FILE *fd)
{
  return (fread(buf, sizeof(*buf), 1, fd) == 1) ? 0 : -1;
}

/* Read a 32bit little endian value and convert to host endianness. */
static int fread_le32(uint32_t *buf, FILE *fd)
{
  size_t count;
  uint8_t tmp[4];
  unsigned int i;

  if (fread(tmp, sizeof(tmp), 1, fd) != 1)
    return -1;
  *buf = 0;
  for (i = 0; i < ARRAY_SIZE(tmp); i++)
    *buf |= (uint32_t)(tmp[i]) << (i * 8);

  return 0;
}

/* Read a 64bit little endian value and convert to host endianness. */
static int fread_le64(uint64_t *buf, FILE *fd)
{
  size_t count;
  uint8_t tmp[8];
  unsigned int i;

  if (fread(tmp, sizeof(tmp), 1, fd) != 1)
    return -1;
  *buf = 0;
  for (i = 0; i < ARRAY_SIZE(tmp); i++)
    *buf |= (uint64_t)(tmp[i]) << (i * 8);

  return 0;
}

/* Write an 8bit value */
static int fwrite_8(uint8_t buf, FILE *fd)
{
  return (fwrite(&buf, sizeof(buf), 1, fd) == 1) ? 0 : -1;
}

/* Convert to little endian and write a 32bit value */
static int fwrite_le32(uint32_t buf, FILE *fd)
{
  size_t count;
  uint8_t tmp[4];
  unsigned int i;

  for (i = 0; i < ARRAY_SIZE(tmp); i++)
    tmp[i] = buf >> (i * 8);
  if (fwrite(tmp, sizeof(tmp), 1, fd) != 1)
    return -1;

  return 0;
}

int
main(int argc, char *argv[])
{
  const char *infile, *outfile;
  FILE *in, *out;
  static const uint8_t buf[4096];
  size_t elems;

  uint8_t properties;
  uint32_t dictsize;
  uint64_t datasize;

  uint32_t magic = 0xfeed1281L;
  uint32_t reclength = 0;
  fpos_t reclengthpos;
  uint32_t loadaddress = 0;
  uint32_t type = 0x075a0201L; /* might be 7Z 2.1? */
  uint32_t checksum = 0;

  uint32_t compsize = 0;
  fpos_t compsizepos;
  uint32_t datasize32 = 0;
  uint32_t datacrc32 = crc32(0, 0, 0);

  uint32_t entry = 0;

  if (argc != 5)
    usage();

  /* "parse" command line */
  loadaddress = strtoul(argv[1], 0, 0);
  entry = strtoul(argv[2], 0, 0);
  infile = argv[3];
  outfile = argv[4];

  in = fopen(infile, "rb");
  if (!in)
    pexit("fopen");
  out = fopen(outfile, "w+b");
  if (!out)
    pexit("fopen");

  /* read LZMA header */
  if (fread_8(&properties, in))
    pexit("fread");
  if (fread_le32(&dictsize, in))
    pexit("fread");
  if (fread_le64(&datasize, in))
    pexit("fread");

  /* write EVA header */
  if (fwrite_le32(magic, out))
    pexit("fwrite");
  if (fgetpos(out, &reclengthpos))
    pexit("fgetpos");
  if (fwrite_le32(reclength, out))
    pexit("fwrite");
  if (fwrite_le32(loadaddress, out))
    pexit("fwrite");
  if (fwrite_le32(type, out))
    pexit("fwrite");

  /* write EVA LZMA header */
  if (fgetpos(out, &compsizepos))
    pexit("fgetpos");
  if (fwrite_le32(compsize, out))
    pexit("fwrite");
  /* XXX check length */
  datasize32 = (uint32_t)datasize;
  if (fwrite_le32(datasize32, out))
    pexit("fwrite");
  if (fwrite_le32(datacrc32, out))
    pexit("fwrite");

  /* write modified LZMA header */
  if (fwrite_8(properties, out))
    pexit("fwrite");
  if (fwrite_le32(dictsize, out))
    pexit("fwrite");
  if (fwrite_le32(0, out))
    pexit("fwrite");

  /* copy compressed data, calculate crc32 */
  while (0 < (elems = fread(&buf, sizeof buf[0], sizeof buf, in))) {
    compsize += elems;
    if (elems != fwrite(&buf, sizeof buf[0], elems, out))
      pexit("fwrite");
    datacrc32 = crc32(datacrc32, buf, elems);
  }
  if (ferror(in))
    pexit("fread");
  fclose(in);

  /* re-write record length */
  reclength = compsize + 24;
  if (fsetpos(out, &reclengthpos))
    pexit("fsetpos");
  if (fwrite_le32(reclength, out))
    pexit("fwrite");

  /* re-write EVA LZMA header including size and data crc */
  if (fsetpos(out, &compsizepos))
    pexit("fsetpos");
  if (fwrite_le32(compsize, out))
    pexit("fwrite");
  if (fwrite_le32(datasize32, out))
    pexit("fwrite");
  if (fwrite_le32(datacrc32, out))
    pexit("fwrite");

  /* calculate record checksum */
  checksum += reclength;
  checksum += loadaddress;
  checksum_add32(checksum, type);
  checksum_add32(checksum, compsize);
  checksum_add32(checksum, datasize32);
  checksum_add32(checksum, datacrc32);
  if (fseek(out, 0, SEEK_CUR))
    pexit("fseek");
  while (0 < (elems = fread(&buf, sizeof buf[0], sizeof buf, out))) {
    size_t i;
    for (i = 0; i < elems; ++i)
      checksum += buf[i];
  }
  if (ferror(out))
    pexit("fread");
  if (fseek(out, 0, SEEK_CUR))
    pexit("fseek");

  checksum = ~checksum + 1;
  if (fwrite_le32(checksum, out))
    pexit("fwrite");

  /* write entry record */
  if (fwrite_le32(0, out))
    pexit("fwrite");
  if (fwrite_le32(entry, out))
    pexit("fwrite");

  if (fclose(out))
    pexit("fclose");

  return 0;
}
