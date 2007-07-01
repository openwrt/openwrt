#ifndef BCM43xx_DEBUGFS_H_
#define BCM43xx_DEBUGFS_H_

struct bcm43xx_wldev;
struct bcm43xx_txstatus;

enum bcm43xx_dyndbg { /* Dynamic debugging features */
	BCM43xx_DBG_XMITPOWER,
	BCM43xx_DBG_DMAOVERFLOW,
	BCM43xx_DBG_DMAVERBOSE,
	BCM43xx_DBG_PWORK_FAST,
	BCM43xx_DBG_PWORK_STOP,
	__BCM43xx_NR_DYNDBG,
};


#ifdef CONFIG_BCM43XX_MAC80211_DEBUG

struct dentry;

#define BCM43xx_NR_LOGGED_TXSTATUS	100

struct bcm43xx_txstatus_log {
	struct bcm43xx_txstatus *log;
	int end;
	int printing;
	char printbuf[(BCM43xx_NR_LOGGED_TXSTATUS * 70) + 200];
	size_t buf_avail;
	spinlock_t lock;
};

struct bcm43xx_dfsentry {
	struct dentry *subdir;
	struct dentry *dentry_tsf;
	struct dentry *dentry_txstat;
	struct dentry *dentry_txpower_g;
	struct dentry *dentry_restart;

	struct bcm43xx_wldev *dev;

	struct bcm43xx_txstatus_log txstatlog;

	/* Enabled/Disabled list for the dynamic debugging features. */
	u32 dyn_debug[__BCM43xx_NR_DYNDBG];
	/* Dentries for the dynamic debugging entries. */
	struct dentry *dyn_debug_dentries[__BCM43xx_NR_DYNDBG];
};

struct bcm43xx_debugfs {
	struct dentry *root;
	struct dentry *dentry_driverinfo;
};

int bcm43xx_debug(struct bcm43xx_wldev *dev, enum bcm43xx_dyndbg feature);

void bcm43xx_debugfs_init(void);
void bcm43xx_debugfs_exit(void);
void bcm43xx_debugfs_add_device(struct bcm43xx_wldev *dev);
void bcm43xx_debugfs_remove_device(struct bcm43xx_wldev *dev);
void bcm43xx_debugfs_log_txstat(struct bcm43xx_wldev *dev,
				const struct bcm43xx_txstatus *status);

/* Debug helper: Dump binary data through printk. */
void bcm43xx_printk_dump(const char *data,
			 size_t size,
			 const char *description);
/* Debug helper: Dump bitwise binary data through printk. */
void bcm43xx_printk_bitdump(const unsigned char *data,
			    size_t bytes, int msb_to_lsb,
			    const char *description);
#define bcm43xx_printk_bitdumpt(pointer, msb_to_lsb, description) \
	do {									\
		bcm43xx_printk_bitdump((const unsigned char *)(pointer),	\
				       sizeof(*(pointer)),			\
				       (msb_to_lsb),				\
				       (description));				\
	} while (0)

#else /* CONFIG_BCM43XX_MAC80211_DEBUG*/

static inline
int bcm43xx_debug(struct bcm43xx_wldev *dev, enum bcm43xx_dyndbg feature)
{
	return 0;
}

static inline
void bcm43xx_debugfs_init(void) { }
static inline
void bcm43xx_debugfs_exit(void) { }
static inline
void bcm43xx_debugfs_add_device(struct bcm43xx_wldev *dev) { }
static inline
void bcm43xx_debugfs_remove_device(struct bcm43xx_wldev *dev) { }
static inline
void bcm43xx_debugfs_log_txstat(struct bcm43xx_wldev *dev,
				const struct bcm43xx_txstatus *status) { }

static inline
void bcm43xx_printk_dump(const char *data,
			 size_t size,
			 const char *description)
{
}
static inline
void bcm43xx_printk_bitdump(const unsigned char *data,
			    size_t bytes, int msb_to_lsb,
			    const char *description)
{
}
#define bcm43xx_printk_bitdumpt(pointer, msb_to_lsb, description)  do { /* nothing */ } while (0)

#endif /* CONFIG_BCM43XX_MAC80211_DEBUG*/

/* Ugly helper macros to make incomplete code more verbose on runtime */
#ifdef TODO
# undef TODO
#endif
#define TODO()  \
	do {										\
		printk(KERN_INFO PFX "TODO: Incomplete code in %s() at %s:%d\n",	\
		       __FUNCTION__, __FILE__, __LINE__);				\
	} while (0)

#ifdef FIXME
# undef FIXME
#endif
#define FIXME()  \
	do {										\
		printk(KERN_INFO PFX "FIXME: Possibly broken code in %s() at %s:%d\n",	\
		       __FUNCTION__, __FILE__, __LINE__);				\
	} while (0)

#endif /* BCM43xx_DEBUGFS_H_ */
