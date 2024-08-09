// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2023 John Thomson
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/errno.h>

#include "mikrotik_wlan_lz77.h"

#define MIKRO_LZ77 "[mikrotik wlan lz77] "

/* examples have all decompressed to start with DRE\x00 */
#define MIKRO_LZ77_EXPECTED_OUT	(('E' << 16) | ('R' << 8) | ('D'))

/*
 * The maximum number of bits used in a counter.
 * For the look behind window, long instruction match offsets
 * up to 6449 have been seen in provided compressed caldata blobs
 * (that would need 21 counter bits: 4 to 12 + 11 to 0).
 * conservative value here: 27 provides offset up to 0x8000 bytes
 * uses a u8 in this code
 */
#define MIKRO_LZ77_MAX_COUNT_BIT_LEN 27

enum mikrotik_wlan_lz77_instruction {
	INSTR_ERROR = -1,
	INSTR_LITERAL_BYTE = 0,
	/* a (non aligned) byte follows this instruction,
	 * which is directly copied into output
	 */
	INSTR_PREVIOUS_OFFSET = 1,
	/* this group is a match, with a bytes length defined by
	 * following counter bits, starting at bitshift 0,
	 * less the built-in count of 1
	 * using the previous offset as source
	 */
	INSTR_LONG = 2
	/* this group has two counters,
	 * the first counter starts at bitshift 4,
	 *	 if this counter == 0, this is a non-matching group
	 *	 the second counter (bytes length) starts at bitshift 4,
	 *	 less the built-in count of 11+1.
	 *	 The final match group has this count 0,
	 *	 and following bits which pad to byte-alignment.
	 *
	 *	 if this counter > 0, this is a matching group
	 *	 this first count is the match offset (in bytes)
	 *	 the second count is the match length (in bytes),
	 *	 less the built-in count of 2
	 *	 these groups can source bytes that are part of this group
	 */
};

struct lz77_mk_instr_opcodes {
	/* group instruction */
	enum mikrotik_wlan_lz77_instruction instruction;
	/* if >0, a match group,
	 * which starts at byte output_position - 1*offset
	 */
	size_t offset;
	/* how long the match group is,
	 * or how long the (following counter) non-match group is
	 */
	size_t length;
	/* how many bits were used for this instruction + op code(s) */
	size_t bits_used;
	/* input char */
	u8 *in;
	/* offset where this instruction started */
	size_t in_pos;
};

/**
 * mikrotik_wlan_lz77_get_bit
 *
 * @in:			compressed data
 * @in_offset_bit:	bit offset to extract
 */
static u8 mikrotik_wlan_lz77_get_bit(
		const u8 *in,
		size_t in_offset_bit)
{
	return ((in[in_offset_bit>>3] >> (in_offset_bit & 7)) & 0x1);

}

/**
 * mikrotik_wlan_lz77_get_byte
 *
 * @in:			compressed data
 * @in_offset_bit:	bit offset to extract byte
 */
static u8 mikrotik_wlan_lz77_get_byte(
		const u8 *in,
		size_t in_offset_bit)
{
	u8 buf = 0;
	int i;

	/* built a byte from unaligned bits (reversed) */
	for (i = 0; i <= 7; ++i)
		buf += mikrotik_wlan_lz77_get_bit(in, in_offset_bit+i) << (7-i);
	return buf;
}

/**
 * mikrotik_wlan_lz77_decode_count - decode bits at given offset as a count
 *
 * @in:			compressed data
 * @in_offset_bit:	bit offset where count starts
 * @shift:		left shift operand value of first count bit
 * @count:		initial count
 * @bits_used:		how many bits were consumed by this count
 * @max_bits:		maximum bit count for this counter
 *
 * Returns the decoded count
 */
static int mikrotik_wlan_lz77_decode_count(
		const u8 *in,
		size_t in_offset_bit,
		u8 shift,
		size_t count,
		u8 *bits_used,
		u8 max_bits)
{
	size_t pos = in_offset_bit;
	size_t max_pos = pos + max_bits;
	bool up = true;

	*bits_used = 0;
	pr_debug(MIKRO_LZ77 "decode_count inbit: %zu, start shift:%u, initial count:%zu\n",
			in_offset_bit, shift, count);

	while (true) {
		if (pos >= max_pos) {
			pr_err(MIKRO_LZ77 "max bit index reached before count completed\n");
			return -EINVAL;
		}

		/* if the bit value at offset is set */
		if (mikrotik_wlan_lz77_get_bit(in, pos))
			count += (1 << shift);

		/* shift increases until we find an unsed bit */
		else if (up)
			up = false;

		if (up)
			++shift;
		else {
			if (!shift) {
				*bits_used = pos - in_offset_bit + 1;
				return count;
			}
			--shift;
		}

		++pos;
	}

	return -EINVAL;
}

