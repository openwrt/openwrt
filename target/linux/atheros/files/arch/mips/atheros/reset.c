#include <linux/module.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/kobject.h>
#include <linux/workqueue.h>
#include <linux/skbuff.h>
#include <linux/netlink.h>
#include <net/sock.h>
#include <asm/uaccess.h>
#include <ar531x.h>

#define AR531X_RESET_GPIO_IRQ	(AR531X_GPIO_IRQ(bcfg->resetConfigGpio))

struct event_t {
	struct work_struct wq;
	int set;
	unsigned long jiffies;
};

static struct ar531x_boarddata *bcfg;
static struct timer_list rst_button_timer;

extern struct sock *uevent_sock;
extern u64 uevent_next_seqnum(void);
static unsigned long seen;

static inline void add_msg(struct sk_buff *skb, char *msg)
{
	char *scratch;
	scratch = skb_put(skb, strlen(msg) + 1);
	sprintf(scratch, msg);
}

static void hotplug_button(struct work_struct *wq)
{
	struct sk_buff *skb;
	struct event_t *event;
	size_t len;
	char *scratch, *s;
	char buf[128];

	event = container_of(wq, struct event_t, wq);
	if (!uevent_sock)
		goto done;

	/* allocate message with the maximum possible size */
	s = event->set ? "pressed" : "released";
	len = strlen(s) + 2;
	skb = alloc_skb(len + 2048, GFP_KERNEL);
	if (!skb)
		goto done;

	/* add header */
	scratch = skb_put(skb, len);
	sprintf(scratch, "%s@",s);

	/* copy keys to our continuous event payload buffer */
	add_msg(skb, "HOME=/");
	add_msg(skb, "PATH=/sbin:/bin:/usr/sbin:/usr/bin");
	add_msg(skb, "SUBSYSTEM=button");
	add_msg(skb, "BUTTON=reset");
	add_msg(skb, (event->set ? "ACTION=pressed" : "ACTION=released"));
	sprintf(buf, "SEEN=%ld", (event->jiffies - seen)/HZ);
	add_msg(skb, buf);
	snprintf(buf, 128, "SEQNUM=%llu", uevent_next_seqnum());
	add_msg(skb, buf);

	NETLINK_CB(skb).dst_group = 1;
	netlink_broadcast(uevent_sock, skb, 0, 1, GFP_KERNEL);

done:
	kfree(event);
}

static int no_release_workaround = 1;

static void
reset_button_poll(unsigned long unused)
{
	struct event_t *event;
	int gpio = ~0;

	if(!no_release_workaround)
		return;

	DO_AR5315(gpio = sysRegRead(AR5315_GPIO_DI);)
    gpio &= 1 << (AR531X_RESET_GPIO_IRQ - AR531X_GPIO_IRQ_BASE);
	if(gpio)
	{
		rst_button_timer.expires = jiffies + (HZ / 4);
		add_timer(&rst_button_timer);
	} else {
		event = (struct event_t *) kzalloc(sizeof(struct event_t), GFP_ATOMIC);
		if (!event)
		{
			printk("Could not alloc hotplug event\n");
			return;
		}
		event->set = 0;
		event->jiffies = jiffies;
		INIT_WORK(&event->wq, (void *)(void *)hotplug_button);
		schedule_work(&event->wq);
	}
}

static irqreturn_t button_handler(int irq, void *dev_id)
{
	static int pressed = 0;
	struct event_t *event;
	u32 gpio = ~0;

	event = (struct event_t *) kzalloc(sizeof(struct event_t), GFP_ATOMIC);
	if (!event)
		return IRQ_NONE;

	pressed = !pressed;

	DO_AR5315(gpio = sysRegRead(AR5315_GPIO_DI);)
	gpio &= 1 << (irq - AR531X_GPIO_IRQ_BASE);

	event->set = gpio;
	if(!event->set)
		no_release_workaround = 0;

	event->jiffies = jiffies;

	INIT_WORK(&event->wq, (void *)(void *)hotplug_button);
	schedule_work(&event->wq);

	seen = jiffies;
	if(event->set && no_release_workaround)
		mod_timer(&rst_button_timer, jiffies + (HZ / 4));

	return IRQ_HANDLED;
}

void ar531x_disable_reset_button(void)
{
	disable_irq(AR531X_RESET_GPIO_IRQ);
}

EXPORT_SYMBOL(ar531x_disable_reset_button);

int __init ar531x_init_reset(void)
{
	bcfg = (struct ar531x_boarddata *) board_config;

	seen = jiffies;

	init_timer(&rst_button_timer);
	rst_button_timer.function = reset_button_poll;
	rst_button_timer.expires = jiffies + HZ / 50;
	add_timer(&rst_button_timer);

	request_irq(AR531X_RESET_GPIO_IRQ, &button_handler, IRQF_SAMPLE_RANDOM, "ar531x_reset", NULL);

	return 0;
}



module_init(ar531x_init_reset);
