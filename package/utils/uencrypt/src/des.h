/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) 2026 Nick French
 */

/* adapted from des.c from libgcrypt */

typedef struct _des_ctx
  {
    uint32_t encrypt_subkeys[32];
    uint32_t decrypt_subkeys[32];
  }
des_ctx[1];

int des_setkey (struct _des_ctx *, const uint8_t *);
int des_ecb_crypt (struct _des_ctx *, const uint8_t *, uint8_t *, int);
