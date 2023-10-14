// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2023 John Thomson
 */

#include <linux/slab.h>
#include <linux/string.h>
#include <linux/errno.h>

#include "rb_hardconfig_lz77.h"

#define RB_HC_PR_PFX_LZ77 "[rb_hardconfig][lz77] "


/**
 * lz77_mikrotik_wlan_get_bit
 *
 * @in:			compressed data
 * @in_offset_bit:	bit offset to extract
 */
static unsigned int lz77_mikrotik_wlan_get_bit(
		const unsigned char *in,
		unsigned int in_offset_bit)
{
	return ((in[in_offset_bit>>3] >> (in_offset_bit & 7)) & 0x1);

}


/**
 * lz77_mikrotik_wlan_get_byte
 *
 * @in:			compressed data
 * @in_offset_bit:	bit offset to extract byte
 */
static unsigned char lz77_mikrotik_wlan_get_byte(
		const unsigned char *in,
		unsigned int in_offset_bit)
{
	unsigned char buf = 0;

	/* built a byte from unaligned bits (reversed) */
	for (int i = 0; i <= 7; ++i)
		buf += ((in[(in_offset_bit+i)>>3] >> ((in_offset_bit+i) & 7)) & 0x1)<<(7-i);
	return buf;
}


/**
 * lz77_mikrotik_wlan_decode_count - decode bits at given offset as a count
 *
 * @in:			compressed data
 * @in_offset_bit:	bit offset where count starts
 * @shift:		left shift operand value of first count bit
 * @count:		initial count
 * @bits_used:		how many bits were consumed by this count
 * @size:		maximum bit count for this counter
 *
 * Returns the decoded count
 */
static int lz77_mikrotik_wlan_decode_count(
		const unsigned char *in,
		unsigned int in_offset_bit,
		unsigned int shift,
		unsigned int count,
		unsigned int *bits_used,
		unsigned int size)
{
	unsigned int pos = in_offset_bit;
	bool up = true;

	size += pos;
	*bits_used = 0;
	pr_debug(RB_HC_PR_PFX_LZ77 "decode_count inbit: %i, start shift:%i, initial count:%i\n",
			in_offset_bit, shift, count);

	/* maybe could use find_first_zero_bit to skip count up,
	 * and extract for the count down,
	 * but would need to mask the bits from and to the byte align boundary
	 */

	while (true) {
		if (pos >= size) {
			pr_err(RB_HC_PR_PFX_LZ77 "max bit index reached before count completed\n");
			return -1;
		}

		/* if the bit value at offset is set */
		if (lz77_mikrotik_wlan_get_bit(in, pos)) {
			count += (1 << shift);

		/* shift increases until we find an unsed bit */
		} else if (up) {
			up = false;
		}

		if (up) {
			++shift;
		} else {
			if (!shift) {
				*bits_used = pos - in_offset_bit + 1;
				return count;
			}
			--shift;
		}

		++pos;
	}

	return -1;
}

enum lz77_mikrotik_instruction {
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
	 * 	if this counter == 0, this is a non-matching group
	 * 	the second counter (bytes length) starts at bitshift 4,
	 * 	less the built-in count of 11+1.
	 * 	The final match group has this count 0,
	 * 	and (at least 1) 0 bits pad to byte-align
	 *
	 * 	if this counter > 0, this is a matching group
	 * 	this first count is the match offset (in bytes)
	 * 	the second count is the match length (in bytes),
	 * 	less the built-in count of 2
	 * 	these groups can source bytes that are part of this group
	 */
};

/**
 * lz77_mikrotik_wlan_decode_instruction
 *
 * @in:			compressed data
 * @in_offset_bit:	bit offset where instruction starts
 * @bits_used:		how many bits were consumed by this count
 *
 * Returns the decoded instruction
 */
static enum lz77_mikrotik_instruction lz77_mikrotik_wlan_decode_instruction(
		const unsigned char *in,
		unsigned int in_offset_bit,
		unsigned int *bits_used)
{
	if (lz77_mikrotik_wlan_get_bit(in, in_offset_bit)) {
		*bits_used = 2;
		if (lz77_mikrotik_wlan_get_bit(in, ++in_offset_bit)) {
			return INSTR_LONG;
		} else {
			return INSTR_PREVIOUS_OFFSET;
		}
	} else {
		*bits_used = 1;
		return INSTR_LITERAL_BYTE;
	}
	return INSTR_ERROR;
}

struct lz77_mk_instr_opcodes {
	/* group instruction */
	enum lz77_mikrotik_instruction instruction;
	/* if >0, a match group,
	 * which starts at byte output_position - 1*offset
	 */
	unsigned int offset;
	/* how long the match group is,
	 * or how long the (following counter) non-match group is
	 */
	unsigned int length;
	/* how many bits were used for this instruction + op code(s) */
	unsigned int bits_used;
	/* input char */
	unsigned char *in;
	/* offset where this instruction started */
	unsigned int in_pos;
};

/**
 * lz77_mikrotik_wlan_decode_instruction_operators
 *
 * @in:			compressed data
 * @in_offset_bit:	bit offset where instruction starts
 * @previous_offset:	last used match offset
 * @opcode:		struct to hold instruction & operators
 *
 * Returns error code
 */


static int lz77_mikrotik_wlan_decode_instruction_operators(
		const unsigned char *in,
		unsigned int in_offset_bit,
		unsigned int previous_offset,
		struct lz77_mk_instr_opcodes *opcode)
{
	enum lz77_mikrotik_instruction instruction;
	unsigned int bit_count = 0;
	unsigned int bits_used = 0;
	int offset = 0;
	int length = 0;

