/* SPDX-License-Identifier: GPL-2.0-or-later
 * Copyright (C) 2026 Nick French
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/if_alg.h>
#include <errno.h>

#include "uencrypt.h"

struct cipher {
    const char *name;         // Cipher name in format used by openssl,etc
    const char *kernel_name;  // Cipher name that the kernel uses
    int iv_size;
    int key_size;
    int block_size;
};

static const struct cipher ciphers[] = {
    { "des-ecb", "ecb(des)", 0, 8, 8 },
};

struct context {
    int tfmfd; // Cipher socket descriptor
    int opfd;  // Connection descriptor for streams
    int enc;   // 1 for encrypt, 0 for decrypt
    const struct cipher* cipher;
};

const cipher_t *get_default_cipher(void)
{
    return (const cipher_t *)&ciphers[0]; //des-ecb
}

const cipher_t *get_cipher_or_print_error(char *name)
{
    const struct cipher *cipher = NULL;
    for (size_t i = 0; i < sizeof(ciphers) / sizeof(ciphers[0]); i++) {
        if (strcasecmp(name, ciphers[i].name) == 0) {
            cipher = &ciphers[i];
            break;
        }
    }

    if (!cipher) {
        fprintf(stderr, "Cipher '%s' is unsupported.\n", name);
        fprintf(stderr, "Available ciphers: \n");
        for (size_t i = 0; i < sizeof(ciphers) / sizeof(ciphers[0]); i++) {
            fprintf(stderr, "  %s\n", ciphers[i].name);
        }
        return NULL;
    }

    return (const cipher_t*)cipher;
}

int get_cipher_ivsize(const cipher_t *cipher)
{
    return ((const struct cipher *)cipher)->iv_size;
}

int get_cipher_keysize(const cipher_t *cipher)
{
    return ((const struct cipher *)cipher)->key_size;
}

ctx_t *create_ctx(const cipher_t *cipher_in, const unsigned char *key,
                  const unsigned char *iv, int enc, int padding) {

    const struct cipher *cipher = (const struct cipher *)cipher_in;

    struct sockaddr_alg sa = {
        .salg_family = AF_ALG,
        .salg_type = "skcipher"
    };

    strncpy((char *)sa.salg_name, cipher->kernel_name, sizeof(sa.salg_name) - 1);

    struct context *ctx = calloc(1, sizeof(*ctx));
    if (!ctx) {
        fprintf(stderr, "Could not allocate memory for context\n");
        return NULL;
    }

    ctx->cipher = cipher;
    ctx->enc = enc;

    // Create algorithmic transformation socket
    ctx->tfmfd = socket(AF_ALG, SOCK_SEQPACKET, 0);
    if (ctx->tfmfd < 0) {
        fprintf(stderr, "Socket creation failed: %s (errno: %d)\n", strerror(errno), errno);
        fprintf(stderr, "Is 'kmod-crypto-user' installed?\n");
        goto err_free;
    }

    if (bind(ctx->tfmfd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        fprintf(stderr, "bind failed. Cipher '%s' module not loaded?\n", cipher->kernel_name);
        goto err_close_tfm;
    }

    if (setsockopt(ctx->tfmfd, SOL_ALG, ALG_SET_KEY, key, cipher->key_size) < 0) {
        fprintf(stderr, "setsockopt failed to set key\n");
        goto err_close_tfm;
    }

    // Accept operational connection to bind stream configurations
    ctx->opfd = accept(ctx->tfmfd, NULL, 0);
    if (ctx->opfd < 0) {
        fprintf(stderr, "accept failed: %s (errno: %d)\n", strerror(errno), errno);
        goto err_close_tfm;
    }

    return ctx;

err_close_tfm:
    close(ctx->tfmfd);
err_free:
    free(ctx);
    return NULL;
}

int do_crypt(FILE *infile, FILE *outfile, ctx_t *ctx_in)
{
    struct context* ctx = (struct context*)ctx_in;

    unsigned char input_buf[CRYPT_BUF_SIZE];
    unsigned char output_buf[CRYPT_BUF_SIZE];
    size_t bytes_read;
    int ret = 0;
    int block_size = ctx->cipher->block_size;

    while ((bytes_read = fread(input_buf, 1, sizeof(input_buf), infile)) > 0) {
        // Enforce basic padding constraints if working on unaligned trailing stream chunks
        if ((bytes_read % block_size) != 0) {
            size_t padded_len = ((bytes_read + block_size - 1) / block_size) * block_size;
            if (padded_len > sizeof(input_buf)) {
                fprintf(stderr, "Error calculating padding\n");
                return EINVAL;
            }
            memset(input_buf + bytes_read, 0, padded_len - bytes_read);
            bytes_read = padded_len;
        }

        // Setup the crypto request
        unsigned char cmsg_buf[CMSG_SPACE(sizeof(__u32))];
        memset(cmsg_buf, 0, sizeof(cmsg_buf));

        struct msghdr msg = {0};
        msg.msg_control = cmsg_buf;
        msg.msg_controllen = sizeof(cmsg_buf);

        struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_ALG;
        cmsg->cmsg_type = ALG_SET_OP;
        cmsg->cmsg_len = CMSG_LEN(sizeof(__u32));
        __u32 op = ctx->enc ? ALG_OP_ENCRYPT : ALG_OP_DECRYPT;
        memcpy(CMSG_DATA(cmsg), &op, sizeof(__u32));

        struct iovec iov = {
            .iov_base = input_buf,
            .iov_len = bytes_read
        };
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        // Send the crypto request to kernel
        if (sendmsg(ctx->opfd, &msg, 0) < 0) {
            fprintf(stderr, "sendmsg failed: %s (errno: %d)\n", strerror(errno), errno);
            return errno;
        }

        // Collect processed stream back out
        if (read(ctx->opfd, output_buf, bytes_read) < 0) {
            fprintf(stderr, "Error calling sendmsg\n");
            return errno;
        }

        // Write processed stream to output
        if (fwrite(output_buf, 1, bytes_read, outfile) != bytes_read) {
            fprintf(stderr, "Error writing buffer\n");
            return EIO;
        }
    }

    return ret;
}

void free_ctx(ctx_t *ctx_in)
{
    struct context* ctx = (struct context*)ctx_in;
    if (!ctx)
        return;

    close(ctx->opfd);
    close(ctx->tfmfd);
    free(ctx);
}


