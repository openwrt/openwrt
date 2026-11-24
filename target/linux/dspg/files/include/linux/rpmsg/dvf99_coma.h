/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2026 John Crispin <john@phrozen.org>
 */

#ifndef __LINUX_RPMSG_DVF99_COMA_H
#define __LINUX_RPMSG_DVF99_COMA_H

#include <linux/types.h>

struct device;
struct dvf99_cfifo;
struct dvf99_coma;

struct dvf99_coma_cmsg {
	__le32 type;
	__le32 params_size;
	__le32 payload_size;
	u8 body[];
} __packed;

struct dvf99_coma *devm_dvf99_coma_create(struct device *dev);
int dvf99_coma_prepare(struct dvf99_coma *coma);
void dvf99_coma_fifos_get(struct dvf99_coma *coma, dma_addr_t *l2c,
			  dma_addr_t *c2l);
int dvf99_coma_start(struct dvf99_coma *coma);
void dvf99_coma_stop(struct dvf99_coma *coma, bool crashed);
void dvf99_coma_unprepare(struct dvf99_coma *coma);

struct dvf99_cfifo *dvf99_cfifo_create(struct device *dev, size_t size);
void dvf99_cfifo_destroy(struct device *dev, struct dvf99_cfifo *f);
dma_addr_t dvf99_cfifo_phys(const struct dvf99_cfifo *f);
bool dvf99_cfifo_empty(struct dvf99_cfifo *f);
int dvf99_cfifo_write(struct dvf99_cfifo *f, const void *data, u32 len);
int dvf99_cfifo_read(struct dvf99_cfifo *f, void **msg, u32 *next);
void dvf99_cfifo_consume(struct dvf99_cfifo *f, u32 next);

#endif
