/*
 ***************************************************************************
 * Copyright (c) 2020-2021, The Linux Foundation. All rights reserved.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ***************************************************************************
 */

#ifndef __NSS_MATCH_STATS_H__
#define __NSS_MATCH_STATS_H__

/**
 * nss_match_stats_types
 *	Match statistics types.
 */
enum nss_match_stats_types {
	NSS_MATCH_STATS_HIT_COUNT_0 = NSS_STATS_NODE_MAX,
					/**< Hit count of rule ID 1. */
	NSS_MATCH_STATS_HIT_COUNT_1,	/**< Hit count of rule ID 2. */
	NSS_MATCH_STATS_HIT_COUNT_2,	/**< Hit count of rule ID 3. */
	NSS_MATCH_STATS_HIT_COUNT_3,	/**< Hit count of rule ID 4. */
	NSS_MATCH_STATS_HIT_COUNT_4,	/**< Hit count of rule ID 5. */
	NSS_MATCH_STATS_HIT_COUNT_5,	/**< Hit count of rule ID 6. */
	NSS_MATCH_STATS_HIT_COUNT_6,	/**< Hit count of rule ID 7. */
	NSS_MATCH_STATS_HIT_COUNT_7,	/**< Hit count of rule ID 8. */
	NSS_MATCH_STATS_HIT_COUNT_8,	/**< Hit count of rule ID 9. */
	NSS_MATCH_STATS_HIT_COUNT_9,	/**< Hit count of rule ID 10. */
	NSS_MATCH_STATS_HIT_COUNT_10,	/**< Hit count of rule ID 11. */
	NSS_MATCH_STATS_HIT_COUNT_11,	/**< Hit count of rule ID 12. */
	NSS_MATCH_STATS_HIT_COUNT_12,	/**< Hit count of rule ID 13. */
	NSS_MATCH_STATS_HIT_COUNT_13,	/**< Hit count of rule ID 14. */
	NSS_MATCH_STATS_HIT_COUNT_14,	/**< Hit count of rule ID 15. */
	NSS_MATCH_STATS_HIT_COUNT_15,	/**< Hit count of rule ID 16. */
	NSS_MATCH_STATS_HIT_COUNT_16,	/**< Hit count of rule ID 17. */
	NSS_MATCH_STATS_HIT_COUNT_17,	/**< Hit count of rule ID 18. */
	NSS_MATCH_STATS_HIT_COUNT_18,	/**< Hit count of rule ID 19. */
	NSS_MATCH_STATS_HIT_COUNT_19,	/**< Hit count of rule ID 20. */
	NSS_MATCH_STATS_HIT_COUNT_20,	/**< Hit count of rule ID 21. */
	NSS_MATCH_STATS_HIT_COUNT_21,	/**< Hit count of rule ID 22. */
	NSS_MATCH_STATS_HIT_COUNT_22,	/**< Hit count of rule ID 23. */
	NSS_MATCH_STATS_HIT_COUNT_23,	/**< Hit count of rule ID 24. */
	NSS_MATCH_STATS_HIT_COUNT_24,	/**< Hit count of rule ID 25. */
	NSS_MATCH_STATS_HIT_COUNT_25,	/**< Hit count of rule ID 26. */
	NSS_MATCH_STATS_HIT_COUNT_26,	/**< Hit count of rule ID 27. */
	NSS_MATCH_STATS_HIT_COUNT_27,	/**< Hit count of rule ID 28. */
	NSS_MATCH_STATS_HIT_COUNT_28,	/**< Hit count of rule ID 29. */
	NSS_MATCH_STATS_HIT_COUNT_29,	/**< Hit count of rule ID 30. */
	NSS_MATCH_STATS_HIT_COUNT_30,	/**< Hit count of rule ID 31. */
	NSS_MATCH_STATS_HIT_COUNT_31,	/**< Hit count of rule ID 32. */
	NSS_MATCH_STATS_MAX,		/**< Maximum statistics type. */
};

/**
 * nss_match_stats_notification
 *	Match transmission statistics structure.
 */
struct nss_match_stats_notification {
	uint64_t stats_ctx[NSS_MATCH_STATS_MAX];	/**< Context transmission statistics. */
	uint32_t core_id;				/**< Core ID. */
	uint32_t if_num;				/**< Interface number. */
};

extern bool nss_match_ifnum_add(int if_num);
extern bool nss_match_ifnum_delete(int if_num);
extern void nss_match_stats_notify(struct nss_ctx_instance *nss_ctx, uint32_t if_num);
extern void nss_match_stats_sync(struct nss_ctx_instance *nss_ctx, struct nss_match_msg *nmm);
extern void nss_match_stats_dentry_create(void);
extern int nss_match_stats_unregister_notifier(struct notifier_block *nb);
extern int nss_match_stats_register_notifier(struct notifier_block *nb);

#endif /* __NSS_MATCH_STATS_H__ */
