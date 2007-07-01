/*

  Broadcom BCM43xx wireless driver

  debugfs driver debugging code

  Copyright (c) 2005 Michael Buesch <mb@bu3sch.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; see the file COPYING.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Steet, Fifth Floor,
  Boston, MA 02110-1301, USA.

*/



#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/pci.h>
#include <linux/mutex.h>

#include "bcm43xx.h"
#include "bcm43xx_main.h"
#include "bcm43xx_debugfs.h"
#include "bcm43xx_dma.h"
#include "bcm43xx_pio.h"
#include "bcm43xx_xmit.h"

#define REALLY_BIG_BUFFER_SIZE	(1024*256)

static struct bcm43xx_debugfs fs;
static char big_buffer[1024*256];
static DEFINE_MUTEX(big_buffer_mutex);


static ssize_t write_file_dummy(struct file *file, const char __user *buf,
				size_t count, loff_t *ppos)
{
	return count;
}

static int open_file_generic(struct inode *inode, struct file *file)
{
	file->private_data = inode->i_private;
	return 0;
}

#define fappend(fmt, x...)	pos += snprintf(buf + pos, len - pos, fmt , ##x)

static ssize_t drvinfo_read_file(struct file *file, char __user *userbuf,
				 size_t count, loff_t *ppos)
{
	const size_t len = ARRAY_SIZE(big_buffer);
	char *buf = big_buffer;
	size_t pos = 0;
	ssize_t res;

	mutex_lock(&big_buffer_mutex);
	/* This is where the information is written to the "driver" file */
	fappend(KBUILD_MODNAME " driver\n");
	fappend("Compiled at: %s %s\n", __DATE__, __TIME__);
	res = simple_read_from_buffer(userbuf, count, ppos, buf, pos);
	mutex_unlock(&big_buffer_mutex);

	return res;
}

static ssize_t tsf_read_file(struct file *file, char __user *userbuf,
			     size_t count, loff_t *ppos)
{
	struct bcm43xx_wldev *dev = file->private_data;
	const size_t len = ARRAY_SIZE(big_buffer);
	char *buf = big_buffer;
	size_t pos = 0;
	ssize_t res;
	unsigned long flags;
	u64 tsf;

	mutex_lock(&big_buffer_mutex);
	mutex_lock(&dev->wl->mutex);
	spin_lock_irqsave(&dev->wl->irq_lock, flags);
	if (bcm43xx_status(dev) != BCM43xx_STAT_INITIALIZED) {
		fappend("Board not initialized.\n");
		goto out;
	}
	bcm43xx_tsf_read(dev, &tsf);
	fappend("0x%08x%08x\n",
		(unsigned int)((tsf & 0xFFFFFFFF00000000ULL) >> 32),
		(unsigned int)(tsf & 0xFFFFFFFFULL));

out:
	spin_unlock_irqrestore(&dev->wl->irq_lock, flags);
	mutex_unlock(&dev->wl->mutex);
	res = simple_read_from_buffer(userbuf, count, ppos, buf, pos);
	mutex_unlock(&big_buffer_mutex);

	return res;
}

static ssize_t tsf_write_file(struct file *file, const char __user *user_buf,
			      size_t count, loff_t *ppos)
{
	struct bcm43xx_wldev *dev = file->private_data;
	char *buf = big_buffer;
	ssize_t buf_size;
	ssize_t res;
	unsigned long flags;
	u64 tsf;

	mutex_lock(&big_buffer_mutex);
	buf_size = min(count, ARRAY_SIZE(big_buffer) - 1);
	if (copy_from_user(buf, user_buf, buf_size)) {
	        res = -EFAULT;
		goto out_unlock_bb;
	}
	mutex_lock(&dev->wl->mutex);
	spin_lock_irqsave(&dev->wl->irq_lock, flags);
	if (bcm43xx_status(dev) != BCM43xx_STAT_INITIALIZED) {
		printk(KERN_INFO PFX "debugfs: Board not initialized.\n");
		res = -EFAULT;
		goto out_unlock;
	}
	if (sscanf(buf, "%llu", (unsigned long long *)(&tsf)) != 1) {
		printk(KERN_INFO PFX "debugfs: invalid values for \"tsf\"\n");
		res = -EINVAL;
		goto out_unlock;
	}
	bcm43xx_tsf_write(dev, tsf);
	mmiowb();
	res = buf_size;

out_unlock:
	spin_unlock_irqrestore(&dev->wl->irq_lock, flags);
	mutex_unlock(&dev->wl->mutex);
out_unlock_bb:
	mutex_unlock(&big_buffer_mutex);

	return res;
}