	instruction = lz77_mikrotik_wlan_decode_instruction(
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

		length = lz77_mikrotik_wlan_decode_count(
				in,
				in_offset_bit + bits_used,
				0, 1, &bit_count,
				LZ77_MK_MAX_COUNT_BIT_LEN);
		if (length < 0)
			return -1;
		/* skip bits used by count */
		bits_used += bit_count;
		break;

	case INSTR_LONG:
		offset = lz77_mikrotik_wlan_decode_count(
				in,
				in_offset_bit + bits_used,
				4, 0, &bit_count,
				LZ77_MK_MAX_COUNT_BIT_LEN);
		if (offset < 0)
			return -1;

		/* skip bits used by offset count */
		bits_used += bit_count;

		if (offset == 0) {
			/* non-matching long group */
			length = lz77_mikrotik_wlan_decode_count(
					in,
					in_offset_bit + bits_used,
					4, 12, &bit_count,
					LZ77_MK_MAX_COUNT_BIT_LEN);
			if (length < 0)
				return -1;
			/* skip bits used by length count */
			bits_used += bit_count;
		} else {
			/* matching group */
			length = lz77_mikrotik_wlan_decode_count(
					in,
					in_offset_bit + bits_used,
					0, 2, &bit_count,
					LZ77_MK_MAX_COUNT_BIT_LEN);
			if (length < 0)
				return -1;
			/* skip bits used by length count */
			bits_used += bit_count;
		}

		break;

	case INSTR_ERROR:
		return -1;
	}

	opcode->instruction = instruction;
	opcode->offset = offset;
	opcode->length = length;
	opcode->bits_used = bits_used;
	opcode->in = (unsigned char *) in;
	opcode->in_pos = in_offset_bit;
	return 0;
}

/**
 * lz77_mikrotik_wlan_decompress
 *
 * @in:			compressed data ptr
 * @in_len:		length of compressed data
 * @out:		buffer ptr to decompress into
 * @out_len:		length of decompressed buffer
 *
 * Returns length of decompressed data
 */
int lz77_mikrotik_wlan_decompress(
		const unsigned char *in,
		size_t in_len,
		unsigned char *out,
		size_t *out_len)
{
	unsigned char *output_ptr;
	unsigned int input_bit = 0;
	unsigned char * const output_end = out + *out_len;
	struct lz77_mk_instr_opcodes *opcode;
	unsigned int match_offset = 0;
	int rc = 0;
	unsigned int match_length, partial_count;

	output_ptr = out;

	if (in_len > LZ77_MK_MAX_ENCODED ||
			(in_len * 8) > UINT_MAX) {
		pr_err(RB_HC_PR_PFX_LZ77 "input longer than expected\n");
		return -1;
	}

	opcode = kmalloc(sizeof(struct lz77_mk_instr_opcodes), GFP_KERNEL);
	if (!opcode)
		return -ENOMEM;

	while (true) {
		if (output_ptr > output_end) {
			pr_err(RB_HC_PR_PFX_LZ77 "output overrun\n");
			goto free_lz77_struct;
		}
		if (input_bit > in_len*8) {
			pr_err(RB_HC_PR_PFX_LZ77 "input overrun\n");
			goto free_lz77_struct;
		}

		rc = lz77_mikrotik_wlan_decode_instruction_operators(
				in, input_bit, match_offset, opcode);
		if (rc < 0) {
			pr_err(RB_HC_PR_PFX_LZ77 "instruction operands decode error\n");
			goto free_lz77_struct;
		}

		pr_debug(RB_HC_PR_PFX_LZ77 "inbit:0x%x->outbyte:0x%x",
				input_bit, output_ptr - out);

		input_bit += opcode->bits_used;
		switch (opcode->instruction) {
		case INSTR_LITERAL_BYTE:
			pr_debug(" short");
			fallthrough;
		case INSTR_LONG:
			if (opcode->offset == 0) {
				/* this is a non-matching group */
				pr_debug(" non-match, len: 0x%x\n",
						opcode->length);
				/* test end marker */
				if (opcode->length == 0xc &&
						((input_bit + opcode->length*8) > in_len))
				{

					if (!(*(unsigned int *)out == LZ77_MK_EXPECTED_OUT)) {
						pr_debug(RB_HC_PR_PFX_LZ77 "lz77 decompressed from %zu to %d\n",
								in_len, output_ptr - out);
						return (unsigned int)(output_ptr - out);
					} else {
						pr_err(RB_HC_PR_PFX_LZ77 "lz77 decompressed: unexpected output\n");
						return -1;
					}
				}

				for (unsigned int i = opcode->length; i > 0; --i) {
					*output_ptr = lz77_mikrotik_wlan_get_byte(in, input_bit);
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

			pr_debug(" match, offset: 0x%x, len: 0x%x",
					opcode->offset, match_length);

			if (opcode->offset == 0) {
				pr_err(RB_HC_PR_PFX_LZ77 "match group missing opcode->offset\n");
				goto free_lz77_struct;
			}

			/* overflow */
			if ((output_ptr + match_length) > output_end) {
				pr_err(RB_HC_PR_PFX_LZ77 "match group output overflow\n");
				goto free_lz77_struct;
			}

			/* underflow */
			if ((output_ptr - opcode->offset) < out) {
				pr_err(RB_HC_PR_PFX_LZ77 "match group offset underflow\n");
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
			if (partial_count) {
				pr_debug(" (%d partial memcpy)\n", partial_count);
			} else {
				pr_debug("\n");
			}

			break;

		case INSTR_ERROR:
			return -1;
		}
	}

	pr_err(RB_HC_PR_PFX_LZ77 "decode loop broken\n");

free_lz77_struct:
	kfree(opcode);

	return -1;
}
