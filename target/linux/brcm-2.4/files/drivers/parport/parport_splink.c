/* Low-level parallel port routines for the ASUS WL-500g built-in port
 *
 * Author: Nuno Grilo <nuno.grilo@netcabo.pt>
 * Based on parport_pc source
 */
  
#include <linux/config.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/parport.h>
#include <linux/parport_pc.h>

#define SPLINK_ADDRESS 0xBF800010

#undef DEBUG

#ifdef DEBUG
#define DPRINTK  printk
#else
#define DPRINTK(stuff...)
#endif


/* __parport_splink_frob_control differs from parport_splink_frob_control in that
 * it doesn't do any extra masking. */
static __inline__ unsigned char __parport_splink_frob_control (struct parport *p,
							   unsigned char mask,
							   unsigned char val)
{
	struct parport_pc_private *priv = p->physport->private_data;
	unsigned char *io = (unsigned char *) p->base;
	unsigned char ctr = priv->ctr;
#ifdef DEBUG_PARPORT
	printk (KERN_DEBUG
		"__parport_splink_frob_control(%02x,%02x): %02x -> %02x\n",
		mask, val, ctr, ((ctr & ~mask) ^ val) & priv->ctr_writable);
#endif
	ctr = (ctr & ~mask) ^ val;
	ctr &= priv->ctr_writable; /* only write writable bits. */
	*(io+2) = ctr;
	priv->ctr = ctr;	/* Update soft copy */
	return ctr;
}



static void parport_splink_data_forward (struct parport *p)
{
	DPRINTK(KERN_DEBUG "parport_splink: parport_data_forward called\n");
	__parport_splink_frob_control (p, 0x20, 0);
}

static void parport_splink_data_reverse (struct parport *p)
{
	DPRINTK(KERN_DEBUG "parport_splink: parport_data_forward called\n");
	__parport_splink_frob_control (p, 0x20, 0x20);
}

/*
static void parport_splink_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	DPRINTK(KERN_DEBUG "parport_splink: IRQ handler called\n");
        parport_generic_irq(irq, (struct parport *) dev_id, regs);
}
*/

static void parport_splink_enable_irq(struct parport *p)
{
	DPRINTK(KERN_DEBUG "parport_splink: parport_splink_enable_irq called\n");
	__parport_splink_frob_control (p, 0x10, 0x10);
}

static void parport_splink_disable_irq(struct parport *p)
{
	DPRINTK(KERN_DEBUG "parport_splink: parport_splink_disable_irq called\n");
	__parport_splink_frob_control (p, 0x10, 0);
}

static void parport_splink_init_state(struct pardevice *dev, struct parport_state *s)
{
	DPRINTK(KERN_DEBUG "parport_splink: parport_splink_init_state called\n");
	s->u.pc.ctr = 0xc | (dev->irq_func ? 0x10 : 0x0);
	if (dev->irq_func &&
            dev->port->irq != PARPORT_IRQ_NONE)
                /* Set ackIntEn */
                s->u.pc.ctr |= 0x10;
}

static void parport_splink_save_state(struct parport *p, struct parport_state *s)
{
	const struct parport_pc_private *priv = p->physport->private_data;
	DPRINTK(KERN_DEBUG "parport_splink: parport_splink_save_state called\n");
	s->u.pc.ctr = priv->ctr;
}

static void parport_splink_restore_state(struct parport *p, struct parport_state *s)
{
	struct parport_pc_private *priv = p->physport->private_data;
	unsigned char *io = (unsigned char *) p->base;
	unsigned char ctr = s->u.pc.ctr;

	DPRINTK(KERN_DEBUG "parport_splink: parport_splink_restore_state called\n");
        *(io+2) = ctr;
	priv->ctr = ctr;
}

static void parport_splink_setup_interrupt(void) {
        return;
}

static void parport_splink_write_data(struct parport *p, unsigned char d) {
	DPRINTK(KERN_DEBUG "parport_splink: write data called\n");
        unsigned char *io = (unsigned char *) p->base;
        *io = d;
}

static unsigned char parport_splink_read_data(struct parport *p) {
	DPRINTK(KERN_DEBUG "parport_splink: read data called\n");
        unsigned char *io = (unsigned char *) p->base;
        return *io;
}

static void parport_splink_write_control(struct parport *p, unsigned char d)
{
	const unsigned char wm = (PARPORT_CONTROL_STROBE |
				  PARPORT_CONTROL_AUTOFD |
				  PARPORT_CONTROL_INIT |
				  PARPORT_CONTROL_SELECT);

	DPRINTK(KERN_DEBUG "parport_splink: write control called\n");
	/* Take this out when drivers have adapted to the newer interface. */
	if (d & 0x20) {
		printk (KERN_DEBUG "%s (%s): use data_reverse for this!\n",
			p->name, p->cad->name);
		parport_splink_data_reverse (p);
	}

	__parport_splink_frob_control (p, wm, d & wm);
}

static unsigned char parport_splink_read_control(struct parport *p)
{
	const unsigned char wm = (PARPORT_CONTROL_STROBE |
				  PARPORT_CONTROL_AUTOFD |
				  PARPORT_CONTROL_INIT |
				  PARPORT_CONTROL_SELECT);
	DPRINTK(KERN_DEBUG "parport_splink: read control called\n");
	const struct parport_pc_private *priv = p->physport->private_data;
	return priv->ctr & wm; /* Use soft copy */
}