/**
 * mikrotik_wlan_lz77_decode_instruction
 *
 * @in:			compressed data
 * @in_offset_bit:	bit offset where instruction starts
 * @bits_used:		how many bits were consumed by this count
 *
 * Returns the decoded instruction
 */
static enum mikrotik_wlan_lz77_instruction mikrotik_wlan_lz77_decode_instruction(
		const u8 *in,
		size_t in_offset_bit,
		u8 *bits_used)
{
	if (mikrotik_wlan_lz77_get_bit(in, in_offset_bit)) {
		*bits_used = 2;
		if (mikrotik_wlan_lz77_get_bit(in, ++in_offset_bit))
			return INSTR_LONG;
		else
			return INSTR_PREVIOUS_OFFSET;
	} else {
		*bits_used = 1;
		return INSTR_LITERAL_BYTE;
	}
	return INSTR_ERROR;
}

/**
 * mikrotik_wlan_lz77_decode_instruction_operators
 *
 * @in:			compressed data
 * @in_offset_bit:	bit offset where instruction starts
 * @previous_offset:	last used match offset
 * @opcode:		struct to hold instruction & operators
 *
 * Returns error code
 */
static int mikrotik_wlan_lz77_decode_instruction_operators(
		const u8 *in,
		size_t in_offset_bit,
		size_t previous_offset,
		struct lz77_mk_instr_opcodes *opcode)
{
	enum mikrotik_wlan_lz77_instruction instruction;
	u8 bit_count = 0;
	u8 bits_used = 0;
	int offset = 0;
	int length = 0;

	instruction = mikrotik_wlan_lz77_decode_instruction(
			in, in_offset_bit, &bit_count);

	/* skip bits used by instruction */
	bits_used += bit_count;

	switch (instruction) {
	case INSTR_LITERAL_BYTE:
		/* non-matching char */
		offset = 0;
		length = 1;
		break;

	case INSTR_PREVIOUS_OFFSET:
		/* matching group uses previous offset */
		offset = previous_offset;

		length = mikrotik_wlan_lz77_decode_count(
				in,
				in_offset_bit + bits_used,
				0, 1, &bit_count,
				MIKRO_LZ77_MAX_COUNT_BIT_LEN);
		if (length < 0)
			return -EINVAL;
		/* skip bits used by count */
		bits_used += bit_count;
		break;

	case INSTR_LONG:
		offset = mikrotik_wlan_lz77_decode_count(
				in,
				in_offset_bit + bits_used,
				4, 0, &bit_count,
				MIKRO_LZ77_MAX_COUNT_BIT_LEN);
		if (offset < 0)
			return -EINVAL;

		/* skip bits used by offset count */
		bits_used += bit_count;

		if (offset == 0) {
			/* non-matching long group */
			length = mikrotik_wlan_lz77_decode_count(
					in,
					in_offset_bit + bits_used,
					4, 12, &bit_count,
					MIKRO_LZ77_MAX_COUNT_BIT_LEN);
			if (length < 0)
				return -EINVAL;
			/* skip bits used by length count */
			bits_used += bit_count;
		} else {
			/* matching group */
			length = mikrotik_wlan_lz77_decode_count(
					in,
					in_offset_bit + bits_used,
					0, 2, &bit_count,
					MIKRO_LZ77_MAX_COUNT_BIT_LEN);
			if (length < 0)
				return -EINVAL;
			/* skip bits used by length count */
			bits_used += bit_count;
		}

		break;

	case INSTR_ERROR:
		return -EINVAL;
	}

	opcode->instruction = instruction;
	opcode->offset = offset;
	opcode->length = length;
	opcode->bits_used = bits_used;
	opcode->in = (u8 *) in;
	opcode->in_pos = in_offset_bit;
	return 0;
}

/**
 * mikrotik_wlan_lz77_decompress
 *
 * @in:			compressed data ptr
 * @in_len:		length of compressed data
 * @out:		buffer ptr to decompress into
 * @out_len:		length of decompressed buffer in input,
 *			length of decompressed data in success
 *
 * Returns 0 on success, or negative error
 */
