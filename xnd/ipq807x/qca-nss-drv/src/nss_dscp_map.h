/*
 **************************************************************************
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
 * OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 **************************************************************************
 */

/*
 * nss_dscp_map.h
 *	NSS dscp map parse APIs
 */

#include "nss_tx_rx_common.h"

#define NSS_DSCP_MAP_PARAM_FIELD_COUNT 3
#define NSS_DSCP_MAP_ARRAY_SIZE 64
#define NSS_DSCP_MAP_PRIORITY_MAX NSS_MAX_NUM_PRI

/*
 * nss dscp map entry structure.
 */
struct nss_dscp_map_entry {
	uint8_t action;		/* Action associated with the DSCP value.*/
	uint8_t priority;	/* Priority associated with the DSCP value. */
};

/*
 * nss dscp map parse output.
 */
struct nss_dscp_map_parse {
	uint8_t dscp;		/* Parsed dscp value */
	uint8_t action;		/* Parsed action value */
	uint8_t priority;	/* Parsed priority value */
};

/*
 * nss_dscp_map_print()
 *	Sysctl handler for printing dscp/pri mapping.
 */
static int nss_dscp_map_print(struct ctl_table *ctl, void __user *buffer, size_t *lenp,
				loff_t *ppos, struct nss_dscp_map_entry *mapping)
{
	char *r_buf;
	int i, len;
	size_t cp_bytes = 0;

	/*
	 * (64 * 8) + 22 bytes for the buffer size is sufficient to write
	 * the table including the spaces and new line characters.
	 */
	r_buf = kzalloc(((NSS_DSCP_MAP_ARRAY_SIZE * 8) + 22) * sizeof(char), GFP_KERNEL);
	if (!r_buf) {
		nss_warning("Failed to alloc buffer to print dscp map table\n");
		return -EFAULT;
	}

	/*
	 * Write the priority values to the first line of the output.
	 */
	len = scnprintf(r_buf + cp_bytes, 11, "%s:  ", "priority");
	cp_bytes += len;
	for (i = 0; i < NSS_DSCP_MAP_ARRAY_SIZE; i++) {
		len = scnprintf(r_buf + cp_bytes, 4, "%d ", mapping[i].priority);
		if (!len) {
			nss_warning("failed to read from buffer %d\n", mapping[i].priority);
			kfree(r_buf);
			return -EFAULT;
		}
		cp_bytes += len;
	}

	/*
	 * Add new line character at the end.
	 */
	len = scnprintf(r_buf + cp_bytes, 4, "\n");
	cp_bytes += len;

	/*
	 * Write the action values to the second line of the output.
	 */
	len = scnprintf(r_buf + cp_bytes, 11, "%s:    ", "action");
	cp_bytes += len;
	for (i = 0; i < NSS_DSCP_MAP_ARRAY_SIZE; i++) {
		len = scnprintf(r_buf + cp_bytes, 4, "%d ", mapping[i].action);
		if (!len) {
			nss_warning("failed to read from buffer %d\n", mapping[i].action);
			kfree(r_buf);
			return -EFAULT;
		}
		cp_bytes += len;
	}

	/*
	 * Add new line character at the end.
	 */
	len = scnprintf(r_buf + cp_bytes, 4, "\n");
	cp_bytes += len;

	cp_bytes = simple_read_from_buffer(buffer, *lenp, ppos, r_buf, cp_bytes);
	*lenp = cp_bytes;
	kfree(r_buf);
	return 0;
}

/*
 * nss_dscp_map_parse()
 *	Sysctl handler for dscp/pri mappings.
 */
static int nss_dscp_map_parse(struct ctl_table *ctl, void __user *buffer, size_t *lenp,
				loff_t *ppos, struct nss_dscp_map_parse *out)
{
	int count;
	size_t cp_bytes = 0;
	char w_buf[7];
	loff_t w_offset = 0;
	char *str;
	char *tokens[NSS_DSCP_MAP_PARAM_FIELD_COUNT];
	unsigned int dscp, priority, action;
	int ret;

	/*
	 * Buffer length cannot be more than 7 and less than 6.
	 */
	if (*lenp < 6 || *lenp > 7) {
		nss_warning("Buffer is not correct. Invalid lenght: %d\n", (int)*lenp);
		return -EINVAL;
	}

	/*
	 * It's a write operation
	 */
	cp_bytes = simple_write_to_buffer(w_buf, *lenp, &w_offset, buffer, 7);
	if (cp_bytes != *lenp) {
		nss_warning("failed to write to buffer\n");
		return -EFAULT;
	}

	count = 0;
	str = w_buf;
	tokens[count] = strsep(&str, " ");
	while (tokens[count] != NULL) {
		count++;
		if (count == NSS_DSCP_MAP_PARAM_FIELD_COUNT) {
			nss_warning("maximum allowed field count is %d\n", NSS_DSCP_MAP_PARAM_FIELD_COUNT);
			break;
		}
		tokens[count] = strsep(&str, " ");
	}

	/*
	 * Did we read enough number of parameters from the command line.
	 * There must be 2 parameters.
	 */
	if (count != NSS_DSCP_MAP_PARAM_FIELD_COUNT) {
		nss_warning("param fields are less than expected: %d\n", count);
		return -EINVAL;
	}

	/*
	 * Write the tokens to integers.
	 */
	ret = sscanf(tokens[0], "%u", &dscp);
	if (ret != 1) {
		nss_warning("failed to write the dscp token to integer\n");
		return -EFAULT;
	}

	ret = sscanf(tokens[1], "%u", &action);
	if (ret != 1) {
		nss_warning("failed to write the action token to integer\n");
		return -EFAULT;
	}

	ret = sscanf(tokens[2], "%u", &priority);
	if (ret != 1) {
		nss_warning("failed to write the priority token to integer\n");
		return -EFAULT;
	}

	/*
	 * dscp value cannot be higher than 63.
	 */
	if (dscp >= NSS_DSCP_MAP_ARRAY_SIZE) {
		nss_warning("invalid dscp value: %d\n", dscp);
		return -EINVAL;
	}

	/*
	 * Priority must be less than NSS_DSCP_MAP_PRIORITY_MAX which is 4.
	 */
	if (priority >= NSS_DSCP_MAP_PRIORITY_MAX) {
		nss_warning("invalid priority value: %d\n", priority);
		return -EINVAL;
	}

	nss_info("dscp: %d action: %d priority: %d\n", dscp, action, priority);

	out->dscp = dscp;
	out->action = action;
	out->priority = priority;

	return 0;
}