static ssize_t txstat_read_file(struct file *file, char __user *userbuf,
				size_t count, loff_t *ppos)
{
	struct bcm43xx_wldev *dev = file->private_data;
	struct bcm43xx_dfsentry *e = dev->dfsentry;
	struct bcm43xx_txstatus_log *log = &e->txstatlog;
	unsigned long flags;
	char *buf = log->printbuf;
	const size_t len = ARRAY_SIZE(log->printbuf);
	size_t pos = 0;
	ssize_t res;
	int i, idx;
	struct bcm43xx_txstatus *stat;

	mutex_lock(&big_buffer_mutex);
	spin_lock_irqsave(&log->lock, flags);
	if (!log->printing) {
		log->printing = 1;
		fappend("bcm43xx TX status reports:\n\n"
			"index | cookie | seq | phy_stat | frame_count | "
			"rts_count | supp_reason | pm_indicated | "
			"intermediate | for_ampdu | acked\n"
			"---\n");
		i = log->end + 1;
		idx = 0;
		while (1) {
			if (i == BCM43xx_NR_LOGGED_TXSTATUS)
				i = 0;
			stat = &(log->log[i]);
			if (stat->cookie) {
				fappend("%03d | "
					"0x%04X | 0x%04X | 0x%02X | "
					"0x%X | 0x%X | "
					"%u | %u | "
					"%u | %u | %u\n",
					idx,
					stat->cookie, stat->seq, stat->phy_stat,
					stat->frame_count, stat->rts_count,
					stat->supp_reason, stat->pm_indicated,
					stat->intermediate, stat->for_ampdu,
					stat->acked);
				idx++;
			}
			if (i == log->end)
				break;
			i++;
		}
		log->buf_avail = pos;
	}
	memcpy(big_buffer, buf,
	       min(log->buf_avail, ARRAY_SIZE(big_buffer)));
	spin_unlock_irqrestore(&log->lock, flags);

	res = simple_read_from_buffer(userbuf, count, ppos,
				      big_buffer,
				      log->buf_avail);
	if (*ppos == log->buf_avail) {
		spin_lock_irqsave(&log->lock, flags);
		log->printing = 0;
		spin_unlock_irqrestore(&log->lock, flags);
	}
	mutex_unlock(&big_buffer_mutex);

	return res;
}

static ssize_t restart_write_file(struct file *file, const char __user *user_buf,
				  size_t count, loff_t *ppos)
{
	struct bcm43xx_wldev *dev = file->private_data;
	char *buf = big_buffer;
	ssize_t buf_size;
	ssize_t res;
	unsigned long flags;

	mutex_lock(&big_buffer_mutex);
	buf_size = min(count, ARRAY_SIZE(big_buffer) - 1);
	if (copy_from_user(buf, user_buf, buf_size)) {
	        res = -EFAULT;
		goto out_unlock_bb;
	}
	mutex_lock(&dev->wl->mutex);
	spin_lock_irqsave(&dev->wl->irq_lock, flags);
	if (bcm43xx_status(dev) != BCM43xx_STAT_INITIALIZED) {
		printk(KERN_INFO PFX "debugfs: Board not initialized.\n");
		res = -EFAULT;
		goto out_unlock;
	}
	if (count > 0 && buf[0] == '1') {
		bcm43xx_controller_restart(dev, "manually restarted");
		res = count;
	} else
		res = -EINVAL;

out_unlock:
	spin_unlock_irqrestore(&dev->wl->irq_lock, flags);
	mutex_unlock(&dev->wl->mutex);
out_unlock_bb:
	mutex_unlock(&big_buffer_mutex);

	return res;
}