int mikrotik_wlan_lz77_decompress(
		const u8 *in,
		const size_t in_len,
		u8 *out,
		size_t *out_len)
{
	u8 *output_ptr;
	size_t input_bit = 0;
	const u8 *output_end = out + *out_len;
	struct lz77_mk_instr_opcodes *opcode;
	size_t match_offset = 0;
	int rc = 0;
	size_t match_length, partial_count, i;

	output_ptr = out;

	if ((in_len * 8) > SIZE_MAX) {
		pr_err(MIKRO_LZ77 "input longer than expected\n");
		return -EINVAL;
	}

	opcode = kmalloc(sizeof(struct lz77_mk_instr_opcodes), GFP_KERNEL);
	if (!opcode)
		return -ENOMEM;

	while (true) {
		if (output_ptr > output_end) {
			pr_err(MIKRO_LZ77 "output overrun\n");
			goto free_lz77_struct;
		}
		if (input_bit > in_len*8) {
			pr_err(MIKRO_LZ77 "input overrun\n");
			goto free_lz77_struct;
		}

		rc = mikrotik_wlan_lz77_decode_instruction_operators(
				in, input_bit, match_offset, opcode);
		if (rc < 0) {
			pr_err(MIKRO_LZ77 "instruction operands decode error\n");
			goto free_lz77_struct;
		}

		pr_debug(MIKRO_LZ77 "inbit:0x%zx->outbyte:0x%zx",
				input_bit, output_ptr - out);

		input_bit += opcode->bits_used;
		switch (opcode->instruction) {
		case INSTR_LITERAL_BYTE:
			pr_debug(" short");
			fallthrough;
		case INSTR_LONG:
			if (opcode->offset == 0) {
				/* this is a non-matching group */
				pr_debug(" non-match, len: 0x%zx\n",
						opcode->length);
				/* test end marker */
				if (opcode->length == 0xc &&
						((input_bit + opcode->length*8) > in_len)) {

					if (*(u32 *)out == MIKRO_LZ77_EXPECTED_OUT) {
						*out_len = output_ptr - out;
						pr_debug(MIKRO_LZ77 "lz77 decompressed from %zu to %zu\n",
								in_len, *out_len);
						return 0;
					} else {
						pr_err(MIKRO_LZ77 "lz77 decompressed but unexpected output first word: 0x%08x\n", *(u32 *)out);
						return -EINVAL;
					}
				}

				for (i = opcode->length; i > 0; --i) {
					*output_ptr = mikrotik_wlan_lz77_get_byte(in, input_bit);
					++output_ptr;
					input_bit += 8;
				}
				break;
			}
			match_offset = opcode->offset;
			fallthrough;
		case INSTR_PREVIOUS_OFFSET:
			match_length = opcode->length;
			partial_count = 0;

			pr_debug(" match, offset: 0x%zx, len: 0x%zx",
					opcode->offset, match_length);

			if (opcode->offset == 0) {
				pr_err(MIKRO_LZ77 "match group missing opcode->offset\n");
				goto free_lz77_struct;
			}

			/* overflow */
			if ((output_ptr + match_length) > output_end) {
				pr_err(MIKRO_LZ77 "match group output overflow\n");
				goto free_lz77_struct;
			}

			/* underflow */
			if ((output_ptr - opcode->offset) < out) {
				pr_err(MIKRO_LZ77 "match group offset underflow\n");
				goto free_lz77_struct;
			}

			while (opcode->offset < match_length) {
				++partial_count;
				memcpy(output_ptr,
						output_ptr - opcode->offset,
						opcode->offset);
				output_ptr += opcode->offset;
				match_length -= opcode->offset;
			}
			memcpy(output_ptr,
					output_ptr - opcode->offset,
					match_length);
			output_ptr += match_length;
			if (partial_count)
				pr_debug(" (%zu partial memcpy)", partial_count);
			pr_debug("\n");

			break;

		case INSTR_ERROR:
			return -EINVAL;
		}
	}

	pr_err(MIKRO_LZ77 "decode loop broken\n");

free_lz77_struct:
	kfree(opcode);

	return -EINVAL;
}
EXPORT_SYMBOL_GPL(mikrotik_wlan_lz77_decompress);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Mikrotik Wi-Fi caldata LZ77 decompressor");
MODULE_AUTHOR("John Thomson");
