/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
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
 */

#include "sw.h"
#include "shiva_reduced_acl.h"
#include "hsl.h"

#define SHIVA_RULE_VLU_ADDR  0x58400
#define SHIVA_RULE_MSK_ADDR  0x58c00
#define SHIVA_RULE_LEN_ADDR  0x58818
#define SHIVA_RULE_ACT_ADDR  0x58000
#define SHIVA_RULE_SLCT_ADDR 0x58800

sw_error_t
shiva_acl_rule_write(a_uint32_t dev_id, a_uint32_t rule_idx, a_uint32_t vlu[8],
                     a_uint32_t msk[8])
{
    sw_error_t rv;
    a_uint32_t i, base, addr;

    /* set rule value */
    base = SHIVA_RULE_VLU_ADDR + (rule_idx << 5);
    for (i = 0; i < 5; i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(vlu[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    /* set rule mask */
    base = SHIVA_RULE_MSK_ADDR + (rule_idx << 5);
    for (i = 0; i < 5; i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(msk[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}

sw_error_t
shiva_acl_action_write(a_uint32_t dev_id, a_uint32_t act_idx,
                       a_uint32_t act[3])
{
    sw_error_t rv;
    a_uint32_t base, addr, i;

    /* set rule action */
    base = SHIVA_RULE_ACT_ADDR + (act_idx << 5);
    for (i = 0; i < 3; i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(act[i])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}

sw_error_t
shiva_acl_slct_write(a_uint32_t dev_id, a_uint32_t slct_idx,
                     a_uint32_t slct[8])
{
    sw_error_t rv;
    a_uint32_t base, addr;
    a_uint32_t i;

    base = SHIVA_RULE_SLCT_ADDR + (slct_idx << 5);

    /* set rule address */
    for (i = 1; i < 7; i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_SET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(slct[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    /* set rule enable */
    HSL_REG_ENTRY_GEN_SET(rv, dev_id, base, sizeof (a_uint32_t),
                          (a_uint8_t *) (&(slct[0])),
                          sizeof (a_uint32_t));
    SW_RTN_ON_ERROR(rv);

    return SW_OK;
}

sw_error_t
shiva_acl_rule_read(a_uint32_t dev_id, a_uint32_t rule_idx, a_uint32_t vlu[8],
                    a_uint32_t msk[8])
{
    sw_error_t rv;
    a_uint32_t i, base, addr;

    /* get rule value */
    base = SHIVA_RULE_VLU_ADDR + (rule_idx << 5);
    for (i = 0; i < 5; i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(vlu[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    /* get rule mask */
    base = SHIVA_RULE_MSK_ADDR + (rule_idx << 5);
    for (i = 0; i < 5; i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(msk[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}

sw_error_t
shiva_acl_action_read(a_uint32_t dev_id, a_uint32_t act_idx,
                      a_uint32_t act[3])
{
    sw_error_t rv;
    a_uint32_t base, addr, i;

    /* get rule action */
    base = SHIVA_RULE_ACT_ADDR + (act_idx << 5);
    for (i = 0; i < 3; i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(act[i])), sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}

sw_error_t
shiva_acl_slct_read(a_uint32_t dev_id, a_uint32_t slct_idx,
                    a_uint32_t slct[8])
{
    sw_error_t rv;
    a_uint32_t i, base, addr;

    base = SHIVA_RULE_SLCT_ADDR + (slct_idx << 5);

    /* get filter address and enable */
    for (i = 0; i < 7; i++)
    {
        addr = base + (i << 2);
        HSL_REG_ENTRY_GEN_GET(rv, dev_id, addr, sizeof (a_uint32_t),
                              (a_uint8_t *) (&(slct[i])),
                              sizeof (a_uint32_t));
        SW_RTN_ON_ERROR(rv);
    }

    return SW_OK;
}