static ssize_t txpower_g_read_file(struct file *file, char __user *userbuf,
				   size_t count, loff_t *ppos)
{
	struct bcm43xx_wldev *dev = file->private_data;
	const size_t len = ARRAY_SIZE(big_buffer);
	char *buf = big_buffer;
	size_t pos = 0;
	ssize_t res;
	unsigned long flags;

	mutex_lock(&big_buffer_mutex);
	mutex_lock(&dev->wl->mutex);
	spin_lock_irqsave(&dev->wl->irq_lock, flags);
	if ((bcm43xx_status(dev) != BCM43xx_STAT_INITIALIZED) ||
	    !dev->started) {
		fappend("Not initialized\n");
		goto out;
	}
	if (dev->phy.type != BCM43xx_PHYTYPE_G) {
		fappend("Device is not a G-PHY\n");
		goto out;
	}
	fappend("Control:               %s\n", dev->phy.manual_txpower_control ?
					       "MANUAL" : "AUTOMATIC");
	fappend("Baseband attenuation:  %u\n", dev->phy.bbatt.att);
	fappend("Radio attenuation:     %u\n", dev->phy.rfatt.att);
	fappend("TX Mixer Gain:         %s\n", (dev->phy.tx_control & BCM43xx_TXCTL_TXMIX) ?
					       "ON" : "OFF");
	fappend("PA Gain 2dB:           %s\n", (dev->phy.tx_control & BCM43xx_TXCTL_PA2DB) ?
					       "ON" : "OFF");
	fappend("PA Gain 3dB:           %s\n", (dev->phy.tx_control & BCM43xx_TXCTL_PA3DB) ?
					       "ON" : "OFF");
	fappend("\n\n");
	fappend("You can write to this file:\n");
	fappend("Writing \"auto\" enables automatic txpower control.\n");
	fappend("Writing the attenuation values as \"bbatt rfatt txmix pa2db pa3db\" "
		"enables manual txpower control.\n");
	fappend("Example: 5 4 0 0 1\n");
	fappend("Enables manual control with Baseband attenuation 5, "
		"Radio attenuation 4, No TX Mixer Gain, "
		"No PA Gain 2dB, With PA Gain 3dB.\n");

out:
	spin_unlock_irqrestore(&dev->wl->irq_lock, flags);
	mutex_unlock(&dev->wl->mutex);
	res = simple_read_from_buffer(userbuf, count, ppos, buf, pos);
	mutex_unlock(&big_buffer_mutex);

	return res;
}

static ssize_t txpower_g_write_file(struct file *file, const char __user *user_buf,
				    size_t count, loff_t *ppos)
{
	struct bcm43xx_wldev *dev = file->private_data;
	char *buf = big_buffer;
	ssize_t buf_size;
	ssize_t res;
	unsigned long flags, phy_flags;

	mutex_lock(&big_buffer_mutex);
	buf_size = min(count, ARRAY_SIZE(big_buffer) - 1);
	if (copy_from_user(buf, user_buf, buf_size)) {
	        res = -EFAULT;
		goto out_unlock_bb;
	}
	mutex_lock(&dev->wl->mutex);
	spin_lock_irqsave(&dev->wl->irq_lock, flags);
	if ((bcm43xx_status(dev) != BCM43xx_STAT_INITIALIZED) ||
	    !dev->started) {
		printk(KERN_INFO PFX "debugfs: Board not initialized.\n");
		res = -ENODEV;
		goto out_unlock;
	}
	if (dev->phy.type != BCM43xx_PHYTYPE_G) {
		printk(KERN_ERR PFX "debugfs: Device is not a G-PHY\n");
		res = -ENODEV;
		goto out_unlock;
	}
	if ((buf_size >= 4) && (memcmp(buf, "auto", 4) == 0)) {
		/* Automatic control */
		dev->phy.manual_txpower_control = 0;
		bcm43xx_phy_xmitpower(dev);
	} else {
		int bbatt = 0, rfatt = 0, txmix = 0, pa2db = 0, pa3db = 0;
		/* Manual control */
		if (sscanf(buf, "%d %d %d %d %d", &bbatt, &rfatt,
			   &txmix, &pa2db, &pa3db) != 5) {
			printk(KERN_INFO PFX "debugfs: invalid value for \"tx_power_g\"\n");
			res = -EINVAL;
			goto out_unlock;
		}
		bcm43xx_put_attenuation_into_ranges(dev, &bbatt, &rfatt);
		dev->phy.manual_txpower_control = 1;
		dev->phy.bbatt.att = bbatt;
		dev->phy.rfatt.att = rfatt;
		dev->phy.tx_control = 0;
		if (txmix)
			dev->phy.tx_control |= BCM43xx_TXCTL_TXMIX;
		if (pa2db)
			dev->phy.tx_control |= BCM43xx_TXCTL_PA2DB;
		if (pa3db)
			dev->phy.tx_control |= BCM43xx_TXCTL_PA3DB;
		bcm43xx_phy_lock(dev, phy_flags);
		bcm43xx_radio_lock(dev);
		bcm43xx_set_txpower_g(dev, &dev->phy.bbatt,
				      &dev->phy.rfatt, dev->phy.tx_control);
		bcm43xx_radio_unlock(dev);
		bcm43xx_phy_unlock(dev, phy_flags);
	}
	res = buf_size;
out_unlock:
	spin_unlock_irqrestore(&dev->wl->irq_lock, flags);
	mutex_unlock(&dev->wl->mutex);
out_unlock_bb:
	mutex_unlock(&big_buffer_mutex);

	return res;
}


