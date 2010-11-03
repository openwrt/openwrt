#include <linux/io.h>

#include "mps.h"

void mps_fifo_init(struct mps_fifo *fifo, void __iomem *base,
	void __iomem *head_addr, void __iomem *tail_addr, uint32_t size)
{
	fifo->base = base;
	fifo->head_addr = head_addr;
	fifo->tail_addr = tail_addr;
	fifo->size = size;
	mps_fifo_reset(fifo);
}

void mps_fifo_in(struct mps_fifo *fifo, const uint32_t *from, size_t len)
{
	uint32_t head = __raw_readl(fifo->head_addr);
	void __iomem *base = fifo->base + head;
	size_t i = 0;
	size_t byte_len = len * 4;

	if (head < byte_len) {
		for(; i <= head / 4; ++i) {
			__raw_writel(from[i], base);
			base -= 4;
		}

		base += fifo->size;
		head += fifo->size;
	}

	for(; i < len; ++i) {
		__raw_writel(from[i], base);
		base -= 4;
	}

	head -= byte_len;
	__raw_writel(head, fifo->head_addr);

}
EXPORT_SYMBOL_GPL(mps_fifo_in);

void mps_fifo_out(struct mps_fifo *fifo, uint32_t *to, size_t len)
{
	uint32_t tail = __raw_readl(fifo->tail_addr);
	void __iomem *base = fifo->base + tail;
	size_t i = 0;
	size_t byte_len = len * 4;

	if (tail < byte_len) {
		for(; i <= tail / 4; ++i) {
			to[i] = __raw_readl(base);
			base -= 4;
		}

		base += fifo->size;
		tail += fifo->size;
	}

	for(; i < len; ++i) {
		to[i] = __raw_readl(base);
		base -= 4;
	}

	tail -= byte_len;
	__raw_writel(tail, fifo->tail_addr);
}
EXPORT_SYMBOL_GPL(mps_fifo_out);

uint32_t mps_fifo_peek(struct mps_fifo *fifo)
{
	uint32_t tail = __raw_readl(fifo->tail_addr);
	void __iomem *base = fifo->base + tail;
	return __raw_readl(base);
}

void mps_fifo_reset(struct mps_fifo *fifo)
{
	void __iomem *base = fifo->base + fifo->size - 4;
	size_t i;

	__raw_writel(fifo->size - 4, fifo->head_addr);
	__raw_writel(fifo->size - 4, fifo->tail_addr);

	for(i = 0; i < 16; ++i) {
		__raw_writel(0x0, base);
		base -= 4;
	}
}

size_t mps_fifo_len(struct mps_fifo *fifo)
{
	uint32_t head = __raw_readl(fifo->head_addr);
	uint32_t tail = __raw_readl(fifo->tail_addr);

	if (tail < head)
		return head - tail;
	else
		return fifo->size - (tail - head);
}
EXPORT_SYMBOL_GPL(mps_fifo_len);

