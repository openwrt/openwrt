/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __SOC_BCM3380_MSP_H
#define __SOC_BCM3380_MSP_H

#include <linux/bits.h>
#include <linux/device.h>
#include <linux/types.h>

struct bcm3380_msp;
typedef void (*msp_dqm_irq_callback_t)(void *data, u32 pending_queues);

#define MSP_DQM_Q_AVAIL_MASK	0x00003fff

static inline bool msp_in_msg_not_empty(u32 status)
{
	return status & BIT(31);
}

static inline u32 msp_og_msg_vacancy(u32 status)
{
	return status & 0x1f;
}

int msp_get(struct device *consumer, struct bcm3380_msp **msp);
void msp_put(struct bcm3380_msp *msp);

u32 msp_in_msg_data_bus_addr(struct bcm3380_msp *msp);
void msp_init_messages(struct bcm3380_msp *msp);
u32 msp_in_msg_status(struct bcm3380_msp *msp);
u32 msp_in_msg_read(struct bcm3380_msp *msp);
u32 msp_og_msg_status(struct bcm3380_msp *msp);
u32 msp_4ke_host_mbox_in(struct bcm3380_msp *msp);
u32 msp_4ke_core_status(struct bcm3380_msp *msp);

int msp_dqm_config_queue(struct bcm3380_msp *msp, unsigned int queue,
				 unsigned int token_words,
				 unsigned int mem_words,
				 unsigned int start_words,
				 unsigned int low_water_mark);
void msp_dqm_test_init(struct bcm3380_msp *msp,
			       unsigned int normal_queue,
			       unsigned int high_queue,
			       unsigned int token_words,
			       unsigned int queue_words,
			       unsigned int total_mem_words);
bool msp_dqm_queue_not_empty(struct bcm3380_msp *msp,
				     unsigned int queue);
bool msp_dqm_queue_has_space(struct bcm3380_msp *msp,
				     unsigned int queue);
u32 msp_dqm_not_empty_status(struct bcm3380_msp *msp);
u32 msp_dqm_queue_status(struct bcm3380_msp *msp,
				 unsigned int queue);
u32 msp_dqm_read_word(struct bcm3380_msp *msp, unsigned int queue,
			      unsigned int word);
void msp_dqm_write_word(struct bcm3380_msp *msp, unsigned int queue,
				unsigned int word, u32 value);
int msp_dqm_irq_register(struct bcm3380_msp *msp, u32 queue_mask,
			 msp_dqm_irq_callback_t callback, void *data);
void msp_dqm_irq_unregister(struct bcm3380_msp *msp);
void msp_dqm_irq_rearm(struct bcm3380_msp *msp, u32 queue_mask);

#endif /* __SOC_BCM3380_MSP_H */