#undef fappend


static struct file_operations drvinfo_fops = {
	.read = drvinfo_read_file,
	.write = write_file_dummy,
	.open = open_file_generic,
};

static struct file_operations tsf_fops = {
	.read = tsf_read_file,
	.write = tsf_write_file,
	.open = open_file_generic,
};

static struct file_operations txstat_fops = {
	.read = txstat_read_file,
	.write = write_file_dummy,
	.open = open_file_generic,
};

static struct file_operations txpower_g_fops = {
	.read = txpower_g_read_file,
	.write = txpower_g_write_file,
	.open = open_file_generic,
};

static struct file_operations restart_fops = {
	.write = restart_write_file,
	.open = open_file_generic,
};


int bcm43xx_debug(struct bcm43xx_wldev *dev, enum bcm43xx_dyndbg feature)
{
	return !!(dev->dfsentry->dyn_debug[feature]);
}

static void bcm43xx_remove_dynamic_debug(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_dfsentry *e = dev->dfsentry;
	int i;

	for (i = 0; i < __BCM43xx_NR_DYNDBG; i++)
		debugfs_remove(e->dyn_debug_dentries[i]);
}

static void bcm43xx_add_dynamic_debug(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_dfsentry *e = dev->dfsentry;
	struct dentry *d;

#define add_dyn_dbg(name, id, initstate) do {		\
	e->dyn_debug[id] = (initstate);			\
	d = debugfs_create_bool(name, 0600, e->subdir,	\
				&(e->dyn_debug[id]));	\
	if (!IS_ERR(d))					\
		e->dyn_debug_dentries[id] = d;		\
				} while (0)

	add_dyn_dbg("debug_xmitpower", BCM43xx_DBG_XMITPOWER, 0);
	add_dyn_dbg("debug_dmaoverflow", BCM43xx_DBG_DMAOVERFLOW, 0);
	add_dyn_dbg("debug_dmaverbose", BCM43xx_DBG_DMAVERBOSE, 0);
	add_dyn_dbg("debug_pwork_fast", BCM43xx_DBG_PWORK_FAST, 0);
	add_dyn_dbg("debug_pwork_stop", BCM43xx_DBG_PWORK_STOP, 0);

#undef add_dyn_dbg
}

void bcm43xx_debugfs_add_device(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_dfsentry *e;
	struct bcm43xx_txstatus_log *log;
	char devdir[16];

	assert(dev);
	e = kzalloc(sizeof(*e), GFP_KERNEL);
	if (!e) {
		printk(KERN_ERR PFX "debugfs: add device OOM\n");
		return;
	}
	e->dev = dev;
	log = &e->txstatlog;
	log->log = kcalloc(BCM43xx_NR_LOGGED_TXSTATUS,
			   sizeof(struct bcm43xx_txstatus),
			   GFP_KERNEL);
	if (!log->log) {
		printk(KERN_ERR PFX "debugfs: add device txstatus OOM\n");
		kfree(e);
		return;
	}
	log->end = -1;
	spin_lock_init(&log->lock);

	dev->dfsentry = e;

	snprintf(devdir, sizeof(devdir), "%s", wiphy_name(dev->wl->hw->wiphy));
	e->subdir = debugfs_create_dir(devdir, fs.root);
	if (!e->subdir || IS_ERR(e->subdir)) {
		e->subdir = NULL;
		kfree(log->log);
		kfree(e);
		return;
	}

	e->dentry_tsf = debugfs_create_file("tsf", 0600, e->subdir,
	                                    dev, &tsf_fops);
	if (IS_ERR(e->dentry_tsf))
		e->dentry_tsf = NULL;
	e->dentry_txstat = debugfs_create_file("tx_status", 0400, e->subdir,
						dev, &txstat_fops);
	if (IS_ERR(e->dentry_txstat))
		e->dentry_txstat = NULL;
	e->dentry_txpower_g = debugfs_create_file("tx_power_g", 0600, e->subdir,
						  dev, &txpower_g_fops);
	if (IS_ERR(e->dentry_txpower_g))
		e->dentry_txpower_g = NULL;
	e->dentry_restart = debugfs_create_file("restart", 0200, e->subdir,
						dev, &restart_fops);
	if (IS_ERR(e->dentry_restart))
		e->dentry_restart = NULL;

	bcm43xx_add_dynamic_debug(dev);
}

