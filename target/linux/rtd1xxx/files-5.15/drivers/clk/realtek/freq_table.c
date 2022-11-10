#include "common.h"

const struct freq_table *ftbl_find_by_rate(const struct freq_table *ftbl,
					   unsigned long rate)
{
	unsigned long best_rate = 0;
	const struct freq_table *best = NULL;

	for ( ; !IS_FREQ_TABLE_END(ftbl); ftbl++) {
		if (ftbl->rate == rate)
			return ftbl;

		if (ftbl->rate > rate)
			continue;

		if ((rate - best_rate) > (rate - ftbl->rate)) {
			best_rate = ftbl->rate;
			best = ftbl;
		}
	}

	return best;
}

const struct freq_table *ftbl_find_by_val_with_mask(const struct freq_table *ftbl,
						    uint32_t mask, uint32_t value)
{
	while (!IS_FREQ_TABLE_END(ftbl)) {
		if ((ftbl->val & mask) == (value & mask))
			return ftbl;
		ftbl++;
	}
	return NULL;
};

const struct div_table *dtbl_find_by_rate(const struct div_table *dtbl, unsigned long rate)
{
	while (!IS_DIV_TABLE_END(dtbl)) {
		if (rate >= dtbl->rate)
			return dtbl;
		dtbl++;
	}
	return NULL;
}

const struct div_table *dtbl_find_by_val(const struct div_table *dtbl, uint32_t val)
{
	while (!IS_DIV_TABLE_END(dtbl)) {
		if (val == dtbl->val)
			return dtbl;
		dtbl++;
	}
	return NULL;
}
