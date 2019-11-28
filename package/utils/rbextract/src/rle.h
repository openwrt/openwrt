#ifndef _RLE_H_
#define _RLE_H_

int rle_decode(const unsigned char *src, size_t srclen,
	       unsigned char *dst, size_t dstlen,
	       size_t *src_done, size_t *dst_done);

#endif /* _RLE_H_ */