void bcm43xx_debugfs_remove_device(struct bcm43xx_wldev *dev)
{
	struct bcm43xx_dfsentry *e;

	if (!dev)
		return;
	e = dev->dfsentry;
	if (!e)
		return;
	bcm43xx_remove_dynamic_debug(dev);
	debugfs_remove(e->dentry_tsf);
	debugfs_remove(e->dentry_txstat);
	debugfs_remove(e->dentry_restart);
	debugfs_remove(e->dentry_txpower_g);
	debugfs_remove(e->subdir);
	kfree(e->txstatlog.log);
	kfree(e);
}

void bcm43xx_debugfs_log_txstat(struct bcm43xx_wldev *dev,
				const struct bcm43xx_txstatus *status)
{
	struct bcm43xx_dfsentry *e = dev->dfsentry;
	struct bcm43xx_txstatus_log *log;
	struct bcm43xx_txstatus *cur;
	int i;

	log = &e->txstatlog;
	assert(irqs_disabled());
	spin_lock(&log->lock);
	i = log->end + 1;
	if (i == BCM43xx_NR_LOGGED_TXSTATUS)
		i = 0;
	log->end = i;
	cur = &(log->log[i]);
	memcpy(cur, status, sizeof(*cur));
	spin_unlock(&log->lock);
}

void bcm43xx_debugfs_init(void)
{
	memset(&fs, 0, sizeof(fs));
	fs.root = debugfs_create_dir(KBUILD_MODNAME, NULL);
	if (!fs.root || IS_ERR(fs.root)) {
		fs.root = NULL;
		return;
	}
	fs.dentry_driverinfo = debugfs_create_file("driver", 0444, fs.root,
						   NULL, &drvinfo_fops);
	if (IS_ERR(fs.dentry_driverinfo))
		fs.dentry_driverinfo = NULL;
}

void bcm43xx_debugfs_exit(void)
{
	debugfs_remove(fs.dentry_driverinfo);
	debugfs_remove(fs.root);
}

void bcm43xx_printk_dump(const char *data,
			 size_t size,
			 const char *description)
{
	unsigned int i;
	char c;

	printk(KERN_INFO PFX "Data dump (%s, %lu bytes):",
	       description, (unsigned long)size);
	for (i = 0; i < size; i++) {
		c = data[i];
		if (i % 8 == 0)
			printk("\n" KERN_INFO PFX "0x%08x:  0x%02x, ", i, c & 0xff);
		else
			printk("0x%02x, ", c & 0xff);
	}
	printk("\n");
}

void bcm43xx_printk_bitdump(const unsigned char *data,
			    size_t bytes, int msb_to_lsb,
			    const char *description)
{
	unsigned int i;
	int j;
	const unsigned char *d;

	printk(KERN_INFO PFX "*** Bitdump (%s, %lu bytes, %s) ***",
	       description, (unsigned long)bytes,
	       msb_to_lsb ? "MSB to LSB" : "LSB to MSB");
	for (i = 0; i < bytes; i++) {
		d = data + i;
		if (i % 8 == 0)
			printk("\n" KERN_INFO PFX "0x%08x:  ", i);
		if (msb_to_lsb) {
			for (j = 7; j >= 0; j--) {
				if (*d & (1 << j))
					printk("1");
				else
					printk("0");
			}
		} else {
			for (j = 0; j < 8; j++) {
				if (*d & (1 << j))
					printk("1");
				else
					printk("0");
			}
		}
		printk(" ");
	}
	printk("\n");
}