static unsigned char parport_splink_frob_control (struct parport *p, unsigned char mask,
				       unsigned char val)
{
	const unsigned char wm = (PARPORT_CONTROL_STROBE |
				  PARPORT_CONTROL_AUTOFD |
				  PARPORT_CONTROL_INIT |
				  PARPORT_CONTROL_SELECT);

	DPRINTK(KERN_DEBUG "parport_splink: frob control called\n");
	/* Take this out when drivers have adapted to the newer interface. */
	if (mask & 0x20) {
		printk (KERN_DEBUG "%s (%s): use data_%s for this!\n",
			p->name, p->cad->name,
			(val & 0x20) ? "reverse" : "forward");
		if (val & 0x20)
			parport_splink_data_reverse (p);
		else
			parport_splink_data_forward (p);
	}

	/* Restrict mask and val to control lines. */
	mask &= wm;
	val &= wm;

	return __parport_splink_frob_control (p, mask, val);
}

static unsigned char parport_splink_read_status(struct parport *p)
{
	DPRINTK(KERN_DEBUG "parport_splink: read status called\n");
        unsigned char *io = (unsigned char *) p->base;
        return *(io+1);
}

static void parport_splink_inc_use_count(void)
{
#ifdef MODULE
	MOD_INC_USE_COUNT;
#endif
}

static void parport_splink_dec_use_count(void)
{
#ifdef MODULE
	MOD_DEC_USE_COUNT;
#endif
}

static struct parport_operations parport_splink_ops = 
{
	parport_splink_write_data,
	parport_splink_read_data,

	parport_splink_write_control,
	parport_splink_read_control,
	parport_splink_frob_control,

	parport_splink_read_status,

	parport_splink_enable_irq,
	parport_splink_disable_irq,

	parport_splink_data_forward,
	parport_splink_data_reverse,

	parport_splink_init_state,
	parport_splink_save_state,
	parport_splink_restore_state,

	parport_splink_inc_use_count,
	parport_splink_dec_use_count,

	parport_ieee1284_epp_write_data,
	parport_ieee1284_epp_read_data,
	parport_ieee1284_epp_write_addr,
	parport_ieee1284_epp_read_addr,

	parport_ieee1284_ecp_write_data,
	parport_ieee1284_ecp_read_data,
	parport_ieee1284_ecp_write_addr,

	parport_ieee1284_write_compat,
	parport_ieee1284_read_nibble,
	parport_ieee1284_read_byte,
};

/* --- Initialisation code -------------------------------- */

static struct parport *parport_splink_probe_port (unsigned long int base)
{
	struct parport_pc_private *priv;
	struct parport_operations *ops;
	struct parport *p;

	if (check_mem_region(base, 3)) {
		printk (KERN_DEBUG "parport (0x%lx): iomem region not available\n", base);
		return NULL;
	}
	priv = kmalloc (sizeof (struct parport_pc_private), GFP_KERNEL);
	if (!priv) {
		printk (KERN_DEBUG "parport (0x%lx): no memory!\n", base);
		return NULL;
	}
	ops = kmalloc (sizeof (struct parport_operations), GFP_KERNEL);
	if (!ops) {
		printk (KERN_DEBUG "parport (0x%lx): no memory for ops!\n",
			base);
		kfree (priv);
		return NULL;
	}
	memcpy (ops, &parport_splink_ops, sizeof (struct parport_operations));
	priv->ctr = 0xc;
	priv->ctr_writable = 0xff;

	if (!(p = parport_register_port(base, PARPORT_IRQ_NONE,
					PARPORT_DMA_NONE, ops))) {
		printk (KERN_DEBUG "parport (0x%lx): registration failed!\n",
			base);
		kfree (priv);
		kfree (ops);
		return NULL;
	}

	p->modes = PARPORT_MODE_PCSPP | PARPORT_MODE_SAFEININT;
	p->size = (p->modes & PARPORT_MODE_EPP)?8:3;
	p->private_data = priv;

	parport_proc_register(p);
	request_mem_region (p->base, 3, p->name);

	/* Done probing.  Now put the port into a sensible start-up state. */
	parport_splink_write_data(p, 0);
	parport_splink_data_forward (p);

	/* Now that we've told the sharing engine about the port, and
	   found out its characteristics, let the high-level drivers
	   know about it. */
	parport_announce_port (p);

	DPRINTK(KERN_DEBUG "parport (0x%lx): init ok!\n",
		base);
	return p;
}

static void parport_splink_unregister_port(struct parport *p) {
	struct parport_pc_private *priv = p->private_data;
	struct parport_operations *ops = p->ops;

        if (p->irq != PARPORT_IRQ_NONE)
		free_irq(p->irq, p);
	release_mem_region(p->base, 3);
        parport_proc_unregister(p);
        kfree (priv);
        parport_unregister_port(p);
        kfree (ops);
}


int parport_splink_init(void)
{	
        int ret;
        
	DPRINTK(KERN_DEBUG "parport_splink init called\n");
        parport_splink_setup_interrupt();
        ret = !parport_splink_probe_port(SPLINK_ADDRESS);
        
        return ret;
}

void parport_splink_cleanup(void) {
        struct parport *p = parport_enumerate(), *tmp;
	DPRINTK(KERN_DEBUG "parport_splink cleanup called\n");
        if (p->size) {
                if (p->modes & PARPORT_MODE_PCSPP) {
                        while(p) {
                                tmp = p->next;
                                parport_splink_unregister_port(p);
                                p = tmp;
                        }
                }
        }
}

MODULE_AUTHOR("Nuno Grilo <nuno.grilo@netcabo.pt>");
MODULE_DESCRIPTION("Parport Driver for ASUS WL-500g router builtin Port");
MODULE_SUPPORTED_DEVICE("ASUS WL-500g builtin Parallel Port");
MODULE_LICENSE("GPL");

module_init(parport_splink_init)
module_exit(parport_splink_cleanup)

