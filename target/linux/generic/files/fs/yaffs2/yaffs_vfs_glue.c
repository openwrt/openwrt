/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2010 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 * Acknowledgements:
 * Luc van OostenRyck for numerous patches.
 * Nick Bane for numerous patches.
 * Nick Bane for 2.5/2.6 integration.
 * Andras Toth for mknod rdev issue.
 * Michael Fischer for finding the problem with inode inconsistency.
 * Some code bodily lifted from JFFS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 *
 * This is the file system front-end to YAFFS that hooks it up to
 * the VFS.
 *
 * Special notes:
 * >> 2.4: sb->u.generic_sbp points to the yaffs_dev_t associated with
 *         this superblock
 * >> 2.6: sb->s_fs_info  points to the yaffs_dev_t associated with this
 *         superblock
 * >> inode->u.generic_ip points to the associated yaffs_obj_t.
 */

/*
 * There are two variants of the VFS glue code. This variant should compile
 * for any version of Linux.
 */
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 10))
#define YAFFS_COMPILE_BACKGROUND
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6, 23))
#define YAFFS_COMPILE_FREEZER
#endif
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28))
#define YAFFS_COMPILE_EXPORTFS
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35))
#define YAFFS_USE_SETATTR_COPY
#define YAFFS_USE_TRUNCATE_SETSIZE
#endif
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,35))
#define YAFFS_HAS_EVICT_INODE
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,13))
#define YAFFS_NEW_FOLLOW_LINK 1
#else
#define YAFFS_NEW_FOLLOW_LINK 0
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 19))
#include <linux/config.h>
#endif

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/smp_lock.h>
#include <linux/pagemap.h>
#include <linux/mtd/mtd.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/ctype.h>

#if (YAFFS_NEW_FOLLOW_LINK == 1)
#include <linux/namei.h>
#endif

#ifdef YAFFS_COMPILE_EXPORTFS
#include <linux/exportfs.h>
#endif

#ifdef YAFFS_COMPILE_BACKGROUND
#include <linux/kthread.h>
#include <linux/delay.h>
#endif
#ifdef YAFFS_COMPILE_FREEZER
#include <linux/freezer.h>
#endif

#include <asm/div64.h>

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))

#include <linux/statfs.h>

#define UnlockPage(p) unlock_page(p)
#define Page_Uptodate(page)	test_bit(PG_uptodate, &(page)->flags)

/* FIXME: use sb->s_id instead ? */
#define yaffs_devname(sb, buf)	bdevname(sb->s_bdev, buf)

#else

#include <linux/locks.h>
#define	BDEVNAME_SIZE		0
#define	yaffs_devname(sb, buf)	kdevname(sb->s_dev)

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 0))
/* added NCB 26/5/2006 for 2.4.25-vrs2-tcl1 kernel */
#define __user
#endif

#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 26))
#define YPROC_ROOT  (&proc_root)
#else
#define YPROC_ROOT  NULL
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))
#define Y_INIT_TIMER(a)	init_timer(a)
#else
#define Y_INIT_TIMER(a)	init_timer_on_stack(a)
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
#define WRITE_SIZE_STR "writesize"
#define WRITE_SIZE(mtd) ((mtd)->writesize)
#else
#define WRITE_SIZE_STR "oobblock"
#define WRITE_SIZE(mtd) ((mtd)->oobblock)
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 27))
#define YAFFS_USE_WRITE_BEGIN_END 1
#else
#define YAFFS_USE_WRITE_BEGIN_END 0
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 28))
static uint32_t YCALCBLOCKS(uint64_t partition_size, uint32_t block_size)
{
	uint64_t result = partition_size;
	do_div(result, block_size);
	return (uint32_t)result;
}
#else
#define YCALCBLOCKS(s, b) ((s)/(b))
#endif

#include <linux/uaccess.h>
#include <linux/mtd/mtd.h>

#include "yportenv.h"
#include "yaffs_trace.h"
#include "yaffs_guts.h"

#include "yaffs_linux.h"

#include "yaffs_mtdif.h"
#include "yaffs_mtdif1.h"
#include "yaffs_mtdif2.h"

unsigned int yaffs_trace_mask = YAFFS_TRACE_BAD_BLOCKS | YAFFS_TRACE_ALWAYS;
unsigned int yaffs_wr_attempts = YAFFS_WR_ATTEMPTS;
unsigned int yaffs_auto_checkpoint = 1;
unsigned int yaffs_gc_control = 1;
unsigned int yaffs_bg_enable = 1;

/* Module Parameters */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
module_param(yaffs_trace_mask, uint, 0644);
module_param(yaffs_wr_attempts, uint, 0644);
module_param(yaffs_auto_checkpoint, uint, 0644);
module_param(yaffs_gc_control, uint, 0644);
module_param(yaffs_bg_enable, uint, 0644);
#else
MODULE_PARM(yaffs_trace_mask, "i");
MODULE_PARM(yaffs_wr_attempts, "i");
MODULE_PARM(yaffs_auto_checkpoint, "i");
MODULE_PARM(yaffs_gc_control, "i");
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 25))
/* use iget and read_inode */
#define Y_IGET(sb, inum) iget((sb), (inum))
static void yaffs_read_inode(struct inode *inode);

#else
/* Call local equivalent */
#define YAFFS_USE_OWN_IGET
#define Y_IGET(sb, inum) yaffs_iget((sb), (inum))

static struct inode *yaffs_iget(struct super_block *sb, unsigned long ino);
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 18))
#define yaffs_InodeToObjectLV(iptr) ((iptr)->i_private)
#else
#define yaffs_InodeToObjectLV(iptr) ((iptr)->u.generic_ip)
#endif

#define yaffs_InodeToObject(iptr) ((yaffs_obj_t *)(yaffs_InodeToObjectLV(iptr)))
#define yaffs_dentry_to_obj(dptr) yaffs_InodeToObject((dptr)->d_inode)

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
#define yaffs_SuperToDevice(sb)	((yaffs_dev_t *)sb->s_fs_info)
#else
#define yaffs_SuperToDevice(sb)	((yaffs_dev_t *)sb->u.generic_sbp)
#endif


#define update_dir_time(dir) do {\
			(dir)->i_ctime = (dir)->i_mtime = CURRENT_TIME; \
		} while(0)
		
static void yaffs_put_super(struct super_block *sb);

static ssize_t yaffs_file_write(struct file *f, const char *buf, size_t n,
				loff_t *pos);
static ssize_t yaffs_hold_space(struct file *f);
static void yaffs_release_space(struct file *f);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
static int yaffs_file_flush(struct file *file, fl_owner_t id);
#else
static int yaffs_file_flush(struct file *file);
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 34))
static int yaffs_sync_object(struct file *file, int datasync);
#else
static int yaffs_sync_object(struct file *file, struct dentry *dentry,
				int datasync);
#endif

static int yaffs_readdir(struct file *f, void *dirent, filldir_t filldir);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
static int yaffs_create(struct inode *dir, struct dentry *dentry, int mode,
			struct nameidata *n);
static struct dentry *yaffs_lookup(struct inode *dir, struct dentry *dentry,
					struct nameidata *n);
#else
static int yaffs_create(struct inode *dir, struct dentry *dentry, int mode);
static struct dentry *yaffs_lookup(struct inode *dir, struct dentry *dentry);
#endif
static int yaffs_link(struct dentry *old_dentry, struct inode *dir,
			struct dentry *dentry);
static int yaffs_unlink(struct inode *dir, struct dentry *dentry);
static int yaffs_symlink(struct inode *dir, struct dentry *dentry,
			const char *symname);
static int yaffs_mkdir(struct inode *dir, struct dentry *dentry, int mode);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
static int yaffs_mknod(struct inode *dir, struct dentry *dentry, int mode,
			dev_t dev);
#else
static int yaffs_mknod(struct inode *dir, struct dentry *dentry, int mode,
			int dev);
#endif
static int yaffs_rename(struct inode *old_dir, struct dentry *old_dentry,
			struct inode *new_dir, struct dentry *new_dentry);
static int yaffs_setattr(struct dentry *dentry, struct iattr *attr);

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
static int yaffs_sync_fs(struct super_block *sb, int wait);
static void yaffs_write_super(struct super_block *sb);
#else
static int yaffs_sync_fs(struct super_block *sb);
static int yaffs_write_super(struct super_block *sb);
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
static int yaffs_statfs(struct dentry *dentry, struct kstatfs *buf);
#elif (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
static int yaffs_statfs(struct super_block *sb, struct kstatfs *buf);
#else
static int yaffs_statfs(struct super_block *sb, struct statfs *buf);
#endif

#ifdef YAFFS_HAS_PUT_INODE
static void yaffs_put_inode(struct inode *inode);
#endif

#ifdef YAFFS_HAS_EVICT_INODE
static void yaffs_evict_inode(struct inode *);
#else
static void yaffs_delete_inode(struct inode *);
static void yaffs_clear_inode(struct inode *);
#endif

static int yaffs_readpage(struct file *file, struct page *page);
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
static int yaffs_writepage(struct page *page, struct writeback_control *wbc);
#else
static int yaffs_writepage(struct page *page);
#endif

#ifdef CONFIG_YAFFS_XATTR
int yaffs_setxattr(struct dentry *dentry, const char *name,
			const void *value, size_t size, int flags);
ssize_t yaffs_getxattr(struct dentry *dentry, const char *name, void *buff,
			size_t size);
int yaffs_removexattr(struct dentry *dentry, const char *name);
ssize_t yaffs_listxattr(struct dentry *dentry, char *buff, size_t size);
#endif


#if (YAFFS_USE_WRITE_BEGIN_END != 0)
static int yaffs_write_begin(struct file *filp, struct address_space *mapping,
				loff_t pos, unsigned len, unsigned flags,
				struct page **pagep, void **fsdata);
static int yaffs_write_end(struct file *filp, struct address_space *mapping,
				loff_t pos, unsigned len, unsigned copied,
				struct page *pg, void *fsdadata);
#else
static int yaffs_prepare_write(struct file *f, struct page *pg,
				unsigned offset, unsigned to);
static int yaffs_commit_write(struct file *f, struct page *pg, unsigned offset,
				unsigned to);

#endif

static int yaffs_readlink(struct dentry *dentry, char __user *buffer,
				int buflen);
#if (YAFFS_NEW_FOLLOW_LINK == 1)
void yaffs_put_link(struct dentry *dentry, struct nameidata *nd, void *alias);
static void *yaffs_follow_link(struct dentry *dentry, struct nameidata *nd);
#else
static int yaffs_follow_link(struct dentry *dentry, struct nameidata *nd);
#endif

static void yaffs_touch_super(yaffs_dev_t *dev);

static loff_t yaffs_dir_llseek(struct file *file, loff_t offset, int origin);

static int yaffs_vfs_setattr(struct inode *, struct iattr *);


static struct address_space_operations yaffs_file_address_operations = {
	.readpage = yaffs_readpage,
	.writepage = yaffs_writepage,
#if (YAFFS_USE_WRITE_BEGIN_END > 0)
	.write_begin = yaffs_write_begin,
	.write_end = yaffs_write_end,
#else
	.prepare_write = yaffs_prepare_write,
	.commit_write = yaffs_commit_write,
#endif
};


#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 22))
static const struct file_operations yaffs_file_operations = {
	.read = do_sync_read,
	.write = do_sync_write,
	.aio_read = generic_file_aio_read,
	.aio_write = generic_file_aio_write,
	.mmap = generic_file_mmap,
	.flush = yaffs_file_flush,
	.fsync = yaffs_sync_object,
	.splice_read = generic_file_splice_read,
	.splice_write = generic_file_splice_write,
	.llseek = generic_file_llseek,
};

#elif (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 18))

static const struct file_operations yaffs_file_operations = {
	.read = do_sync_read,
	.write = do_sync_write,
	.aio_read = generic_file_aio_read,
	.aio_write = generic_file_aio_write,
	.mmap = generic_file_mmap,
	.flush = yaffs_file_flush,
	.fsync = yaffs_sync_object,
	.sendfile = generic_file_sendfile,
};

#else

static const struct file_operations yaffs_file_operations = {
	.read = generic_file_read,
	.write = generic_file_write,
	.mmap = generic_file_mmap,
	.flush = yaffs_file_flush,
	.fsync = yaffs_sync_object,
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
	.sendfile = generic_file_sendfile,
#endif
};
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25))
static void zero_user_segment(struct page *page, unsigned start, unsigned end)
{
	void * kaddr = kmap_atomic(page, KM_USER0);
	memset(kaddr + start, 0, end - start);
	kunmap_atomic(kaddr, KM_USER0);
	flush_dcache_page(page);
}
#endif


static const struct inode_operations yaffs_file_inode_operations = {
	.setattr = yaffs_setattr,
#ifdef CONFIG_YAFFS_XATTR
	.setxattr = yaffs_setxattr,
	.getxattr = yaffs_getxattr,
	.listxattr = yaffs_listxattr,
	.removexattr = yaffs_removexattr,
#endif
};

static const struct inode_operations yaffs_symlink_inode_operations = {
	.readlink = yaffs_readlink,
	.follow_link = yaffs_follow_link,
#if (YAFFS_NEW_FOLLOW_LINK == 1)
	.put_link = yaffs_put_link,
#endif
	.setattr = yaffs_setattr,
#ifdef CONFIG_YAFFS_XATTR
	.setxattr = yaffs_setxattr,
	.getxattr = yaffs_getxattr,
	.listxattr = yaffs_listxattr,
	.removexattr = yaffs_removexattr,
#endif
};

static const struct inode_operations yaffs_dir_inode_operations = {
	.create = yaffs_create,
	.lookup = yaffs_lookup,
	.link = yaffs_link,
	.unlink = yaffs_unlink,
	.symlink = yaffs_symlink,
	.mkdir = yaffs_mkdir,
	.rmdir = yaffs_unlink,
	.mknod = yaffs_mknod,
	.rename = yaffs_rename,
	.setattr = yaffs_setattr,
#ifdef CONFIG_YAFFS_XATTR
	.setxattr = yaffs_setxattr,
	.getxattr = yaffs_getxattr,
	.listxattr = yaffs_listxattr,
	.removexattr = yaffs_removexattr,
#endif
};

static const struct file_operations yaffs_dir_operations = {
	.read = generic_read_dir,
	.readdir = yaffs_readdir,
	.fsync = yaffs_sync_object,
	.llseek = yaffs_dir_llseek,
};

static const struct super_operations yaffs_super_ops = {
	.statfs = yaffs_statfs,

#ifndef YAFFS_USE_OWN_IGET
	.read_inode = yaffs_read_inode,
#endif
#ifdef YAFFS_HAS_PUT_INODE
	.put_inode = yaffs_put_inode,
#endif
	.put_super = yaffs_put_super,
#ifdef YAFFS_HAS_EVICT_INODE
	.evict_inode = yaffs_evict_inode,
#else
	.delete_inode = yaffs_delete_inode,
	.clear_inode = yaffs_clear_inode,
#endif
	.sync_fs = yaffs_sync_fs,
	.write_super = yaffs_write_super,
};


static  int yaffs_vfs_setattr(struct inode *inode, struct iattr *attr)
{
#ifdef  YAFFS_USE_SETATTR_COPY
	setattr_copy(inode,attr);
	return 0;
#else
	return inode_setattr(inode, attr);
#endif

}

static  int yaffs_vfs_setsize(struct inode *inode, loff_t newsize)
{
#ifdef  YAFFS_USE_TRUNCATE_SETSIZE
	truncate_setsize(inode,newsize);
	return 0;
#else
	truncate_inode_pages(&inode->i_data,newsize);
	return 0;
#endif

}

static unsigned yaffs_gc_control_callback(yaffs_dev_t *dev)
{
	return yaffs_gc_control;
}
                	                                                                                          	
static void yaffs_gross_lock(yaffs_dev_t *dev)
{
	T(YAFFS_TRACE_LOCK, (TSTR("yaffs locking %p\n"), current));
	down(&(yaffs_dev_to_lc(dev)->grossLock));
	T(YAFFS_TRACE_LOCK, (TSTR("yaffs locked %p\n"), current));
}

static void yaffs_gross_unlock(yaffs_dev_t *dev)
{
	T(YAFFS_TRACE_LOCK, (TSTR("yaffs unlocking %p\n"), current));
	up(&(yaffs_dev_to_lc(dev)->grossLock));
}

#ifdef YAFFS_COMPILE_EXPORTFS

static struct inode *
yaffs2_nfs_get_inode(struct super_block *sb, uint64_t ino, uint32_t generation)
{
	return Y_IGET(sb, ino);
}

static struct dentry *
yaffs2_fh_to_dentry(struct super_block *sb, struct fid *fid, int fh_len, int fh_type)
{
	return generic_fh_to_dentry(sb, fid, fh_len, fh_type, yaffs2_nfs_get_inode) ;
}

static struct dentry *
 yaffs2_fh_to_parent(struct super_block *sb, struct fid *fid, int fh_len, int fh_type)
{
	return generic_fh_to_parent(sb, fid, fh_len, fh_type, yaffs2_nfs_get_inode);
}

struct dentry *yaffs2_get_parent(struct dentry *dentry)
{

	struct super_block *sb = dentry->d_inode->i_sb;
	struct dentry *parent = ERR_PTR(-ENOENT);
	struct inode *inode;
	unsigned long parent_ino;
	yaffs_obj_t *d_obj;
	yaffs_obj_t *parent_obj;

	d_obj = yaffs_InodeToObject(dentry->d_inode);

	if (d_obj) {
		parent_obj = d_obj->parent;
		if (parent_obj) {
			parent_ino = yaffs_get_obj_inode(parent_obj);
			inode = Y_IGET(sb, parent_ino);

			if (IS_ERR(inode)) {
				parent = ERR_CAST(inode);
			} else {
				parent = d_obtain_alias(inode);
				if (!IS_ERR(parent)) {
					parent = ERR_PTR(-ENOMEM);
					iput(inode);
				}
			}
		}
	}

	return parent;
}

/* Just declare a zero structure as a NULL value implies
 * using the default functions of exportfs.
 */

static struct export_operations yaffs_export_ops =
{
	.fh_to_dentry = yaffs2_fh_to_dentry,
	.fh_to_parent = yaffs2_fh_to_parent,
	.get_parent = yaffs2_get_parent,
} ;

#endif

/*-----------------------------------------------------------------*/
/* Directory search context allows us to unlock access to yaffs during
 * filldir without causing problems with the directory being modified.
 * This is similar to the tried and tested mechanism used in yaffs direct.
 *
 * A search context iterates along a doubly linked list of siblings in the
 * directory. If the iterating object is deleted then this would corrupt
 * the list iteration, likely causing a crash. The search context avoids
 * this by using the remove_obj_fn to move the search context to the
 * next object before the object is deleted.
 *
 * Many readdirs (and thus seach conexts) may be alive simulateously so
 * each yaffs_dev_t has a list of these.
 *
 * A seach context lives for the duration of a readdir.
 *
 * All these functions must be called while yaffs is locked.
 */

struct yaffs_SearchContext {
	yaffs_dev_t *dev;
	yaffs_obj_t *dirObj;
	yaffs_obj_t *nextReturn;
	struct ylist_head others;
};

/*
 * yaffs_NewSearch() creates a new search context, initialises it and
 * adds it to the device's search context list.
 *
 * Called at start of readdir.
 */
static struct yaffs_SearchContext * yaffs_NewSearch(yaffs_obj_t *dir)
{
	yaffs_dev_t *dev = dir->my_dev;
	struct yaffs_SearchContext *sc = YMALLOC(sizeof(struct yaffs_SearchContext));
	if(sc){
		sc->dirObj = dir;
		sc->dev = dev;
		if( ylist_empty(&sc->dirObj->variant.dir_variant.children))
			sc->nextReturn = NULL;
		else
			sc->nextReturn = ylist_entry(
                                dir->variant.dir_variant.children.next,
				yaffs_obj_t,siblings);
		YINIT_LIST_HEAD(&sc->others);
		ylist_add(&sc->others,&(yaffs_dev_to_lc(dev)->searchContexts));
	}
	return sc;
}

/*
 * yaffs_search_end() disposes of a search context and cleans up.
 */
static void yaffs_search_end(struct yaffs_SearchContext * sc)
{
	if(sc){
		ylist_del(&sc->others);
		YFREE(sc);
	}
}

/*
 * yaffs_search_advance() moves a search context to the next object.
 * Called when the search iterates or when an object removal causes
 * the search context to be moved to the next object.
 */
static void yaffs_search_advance(struct yaffs_SearchContext *sc)
{
        if(!sc)
                return;

        if( sc->nextReturn == NULL ||
                ylist_empty(&sc->dirObj->variant.dir_variant.children))
                sc->nextReturn = NULL;
        else {
                struct ylist_head *next = sc->nextReturn->siblings.next;

                if( next == &sc->dirObj->variant.dir_variant.children)
                        sc->nextReturn = NULL; /* end of list */
                else
                        sc->nextReturn = ylist_entry(next,yaffs_obj_t,siblings);
        }
}

/*
 * yaffs_remove_obj_callback() is called when an object is unlinked.
 * We check open search contexts and advance any which are currently
 * on the object being iterated.
 */
static void yaffs_remove_obj_callback(yaffs_obj_t *obj)
{

        struct ylist_head *i;
        struct yaffs_SearchContext *sc;
        struct ylist_head *search_contexts = &(yaffs_dev_to_lc(obj->my_dev)->searchContexts);


        /* Iterate through the directory search contexts.
         * If any are currently on the object being removed, then advance
         * the search context to the next object to prevent a hanging pointer.
         */
         ylist_for_each(i, search_contexts) {
                if (i) {
                        sc = ylist_entry(i, struct yaffs_SearchContext,others);
                        if(sc->nextReturn == obj)
                                yaffs_search_advance(sc);
                }
	}

}


/*-----------------------------------------------------------------*/

static int yaffs_readlink(struct dentry *dentry, char __user *buffer,
			int buflen)
{
	unsigned char *alias;
	int ret;

	yaffs_dev_t *dev = yaffs_dentry_to_obj(dentry)->my_dev;

	yaffs_gross_lock(dev);

	alias = yaffs_get_symlink_alias(yaffs_dentry_to_obj(dentry));

	yaffs_gross_unlock(dev);

	if (!alias)
		return -ENOMEM;

	ret = vfs_readlink(dentry, buffer, buflen, alias);
	kfree(alias);
	return ret;
}

#if (YAFFS_NEW_FOLLOW_LINK == 1)
static void *yaffs_follow_link(struct dentry *dentry, struct nameidata *nd)
#else
static int yaffs_follow_link(struct dentry *dentry, struct nameidata *nd)
#endif
{
	unsigned char *alias;
	int ret;
	yaffs_dev_t *dev = yaffs_dentry_to_obj(dentry)->my_dev;

	yaffs_gross_lock(dev);

	alias = yaffs_get_symlink_alias(yaffs_dentry_to_obj(dentry));
	yaffs_gross_unlock(dev);

	if (!alias) {
		ret = -ENOMEM;
		goto out;
	}

#if (YAFFS_NEW_FOLLOW_LINK == 1)
	nd_set_link(nd, alias);
	ret = (int)alias;
out:
	return ERR_PTR(ret);
#else
	ret = vfs_follow_link(nd, alias);
	kfree(alias);
out:
	return ret;
#endif
}

#if (YAFFS_NEW_FOLLOW_LINK == 1)
void yaffs_put_link(struct dentry *dentry, struct nameidata *nd, void *alias) {
	kfree(alias);
}
#endif

struct inode *yaffs_get_inode(struct super_block *sb, int mode, int dev,
				yaffs_obj_t *obj);

/*
 * Lookup is used to find objects in the fs
 */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))

static struct dentry *yaffs_lookup(struct inode *dir, struct dentry *dentry,
				struct nameidata *n)
#else
static struct dentry *yaffs_lookup(struct inode *dir, struct dentry *dentry)
#endif
{
	yaffs_obj_t *obj;
	struct inode *inode = NULL;	/* NCB 2.5/2.6 needs NULL here */

	yaffs_dev_t *dev = yaffs_InodeToObject(dir)->my_dev;

	if(current != yaffs_dev_to_lc(dev)->readdirProcess)
		yaffs_gross_lock(dev);

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_lookup for %d:%s\n"),
		yaffs_InodeToObject(dir)->obj_id, dentry->d_name.name));

	obj = yaffs_find_by_name(yaffs_InodeToObject(dir),
					dentry->d_name.name);

	obj = yaffs_get_equivalent_obj(obj);	/* in case it was a hardlink */

	/* Can't hold gross lock when calling yaffs_get_inode() */
	if(current != yaffs_dev_to_lc(dev)->readdirProcess)
		yaffs_gross_unlock(dev);

	if (obj) {
		T(YAFFS_TRACE_OS,
			(TSTR("yaffs_lookup found %d\n"), obj->obj_id));

		inode = yaffs_get_inode(dir->i_sb, obj->yst_mode, 0, obj);

		if (inode) {
			T(YAFFS_TRACE_OS,
				(TSTR("yaffs_loookup dentry \n")));
/* #if 0 asserted by NCB for 2.5/6 compatability - falls through to
 * d_add even if NULL inode */
#if 0
			/*dget(dentry); // try to solve directory bug */
			d_add(dentry, inode);

			/* return dentry; */
			return NULL;
#endif
		}

	} else {
		T(YAFFS_TRACE_OS,(TSTR("yaffs_lookup not found\n")));

	}

/* added NCB for 2.5/6 compatability - forces add even if inode is
 * NULL which creates dentry hash */
	d_add(dentry, inode);

	return NULL;
}


#ifdef YAFFS_HAS_PUT_INODE

/* For now put inode is just for debugging
 * Put inode is called when the inode **structure** is put.
 */
static void yaffs_put_inode(struct inode *inode)
{
	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_put_inode: ino %d, count %d\n"), (int)inode->i_ino,
		atomic_read(&inode->i_count)));

}
#endif


static void yaffs_unstitch_obj(struct inode *inode, yaffs_obj_t *obj)
{
	/* Clear the association between the inode and
	 * the yaffs_obj_t.
	 */
	obj->my_inode = NULL;
	yaffs_InodeToObjectLV(inode) = NULL;

	/* If the object freeing was deferred, then the real
	 * free happens now.
	 * This should fix the inode inconsistency problem.
	 */
	yaffs_handle_defered_free(obj);
}

#ifdef YAFFS_HAS_EVICT_INODE
/* yaffs_evict_inode combines into one operation what was previously done in
 * yaffs_clear_inode() and yaffs_delete_inode()
 *
 */
static void yaffs_evict_inode( struct inode *inode)
{
	yaffs_obj_t *obj;
	yaffs_dev_t *dev;
	int deleteme = 0;

	obj = yaffs_InodeToObject(inode);

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_evict_inode: ino %d, count %d %s\n"), (int)inode->i_ino,
		atomic_read(&inode->i_count),
		obj ? "object exists" : "null object"));

	if (!inode->i_nlink && !is_bad_inode(inode))
		deleteme = 1;
	truncate_inode_pages(&inode->i_data,0);
	end_writeback(inode);

	if(deleteme && obj){
		dev = obj->my_dev;
		yaffs_gross_lock(dev);
		yaffs_del_obj(obj);
		yaffs_gross_unlock(dev);
	}
	if (obj) {
		dev = obj->my_dev;
		yaffs_gross_lock(dev);
		yaffs_unstitch_obj(inode,obj);
		yaffs_gross_unlock(dev);
	}


}
#else

/* clear is called to tell the fs to release any per-inode data it holds.
 * The object might still exist on disk and is just being thrown out of the cache
 * or else the object has actually been deleted and we're being called via
 * the chain
 *   yaffs_delete_inode() -> clear_inode()->yaffs_clear_inode()
 */

static void yaffs_clear_inode(struct inode *inode)
{
	yaffs_obj_t *obj;
	yaffs_dev_t *dev;

	obj = yaffs_InodeToObject(inode);

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_clear_inode: ino %d, count %d %s\n"), (int)inode->i_ino,
		atomic_read(&inode->i_count),
		obj ? "object exists" : "null object"));

	if (obj) {
		dev = obj->my_dev;
		yaffs_gross_lock(dev);
		yaffs_unstitch_obj(inode,obj);
		yaffs_gross_unlock(dev);
	}

}

/* delete is called when the link count is zero and the inode
 * is put (ie. nobody wants to know about it anymore, time to
 * delete the file).
 * NB Must call clear_inode()
 */
static void yaffs_delete_inode(struct inode *inode)
{
	yaffs_obj_t *obj = yaffs_InodeToObject(inode);
	yaffs_dev_t *dev;

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_delete_inode: ino %d, count %d %s\n"), (int)inode->i_ino,
		atomic_read(&inode->i_count),
		obj ? "object exists" : "null object"));

	if (obj) {
		dev = obj->my_dev;
		yaffs_gross_lock(dev);
		yaffs_del_obj(obj);
		yaffs_gross_unlock(dev);
	}
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 13))
	truncate_inode_pages(&inode->i_data, 0);
#endif
	clear_inode(inode);
}
#endif


#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
static int yaffs_file_flush(struct file *file, fl_owner_t id)
#else
static int yaffs_file_flush(struct file *file)
#endif
{
	yaffs_obj_t *obj = yaffs_dentry_to_obj(file->f_dentry);

	yaffs_dev_t *dev = obj->my_dev;

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_file_flush object %d (%s)\n"), obj->obj_id,
		obj->dirty ? "dirty" : "clean"));

	yaffs_gross_lock(dev);

	yaffs_flush_file(obj, 1, 0);

	yaffs_gross_unlock(dev);

	return 0;
}

static int yaffs_readpage_nolock(struct file *f, struct page *pg)
{
	/* Lifted from jffs2 */

	yaffs_obj_t *obj;
	unsigned char *pg_buf;
	int ret;

	yaffs_dev_t *dev;

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_readpage_nolock at %08x, size %08x\n"),
		(unsigned)(pg->index << PAGE_CACHE_SHIFT),
		(unsigned)PAGE_CACHE_SIZE));

	obj = yaffs_dentry_to_obj(f->f_dentry);

	dev = obj->my_dev;

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
	BUG_ON(!PageLocked(pg));
#else
	if (!PageLocked(pg))
		PAGE_BUG(pg);
#endif

	pg_buf = kmap(pg);
	/* FIXME: Can kmap fail? */

	yaffs_gross_lock(dev);

	ret = yaffs_file_rd(obj, pg_buf,
				pg->index << PAGE_CACHE_SHIFT,
				PAGE_CACHE_SIZE);

	yaffs_gross_unlock(dev);

	if (ret >= 0)
		ret = 0;

	if (ret) {
		ClearPageUptodate(pg);
		SetPageError(pg);
	} else {
		SetPageUptodate(pg);
		ClearPageError(pg);
	}

	flush_dcache_page(pg);
	kunmap(pg);

	T(YAFFS_TRACE_OS, (TSTR("yaffs_readpage_nolock done\n")));
	return ret;
}

static int yaffs_readpage_unlock(struct file *f, struct page *pg)
{
	int ret = yaffs_readpage_nolock(f, pg);
	UnlockPage(pg);
	return ret;
}

static int yaffs_readpage(struct file *f, struct page *pg)
{
	int ret;

	T(YAFFS_TRACE_OS, (TSTR("yaffs_readpage\n")));
	ret=yaffs_readpage_unlock(f, pg);
	T(YAFFS_TRACE_OS, (TSTR("yaffs_readpage done\n")));
	return ret;
}

/* writepage inspired by/stolen from smbfs */

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
static int yaffs_writepage(struct page *page, struct writeback_control *wbc)
#else
static int yaffs_writepage(struct page *page)
#endif
{
	yaffs_dev_t *dev;
	struct address_space *mapping = page->mapping;
	struct inode *inode;
	unsigned long end_index;
	char *buffer;
	yaffs_obj_t *obj;
	int nWritten = 0;
	unsigned n_bytes;
	loff_t i_size;

	if (!mapping)
		BUG();
	inode = mapping->host;
	if (!inode)
		BUG();
	i_size = i_size_read(inode);

	end_index = i_size >> PAGE_CACHE_SHIFT;

	if(page->index < end_index)
		n_bytes = PAGE_CACHE_SIZE;
	else {
		n_bytes = i_size & (PAGE_CACHE_SIZE -1);

		if (page->index > end_index || !n_bytes) {
			T(YAFFS_TRACE_OS,
				(TSTR("yaffs_writepage at %08x, inode size = %08x!!!\n"),
				(unsigned)(page->index << PAGE_CACHE_SHIFT),
				(unsigned)inode->i_size));
			T(YAFFS_TRACE_OS,
				(TSTR("                -> don't care!!\n")));

			zero_user_segment(page,0,PAGE_CACHE_SIZE);
			set_page_writeback(page);
			unlock_page(page);
			end_page_writeback(page);
			return 0;
		}
	}

	if(n_bytes != PAGE_CACHE_SIZE)
		zero_user_segment(page,n_bytes,PAGE_CACHE_SIZE);

	get_page(page);

	buffer = kmap(page);

	obj = yaffs_InodeToObject(inode);
	dev = obj->my_dev;
	yaffs_gross_lock(dev);

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_writepage at %08x, size %08x\n"),
		(unsigned)(page->index << PAGE_CACHE_SHIFT), n_bytes));
	T(YAFFS_TRACE_OS,
		(TSTR("writepag0: obj = %05x, ino = %05x\n"),
		(int)obj->variant.file_variant.file_size, (int)inode->i_size));

	nWritten = yaffs_wr_file(obj, buffer,
			page->index << PAGE_CACHE_SHIFT, n_bytes, 0);

	yaffs_touch_super(dev);

	T(YAFFS_TRACE_OS,
		(TSTR("writepag1: obj = %05x, ino = %05x\n"),
		(int)obj->variant.file_variant.file_size, (int)inode->i_size));

	yaffs_gross_unlock(dev);

	kunmap(page);
	set_page_writeback(page);
	unlock_page(page);
	end_page_writeback(page);
	put_page(page);

	return (nWritten == n_bytes) ? 0 : -ENOSPC;
}


#if (YAFFS_USE_WRITE_BEGIN_END > 0)
static int yaffs_write_begin(struct file *filp, struct address_space *mapping,
				loff_t pos, unsigned len, unsigned flags,
				struct page **pagep, void **fsdata)
{
	struct page *pg = NULL;
	pgoff_t index = pos >> PAGE_CACHE_SHIFT;

	int ret = 0;
	int space_held = 0;

	/* Get a page */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 28)
	pg = grab_cache_page_write_begin(mapping, index, flags);
#else
	pg = __grab_cache_page(mapping, index);
#endif

	*pagep = pg;
	if (!pg) {
		ret =  -ENOMEM;
		goto out;
	}
	T(YAFFS_TRACE_OS,
		(TSTR("start yaffs_write_begin index %d(%x) uptodate %d\n"),
		(int)index,(int)index,Page_Uptodate(pg) ? 1 : 0));

	/* Get fs space */
	space_held = yaffs_hold_space(filp);

	if (!space_held) {
		ret = -ENOSPC;
		goto out;
	}

	/* Update page if required */

	if (!Page_Uptodate(pg))
		ret = yaffs_readpage_nolock(filp, pg);

	if (ret)
		goto out;

	/* Happy path return */
	T(YAFFS_TRACE_OS, (TSTR("end yaffs_write_begin - ok\n")));

	return 0;

out:
	T(YAFFS_TRACE_OS,
		(TSTR("end yaffs_write_begin fail returning %d\n"), ret));
	if (space_held)
		yaffs_release_space(filp);
	if (pg) {
		unlock_page(pg);
		page_cache_release(pg);
	}
	return ret;
}

#else

static int yaffs_prepare_write(struct file *f, struct page *pg,
				unsigned offset, unsigned to)
{
	T(YAFFS_TRACE_OS, (TSTR("yaffs_prepair_write\n")));

	if (!Page_Uptodate(pg))
		return yaffs_readpage_nolock(f, pg);
	return 0;
}
#endif

#if (YAFFS_USE_WRITE_BEGIN_END > 0)
static int yaffs_write_end(struct file *filp, struct address_space *mapping,
				loff_t pos, unsigned len, unsigned copied,
				struct page *pg, void *fsdadata)
{
	int ret = 0;
	void *addr, *kva;
	uint32_t offset_into_page = pos & (PAGE_CACHE_SIZE - 1);

	kva = kmap(pg);
	addr = kva + offset_into_page;

	T(YAFFS_TRACE_OS,
		("yaffs_write_end addr %p pos %x n_bytes %d\n",
		addr,(unsigned)pos, copied));

	ret = yaffs_file_write(filp, addr, copied, &pos);

	if (ret != copied) {
		T(YAFFS_TRACE_OS,
			(TSTR("yaffs_write_end not same size ret %d  copied %d\n"),
			ret, copied));
		SetPageError(pg);
	} else {
		/* Nothing */
	}

	kunmap(pg);

	yaffs_release_space(filp);
	unlock_page(pg);
	page_cache_release(pg);
	return ret;
}
#else

static int yaffs_commit_write(struct file *f, struct page *pg, unsigned offset,
				unsigned to)
{
	void *addr, *kva;

	loff_t pos = (((loff_t) pg->index) << PAGE_CACHE_SHIFT) + offset;
	int n_bytes = to - offset;
	int nWritten;

	unsigned spos = pos;
	unsigned saddr;

	kva = kmap(pg);
	addr = kva + offset;

	saddr = (unsigned) addr;

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_commit_write addr %x pos %x n_bytes %d\n"),
		saddr, spos, n_bytes));

	nWritten = yaffs_file_write(f, addr, n_bytes, &pos);

	if (nWritten != n_bytes) {
		T(YAFFS_TRACE_OS,
			(TSTR("yaffs_commit_write not same size nWritten %d  n_bytes %d\n"),
			nWritten, n_bytes));
		SetPageError(pg);
	} else {
		/* Nothing */
	}

	kunmap(pg);

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_commit_write returning %d\n"),
		nWritten == n_bytes ? 0 : nWritten));

	return nWritten == n_bytes ? 0 : nWritten;
}
#endif


static void yaffs_fill_inode_from_obj(struct inode *inode, yaffs_obj_t *obj)
{
	if (inode && obj) {


		/* Check mode against the variant type and attempt to repair if broken. */
		__u32 mode = obj->yst_mode;
		switch (obj->variant_type) {
		case YAFFS_OBJECT_TYPE_FILE:
			if (!S_ISREG(mode)) {
				obj->yst_mode &= ~S_IFMT;
				obj->yst_mode |= S_IFREG;
			}

			break;
		case YAFFS_OBJECT_TYPE_SYMLINK:
			if (!S_ISLNK(mode)) {
				obj->yst_mode &= ~S_IFMT;
				obj->yst_mode |= S_IFLNK;
			}

			break;
		case YAFFS_OBJECT_TYPE_DIRECTORY:
			if (!S_ISDIR(mode)) {
				obj->yst_mode &= ~S_IFMT;
				obj->yst_mode |= S_IFDIR;
			}

			break;
		case YAFFS_OBJECT_TYPE_UNKNOWN:
		case YAFFS_OBJECT_TYPE_HARDLINK:
		case YAFFS_OBJECT_TYPE_SPECIAL:
		default:
			/* TODO? */
			break;
		}

		inode->i_flags |= S_NOATIME;

		inode->i_ino = obj->obj_id;
		inode->i_mode = obj->yst_mode;
		inode->i_uid = obj->yst_uid;
		inode->i_gid = obj->yst_gid;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 19))
		inode->i_blksize = inode->i_sb->s_blocksize;
#endif
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))

		inode->i_rdev = old_decode_dev(obj->yst_rdev);
		inode->i_atime.tv_sec = (time_t) (obj->yst_atime);
		inode->i_atime.tv_nsec = 0;
		inode->i_mtime.tv_sec = (time_t) obj->yst_mtime;
		inode->i_mtime.tv_nsec = 0;
		inode->i_ctime.tv_sec = (time_t) obj->yst_ctime;
		inode->i_ctime.tv_nsec = 0;
#else
		inode->i_rdev = obj->yst_rdev;
		inode->i_atime = obj->yst_atime;
		inode->i_mtime = obj->yst_mtime;
		inode->i_ctime = obj->yst_ctime;
#endif
		inode->i_size = yaffs_get_obj_length(obj);
		inode->i_blocks = (inode->i_size + 511) >> 9;

		inode->i_nlink = yaffs_get_obj_link_count(obj);

		T(YAFFS_TRACE_OS,
			(TSTR("yaffs_fill_inode mode %x uid %d gid %d size %d count %d\n"),
			inode->i_mode, inode->i_uid, inode->i_gid,
			(int)inode->i_size, atomic_read(&inode->i_count)));

		switch (obj->yst_mode & S_IFMT) {
		default:	/* fifo, device or socket */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
			init_special_inode(inode, obj->yst_mode,
					old_decode_dev(obj->yst_rdev));
#else
			init_special_inode(inode, obj->yst_mode,
					(dev_t) (obj->yst_rdev));
#endif
			break;
		case S_IFREG:	/* file */
			inode->i_op = &yaffs_file_inode_operations;
			inode->i_fop = &yaffs_file_operations;
			inode->i_mapping->a_ops =
				&yaffs_file_address_operations;
			break;
		case S_IFDIR:	/* directory */
			inode->i_op = &yaffs_dir_inode_operations;
			inode->i_fop = &yaffs_dir_operations;
			break;
		case S_IFLNK:	/* symlink */
			inode->i_op = &yaffs_symlink_inode_operations;
			break;
		}

		yaffs_InodeToObjectLV(inode) = obj;

		obj->my_inode = inode;

	} else {
		T(YAFFS_TRACE_OS,
			(TSTR("yaffs_FileInode invalid parameters\n")));
	}

}

struct inode *yaffs_get_inode(struct super_block *sb, int mode, int dev,
				yaffs_obj_t *obj)
{
	struct inode *inode;

	if (!sb) {
		T(YAFFS_TRACE_OS,
			(TSTR("yaffs_get_inode for NULL super_block!!\n")));
		return NULL;

	}

	if (!obj) {
		T(YAFFS_TRACE_OS,
			(TSTR("yaffs_get_inode for NULL object!!\n")));
		return NULL;

	}

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_get_inode for object %d\n"), obj->obj_id));

	inode = Y_IGET(sb, obj->obj_id);
	if (IS_ERR(inode))
		return NULL;

	/* NB Side effect: iget calls back to yaffs_read_inode(). */
	/* iget also increments the inode's i_count */
	/* NB You can't be holding grossLock or deadlock will happen! */

	return inode;
}

static ssize_t yaffs_file_write(struct file *f, const char *buf, size_t n,
				loff_t *pos)
{
	yaffs_obj_t *obj;
	int nWritten, ipos;
	struct inode *inode;
	yaffs_dev_t *dev;

	obj = yaffs_dentry_to_obj(f->f_dentry);

	dev = obj->my_dev;

	yaffs_gross_lock(dev);

	inode = f->f_dentry->d_inode;

	if (!S_ISBLK(inode->i_mode) && f->f_flags & O_APPEND)
		ipos = inode->i_size;
	else
		ipos = *pos;

	if (!obj)
		T(YAFFS_TRACE_OS,
			(TSTR("yaffs_file_write: hey obj is null!\n")));
	else
		T(YAFFS_TRACE_OS,
			(TSTR("yaffs_file_write about to write writing %u(%x) bytes"
			"to object %d at %d(%x)\n"),
			(unsigned) n, (unsigned) n, obj->obj_id, ipos,ipos));

	nWritten = yaffs_wr_file(obj, buf, ipos, n, 0);

	yaffs_touch_super(dev);

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_file_write: %d(%x) bytes written\n"),
		(unsigned )n,(unsigned)n));

	if (nWritten > 0) {
		ipos += nWritten;
		*pos = ipos;
		if (ipos > inode->i_size) {
			inode->i_size = ipos;
			inode->i_blocks = (ipos + 511) >> 9;

			T(YAFFS_TRACE_OS,
				(TSTR("yaffs_file_write size updated to %d bytes, "
				"%d blocks\n"),
				ipos, (int)(inode->i_blocks)));
		}

	}
	yaffs_gross_unlock(dev);
	return (nWritten == 0) && (n > 0) ? -ENOSPC : nWritten;
}

/* Space holding and freeing is done to ensure we have space available for write_begin/end */
/* For now we just assume few parallel writes and check against a small number. */
/* Todo: need to do this with a counter to handle parallel reads better */

static ssize_t yaffs_hold_space(struct file *f)
{
	yaffs_obj_t *obj;
	yaffs_dev_t *dev;

	int n_free_chunks;


	obj = yaffs_dentry_to_obj(f->f_dentry);

	dev = obj->my_dev;

	yaffs_gross_lock(dev);

	n_free_chunks = yaffs_get_n_free_chunks(dev);

	yaffs_gross_unlock(dev);

	return (n_free_chunks > 20) ? 1 : 0;
}

static void yaffs_release_space(struct file *f)
{
	yaffs_obj_t *obj;
	yaffs_dev_t *dev;


	obj = yaffs_dentry_to_obj(f->f_dentry);

	dev = obj->my_dev;

	yaffs_gross_lock(dev);


	yaffs_gross_unlock(dev);
}


static loff_t yaffs_dir_llseek(struct file *file, loff_t offset, int origin)
{
	long long retval;

	lock_kernel();

	switch (origin){
	case 2:
		offset += i_size_read(file->f_path.dentry->d_inode);
		break;
	case 1:
		offset += file->f_pos;
	}
	retval = -EINVAL;

	if (offset >= 0){
		if (offset != file->f_pos)
			file->f_pos = offset;

		retval = offset;
	}
	unlock_kernel();
	return retval;
}


static int yaffs_readdir(struct file *f, void *dirent, filldir_t filldir)
{
	yaffs_obj_t *obj;
	yaffs_dev_t *dev;
        struct yaffs_SearchContext *sc;
	struct inode *inode = f->f_dentry->d_inode;
	unsigned long offset, curoffs;
	yaffs_obj_t *l;
        int retVal = 0;

	char name[YAFFS_MAX_NAME_LENGTH + 1];

	obj = yaffs_dentry_to_obj(f->f_dentry);
	dev = obj->my_dev;

	yaffs_gross_lock(dev);

	yaffs_dev_to_lc(dev)->readdirProcess = current;

	offset = f->f_pos;

        sc = yaffs_NewSearch(obj);
        if(!sc){
                retVal = -ENOMEM;
                goto out;
        }

	T(YAFFS_TRACE_OS, (TSTR("yaffs_readdir: starting at %d\n"), (int)offset));

	if (offset == 0) {
		T(YAFFS_TRACE_OS,
			(TSTR("yaffs_readdir: entry . ino %d \n"),
			(int)inode->i_ino));
		yaffs_gross_unlock(dev);
		if (filldir(dirent, ".", 1, offset, inode->i_ino, DT_DIR) < 0){
			yaffs_gross_lock(dev);
			goto out;
		}
		yaffs_gross_lock(dev);
		offset++;
		f->f_pos++;
	}
	if (offset == 1) {
		T(YAFFS_TRACE_OS,
			(TSTR("yaffs_readdir: entry .. ino %d \n"),
			(int)f->f_dentry->d_parent->d_inode->i_ino));
		yaffs_gross_unlock(dev);
		if (filldir(dirent, "..", 2, offset,
			f->f_dentry->d_parent->d_inode->i_ino, DT_DIR) < 0){
			yaffs_gross_lock(dev);
			goto out;
		}
		yaffs_gross_lock(dev);
		offset++;
		f->f_pos++;
	}

	curoffs = 1;

	/* If the directory has changed since the open or last call to
	   readdir, rewind to after the 2 canned entries. */
	if (f->f_version != inode->i_version) {
		offset = 2;
		f->f_pos = offset;
		f->f_version = inode->i_version;
	}

	while(sc->nextReturn){
		curoffs++;
                l = sc->nextReturn;
		if (curoffs >= offset) {
                        int this_inode = yaffs_get_obj_inode(l);
                        int this_type = yaffs_get_obj_type(l);

			yaffs_get_obj_name(l, name,
					    YAFFS_MAX_NAME_LENGTH + 1);
			T(YAFFS_TRACE_OS,
			  (TSTR("yaffs_readdir: %s inode %d\n"),
			  name, yaffs_get_obj_inode(l)));

                        yaffs_gross_unlock(dev);

			if (filldir(dirent,
					name,
					strlen(name),
					offset,
					this_inode,
					this_type) < 0){
				yaffs_gross_lock(dev);
				goto out;
			}

                        yaffs_gross_lock(dev);

			offset++;
			f->f_pos++;
		}
                yaffs_search_advance(sc);
	}

out:
	yaffs_search_end(sc);
	yaffs_dev_to_lc(dev)->readdirProcess = NULL;
	yaffs_gross_unlock(dev);

	return retVal;
}



/*
 * File creation. Allocate an inode, and we're done..
 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 29)
#define YCRED(x) x
#else
#define YCRED(x) (x->cred)
#endif

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
static int yaffs_mknod(struct inode *dir, struct dentry *dentry, int mode,
			dev_t rdev)
#else
static int yaffs_mknod(struct inode *dir, struct dentry *dentry, int mode,
			int rdev)
#endif
{
	struct inode *inode;

	yaffs_obj_t *obj = NULL;
	yaffs_dev_t *dev;

	yaffs_obj_t *parent = yaffs_InodeToObject(dir);

	int error = -ENOSPC;
	uid_t uid = YCRED(current)->fsuid;
	gid_t gid = (dir->i_mode & S_ISGID) ? dir->i_gid : YCRED(current)->fsgid;

	if ((dir->i_mode & S_ISGID) && S_ISDIR(mode))
		mode |= S_ISGID;

	if (parent) {
		T(YAFFS_TRACE_OS,
			(TSTR("yaffs_mknod: parent object %d type %d\n"),
			parent->obj_id, parent->variant_type));
	} else {
		T(YAFFS_TRACE_OS,
			(TSTR("yaffs_mknod: could not get parent object\n")));
		return -EPERM;
	}

	T(YAFFS_TRACE_OS, (TSTR("yaffs_mknod: making oject for %s, "
			"mode %x dev %x\n"),
			dentry->d_name.name, mode, rdev));

	dev = parent->my_dev;

	yaffs_gross_lock(dev);

	switch (mode & S_IFMT) {
	default:
		/* Special (socket, fifo, device...) */
		T(YAFFS_TRACE_OS, (TSTR("yaffs_mknod: making special\n")));
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
		obj = yaffs_create_special(parent, dentry->d_name.name, mode, uid,
				gid, old_encode_dev(rdev));
#else
		obj = yaffs_create_special(parent, dentry->d_name.name, mode, uid,
				gid, rdev);
#endif
		break;
	case S_IFREG:		/* file          */
		T(YAFFS_TRACE_OS, (TSTR("yaffs_mknod: making file\n")));
		obj = yaffs_create_file(parent, dentry->d_name.name, mode, uid,
				gid);
		break;
	case S_IFDIR:		/* directory */
		T(YAFFS_TRACE_OS,
			(TSTR("yaffs_mknod: making directory\n")));
		obj = yaffs_create_dir(parent, dentry->d_name.name, mode,
					uid, gid);
		break;
	case S_IFLNK:		/* symlink */
		T(YAFFS_TRACE_OS, (TSTR("yaffs_mknod: making symlink\n")));
		obj = NULL;	/* Do we ever get here? */
		break;
	}

	/* Can not call yaffs_get_inode() with gross lock held */
	yaffs_gross_unlock(dev);

	if (obj) {
		inode = yaffs_get_inode(dir->i_sb, mode, rdev, obj);
		d_instantiate(dentry, inode);
		update_dir_time(dir);
		T(YAFFS_TRACE_OS,
			(TSTR("yaffs_mknod created object %d count = %d\n"),
			obj->obj_id, atomic_read(&inode->i_count)));
		error = 0;
		yaffs_fill_inode_from_obj(dir,parent);
	} else {
		T(YAFFS_TRACE_OS,
			(TSTR("yaffs_mknod failed making object\n")));
		error = -ENOMEM;
	}

	return error;
}

static int yaffs_mkdir(struct inode *dir, struct dentry *dentry, int mode)
{
	int retVal;
	T(YAFFS_TRACE_OS, (TSTR("yaffs_mkdir\n")));
	retVal = yaffs_mknod(dir, dentry, mode | S_IFDIR, 0);
	return retVal;
}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
static int yaffs_create(struct inode *dir, struct dentry *dentry, int mode,
			struct nameidata *n)
#else
static int yaffs_create(struct inode *dir, struct dentry *dentry, int mode)
#endif
{
	T(YAFFS_TRACE_OS,(TSTR("yaffs_create\n")));
	return yaffs_mknod(dir, dentry, mode | S_IFREG, 0);
}

static int yaffs_unlink(struct inode *dir, struct dentry *dentry)
{
	int retVal;

	yaffs_dev_t *dev;
	yaffs_obj_t *obj;

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_unlink %d:%s\n"),
		(int)(dir->i_ino),
		dentry->d_name.name));
	obj = yaffs_InodeToObject(dir);
	dev = obj->my_dev;

	yaffs_gross_lock(dev);

	retVal = yaffs_unlinker(obj, dentry->d_name.name);

	if (retVal == YAFFS_OK) {
		dentry->d_inode->i_nlink--;
		dir->i_version++;
		yaffs_gross_unlock(dev);
		mark_inode_dirty(dentry->d_inode);
		update_dir_time(dir);
		return 0;
	}
	yaffs_gross_unlock(dev);
	return -ENOTEMPTY;
}

/*
 * Create a link...
 */
static int yaffs_link(struct dentry *old_dentry, struct inode *dir,
			struct dentry *dentry)
{
	struct inode *inode = old_dentry->d_inode;
	yaffs_obj_t *obj = NULL;
	yaffs_obj_t *link = NULL;
	yaffs_dev_t *dev;

	T(YAFFS_TRACE_OS, (TSTR("yaffs_link\n")));

	obj = yaffs_InodeToObject(inode);
	dev = obj->my_dev;

	yaffs_gross_lock(dev);

	if (!S_ISDIR(inode->i_mode))		/* Don't link directories */
		link = yaffs_link_obj(yaffs_InodeToObject(dir), dentry->d_name.name,
			obj);

	if (link) {
		old_dentry->d_inode->i_nlink = yaffs_get_obj_link_count(obj);
		d_instantiate(dentry, old_dentry->d_inode);
		atomic_inc(&old_dentry->d_inode->i_count);
		T(YAFFS_TRACE_OS,
			(TSTR("yaffs_link link count %d i_count %d\n"),
			old_dentry->d_inode->i_nlink,
			atomic_read(&old_dentry->d_inode->i_count)));
	}

	yaffs_gross_unlock(dev);

	if (link){
		update_dir_time(dir);
		return 0;
	}

	return -EPERM;
}

static int yaffs_symlink(struct inode *dir, struct dentry *dentry,
				const char *symname)
{
	yaffs_obj_t *obj;
	yaffs_dev_t *dev;
	uid_t uid = YCRED(current)->fsuid;
	gid_t gid = (dir->i_mode & S_ISGID) ? dir->i_gid : YCRED(current)->fsgid;

	T(YAFFS_TRACE_OS, (TSTR("yaffs_symlink\n")));

	dev = yaffs_InodeToObject(dir)->my_dev;
	yaffs_gross_lock(dev);
	obj = yaffs_create_symlink(yaffs_InodeToObject(dir), dentry->d_name.name,
				S_IFLNK | S_IRWXUGO, uid, gid, symname);
	yaffs_gross_unlock(dev);

	if (obj) {
		struct inode *inode;

		inode = yaffs_get_inode(dir->i_sb, obj->yst_mode, 0, obj);
		d_instantiate(dentry, inode);
		update_dir_time(dir);
		T(YAFFS_TRACE_OS, (TSTR("symlink created OK\n")));
		return 0;
	} else {
		T(YAFFS_TRACE_OS, (TSTR("symlink not created\n")));
	}

	return -ENOMEM;
}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 34))
static int yaffs_sync_object(struct file *file, int datasync)
#else
static int yaffs_sync_object(struct file *file, struct dentry *dentry,
				int datasync)
#endif
{

	yaffs_obj_t *obj;
	yaffs_dev_t *dev;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 34))
	struct dentry *dentry = file->f_path.dentry;
#endif

	obj = yaffs_dentry_to_obj(dentry);

	dev = obj->my_dev;

	T(YAFFS_TRACE_OS | YAFFS_TRACE_SYNC,
		(TSTR("yaffs_sync_object\n")));
	yaffs_gross_lock(dev);
	yaffs_flush_file(obj, 1, datasync);
	yaffs_gross_unlock(dev);
	return 0;
}

/*
 * The VFS layer already does all the dentry stuff for rename.
 *
 * NB: POSIX says you can rename an object over an old object of the same name
 */
static int yaffs_rename(struct inode *old_dir, struct dentry *old_dentry,
			struct inode *new_dir, struct dentry *new_dentry)
{
	yaffs_dev_t *dev;
	int retVal = YAFFS_FAIL;
	yaffs_obj_t *target;

	T(YAFFS_TRACE_OS, (TSTR("yaffs_rename\n")));
	dev = yaffs_InodeToObject(old_dir)->my_dev;

	yaffs_gross_lock(dev);

	/* Check if the target is an existing directory that is not empty. */
	target = yaffs_find_by_name(yaffs_InodeToObject(new_dir),
				new_dentry->d_name.name);



	if (target && target->variant_type == YAFFS_OBJECT_TYPE_DIRECTORY &&
		!ylist_empty(&target->variant.dir_variant.children)) {

		T(YAFFS_TRACE_OS, (TSTR("target is non-empty dir\n")));

		retVal = YAFFS_FAIL;
	} else {
		/* Now does unlinking internally using shadowing mechanism */
		T(YAFFS_TRACE_OS, (TSTR("calling yaffs_rename_obj\n")));

		retVal = yaffs_rename_obj(yaffs_InodeToObject(old_dir),
				old_dentry->d_name.name,
				yaffs_InodeToObject(new_dir),
				new_dentry->d_name.name);
	}
	yaffs_gross_unlock(dev);

	if (retVal == YAFFS_OK) {
		if (target) {
			new_dentry->d_inode->i_nlink--;
			mark_inode_dirty(new_dentry->d_inode);
		}
		
		update_dir_time(old_dir);
		if(old_dir != new_dir)
			update_dir_time(new_dir);
		return 0;
	} else {
		return -ENOTEMPTY;
	}
}

static int yaffs_setattr(struct dentry *dentry, struct iattr *attr)
{
	struct inode *inode = dentry->d_inode;
	int error = 0;
	yaffs_dev_t *dev;

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_setattr of object %d\n"),
		yaffs_InodeToObject(inode)->obj_id));

	/* Fail if a requested resize >= 2GB */		
	if (attr->ia_valid & ATTR_SIZE &&
		(attr->ia_size >> 31))
		error = -EINVAL;

	if (error == 0)
		error = inode_change_ok(inode, attr);
	if (error == 0) {
		int result;
		if (!error){
			error = yaffs_vfs_setattr(inode, attr);
			T(YAFFS_TRACE_OS,(TSTR("inode_setattr called\n")));
			if (attr->ia_valid & ATTR_SIZE){
                        	yaffs_vfs_setsize(inode,attr->ia_size);
                        	inode->i_blocks = (inode->i_size + 511) >> 9;
			}
		}
		dev = yaffs_InodeToObject(inode)->my_dev;
		if (attr->ia_valid & ATTR_SIZE){
			T(YAFFS_TRACE_OS,(TSTR("resize to %d(%x)\n"),
				(int)(attr->ia_size),(int)(attr->ia_size)));
		}
		yaffs_gross_lock(dev);
		result = yaffs_set_attribs(yaffs_InodeToObject(inode), attr);
		if(result == YAFFS_OK) {
			error = 0;
		} else {
			error = -EPERM;
		}
		yaffs_gross_unlock(dev);

	}

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_setattr done returning %d\n"),error));

	return error;
}

#ifdef CONFIG_YAFFS_XATTR
int yaffs_setxattr(struct dentry *dentry, const char *name,
			const void *value, size_t size, int flags)
{
	struct inode *inode = dentry->d_inode;
	int error = 0;
	yaffs_dev_t *dev;
	yaffs_obj_t *obj = yaffs_InodeToObject(inode);

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_setxattr of object %d\n"),
		obj->obj_id));


	if (error == 0) {
		int result;
		dev = obj->my_dev;
		yaffs_gross_lock(dev);
		result = yaffs_set_xattrib(obj, name, value, size, flags);
		if(result == YAFFS_OK)
			error = 0;
		else if(result < 0)
			error = result;
		yaffs_gross_unlock(dev);

	}
	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_setxattr done returning %d\n"),error));

	return error;
}


ssize_t yaffs_getxattr(struct dentry *dentry, const char *name, void *buff,
			size_t size)
{
	struct inode *inode = dentry->d_inode;
	int error = 0;
	yaffs_dev_t *dev;
	yaffs_obj_t *obj = yaffs_InodeToObject(inode);

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_getxattr \"%s\" from object %d\n"),
		name, obj->obj_id));

	if (error == 0) {
		dev = obj->my_dev;
		yaffs_gross_lock(dev);
		error = yaffs_get_xattrib(obj, name, buff, size);
		yaffs_gross_unlock(dev);

	}
	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_getxattr done returning %d\n"),error));

	return error;
}

int yaffs_removexattr(struct dentry *dentry, const char *name)
{
	struct inode *inode = dentry->d_inode;
	int error = 0;
	yaffs_dev_t *dev;
	yaffs_obj_t *obj = yaffs_InodeToObject(inode);

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_removexattr of object %d\n"),
		obj->obj_id));


	if (error == 0) {
		int result;
		dev = obj->my_dev;
		yaffs_gross_lock(dev);
		result = yaffs_remove_xattrib(obj, name);
		if(result == YAFFS_OK)
			error = 0;
		else if(result < 0)
			error = result;
		yaffs_gross_unlock(dev);

	}
	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_removexattr done returning %d\n"),error));

	return error;
}

ssize_t yaffs_listxattr(struct dentry *dentry, char *buff, size_t size)
{
	struct inode *inode = dentry->d_inode;
	int error = 0;
	yaffs_dev_t *dev;
	yaffs_obj_t *obj = yaffs_InodeToObject(inode);

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_listxattr of object %d\n"),
		obj->obj_id));


	if (error == 0) {
		dev = obj->my_dev;
		yaffs_gross_lock(dev);
		error = yaffs_list_xattrib(obj, buff, size);
		yaffs_gross_unlock(dev);

	}
	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_listxattr done returning %d\n"),error));

	return error;
}

#endif


#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
static int yaffs_statfs(struct dentry *dentry, struct kstatfs *buf)
{
	yaffs_dev_t *dev = yaffs_dentry_to_obj(dentry)->my_dev;
	struct super_block *sb = dentry->d_sb;
#elif (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
static int yaffs_statfs(struct super_block *sb, struct kstatfs *buf)
{
	yaffs_dev_t *dev = yaffs_SuperToDevice(sb);
#else
static int yaffs_statfs(struct super_block *sb, struct statfs *buf)
{
	yaffs_dev_t *dev = yaffs_SuperToDevice(sb);
#endif

	T(YAFFS_TRACE_OS, (TSTR("yaffs_statfs\n")));

	yaffs_gross_lock(dev);

	buf->f_type = YAFFS_MAGIC;
	buf->f_bsize = sb->s_blocksize;
	buf->f_namelen = 255;

	if (dev->data_bytes_per_chunk & (dev->data_bytes_per_chunk - 1)) {
		/* Do this if chunk size is not a power of 2 */

		uint64_t bytesInDev;
		uint64_t bytesFree;

		bytesInDev = ((uint64_t)((dev->param.end_block - dev->param.start_block + 1))) *
			((uint64_t)(dev->param.chunks_per_block * dev->data_bytes_per_chunk));

		do_div(bytesInDev, sb->s_blocksize); /* bytesInDev becomes the number of blocks */
		buf->f_blocks = bytesInDev;

		bytesFree  = ((uint64_t)(yaffs_get_n_free_chunks(dev))) *
			((uint64_t)(dev->data_bytes_per_chunk));

		do_div(bytesFree, sb->s_blocksize);

		buf->f_bfree = bytesFree;

	} else if (sb->s_blocksize > dev->data_bytes_per_chunk) {

		buf->f_blocks =
			(dev->param.end_block - dev->param.start_block + 1) *
			dev->param.chunks_per_block /
			(sb->s_blocksize / dev->data_bytes_per_chunk);
		buf->f_bfree =
			yaffs_get_n_free_chunks(dev) /
			(sb->s_blocksize / dev->data_bytes_per_chunk);
	} else {
		buf->f_blocks =
			(dev->param.end_block - dev->param.start_block + 1) *
			dev->param.chunks_per_block *
			(dev->data_bytes_per_chunk / sb->s_blocksize);

		buf->f_bfree =
			yaffs_get_n_free_chunks(dev) *
			(dev->data_bytes_per_chunk / sb->s_blocksize);
	}

	buf->f_files = 0;
	buf->f_ffree = 0;
	buf->f_bavail = buf->f_bfree;

	yaffs_gross_unlock(dev);
	return 0;
}



static void yaffs_flush_inodes(struct super_block *sb)
{
	struct inode *iptr;
	yaffs_obj_t *obj;
	
	list_for_each_entry(iptr,&sb->s_inodes, i_sb_list){
		obj = yaffs_InodeToObject(iptr);
		if(obj){
			T(YAFFS_TRACE_OS, (TSTR("flushing obj %d\n"),
				obj->obj_id));
			yaffs_flush_file(obj,1,0);
		}
	}
}


static void yaffs_flush_super(struct super_block *sb, int do_checkpoint)
{
	yaffs_dev_t *dev = yaffs_SuperToDevice(sb);	
	if(!dev)
		return;
	
	yaffs_flush_inodes(sb);
	yaffs_update_dirty_dirs(dev);
	yaffs_flush_whole_cache(dev);
	if(do_checkpoint)
		yaffs_checkpoint_save(dev);
}


static unsigned yaffs_bg_gc_urgency(yaffs_dev_t *dev)
{
	unsigned erasedChunks = dev->n_erased_blocks * dev->param.chunks_per_block;
	struct yaffs_LinuxContext *context = yaffs_dev_to_lc(dev);
	unsigned scatteredFree = 0; /* Free chunks not in an erased block */

	if(erasedChunks < dev->n_free_chunks)
		scatteredFree = (dev->n_free_chunks - erasedChunks);

	if(!context->bgRunning)
		return 0;
	else if(scatteredFree < (dev->param.chunks_per_block * 2))
		return 0;
	else if(erasedChunks > dev->n_free_chunks/2)
		return 0;
	else if(erasedChunks > dev->n_free_chunks/4)
		return 1;
	else
		return 2;
}

static int yaffs_do_sync_fs(struct super_block *sb,
				int request_checkpoint)
{

	yaffs_dev_t *dev = yaffs_SuperToDevice(sb);
	unsigned int oneshot_checkpoint = (yaffs_auto_checkpoint & 4);
	unsigned gc_urgent = yaffs_bg_gc_urgency(dev);
	int do_checkpoint;

	T(YAFFS_TRACE_OS | YAFFS_TRACE_SYNC | YAFFS_TRACE_BACKGROUND,
		(TSTR("yaffs_do_sync_fs: gc-urgency %d %s %s%s\n"),
		gc_urgent,
		sb->s_dirt ? "dirty" : "clean",
		request_checkpoint ? "checkpoint requested" : "no checkpoint",
		oneshot_checkpoint ? " one-shot" : "" ));

	yaffs_gross_lock(dev);
	do_checkpoint = ((request_checkpoint && !gc_urgent) ||
			oneshot_checkpoint) &&
			!dev->is_checkpointed;

	if (sb->s_dirt || do_checkpoint) {
		yaffs_flush_super(sb, !dev->is_checkpointed && do_checkpoint);
		sb->s_dirt = 0;
		if(oneshot_checkpoint)
			yaffs_auto_checkpoint &= ~4;
	}
	yaffs_gross_unlock(dev);

	return 0;
}

/*
 * yaffs background thread functions .
 * yaffs_bg_thread_fn() the thread function
 * yaffs_bg_start() launches the background thread.
 * yaffs_bg_stop() cleans up the background thread.
 *
 * NB: 
 * The thread should only run after the yaffs is initialised
 * The thread should be stopped before yaffs is unmounted.
 * The thread should not do any writing while the fs is in read only.
 */

#ifdef YAFFS_COMPILE_BACKGROUND

void yaffs_background_waker(unsigned long data)
{
	wake_up_process((struct task_struct *)data);
}

static int yaffs_bg_thread_fn(void *data)
{
	yaffs_dev_t *dev = (yaffs_dev_t *)data;
	struct yaffs_LinuxContext *context = yaffs_dev_to_lc(dev);
	unsigned long now = jiffies;
	unsigned long next_dir_update = now;
	unsigned long next_gc = now;
	unsigned long expires;
	unsigned int urgency;

	int gcResult;
	struct timer_list timer;

	T(YAFFS_TRACE_BACKGROUND,
		(TSTR("yaffs_background starting for dev %p\n"),
		(void *)dev));

#ifdef YAFFS_COMPILE_FREEZER
	set_freezable();
#endif
	while(context->bgRunning){
		T(YAFFS_TRACE_BACKGROUND,
			(TSTR("yaffs_background\n")));

		if(kthread_should_stop())
			break;

#ifdef YAFFS_COMPILE_FREEZER
		if(try_to_freeze())
			continue;
#endif
		yaffs_gross_lock(dev);

		now = jiffies;

		if(time_after(now, next_dir_update) && yaffs_bg_enable){
			yaffs_update_dirty_dirs(dev);
			next_dir_update = now + HZ;
		}

		if(time_after(now,next_gc) && yaffs_bg_enable){
			if(!dev->is_checkpointed){
				urgency = yaffs_bg_gc_urgency(dev);
				gcResult = yaffs_bg_gc(dev, urgency);
				if(urgency > 1)
					next_gc = now + HZ/20+1;
				else if(urgency > 0)
					next_gc = now + HZ/10+1;
				else
					next_gc = now + HZ * 2;
			} else /*
				* gc not running so set to next_dir_update
				* to cut down on wake ups
				*/
				next_gc = next_dir_update;
		}
		yaffs_gross_unlock(dev);
#if 1
		expires = next_dir_update;
		if (time_before(next_gc,expires))
			expires = next_gc;
		if(time_before(expires,now))
			expires = now + HZ;

		Y_INIT_TIMER(&timer);
		timer.expires = expires+1;
		timer.data = (unsigned long) current;
		timer.function = yaffs_background_waker;

                set_current_state(TASK_INTERRUPTIBLE);
		add_timer(&timer);
		schedule();
		del_timer_sync(&timer);
#else
		msleep(10);
#endif
	}

	return 0;
}

static int yaffs_bg_start(yaffs_dev_t *dev)
{
	int retval = 0;
	struct yaffs_LinuxContext *context = yaffs_dev_to_lc(dev);

	if(dev->read_only)
		return -1;

	context->bgRunning = 1;

	context->bgThread = kthread_run(yaffs_bg_thread_fn,
	                        (void *)dev,"yaffs-bg-%d",context->mount_id);

	if(IS_ERR(context->bgThread)){
		retval = PTR_ERR(context->bgThread);
		context->bgThread = NULL;
		context->bgRunning = 0;
	}
	return retval;
}

static void yaffs_bg_stop(yaffs_dev_t *dev)
{
	struct yaffs_LinuxContext *ctxt = yaffs_dev_to_lc(dev);

	ctxt->bgRunning = 0;

	if( ctxt->bgThread){
		kthread_stop(ctxt->bgThread);
		ctxt->bgThread = NULL;
	}
}
#else
static int yaffs_bg_thread_fn(void *data)
{
	return 0;
}

static int yaffs_bg_start(yaffs_dev_t *dev)
{
	return 0;
}

static void yaffs_bg_stop(yaffs_dev_t *dev)
{
}
#endif


#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
static void yaffs_write_super(struct super_block *sb)
#else
static int yaffs_write_super(struct super_block *sb)
#endif
{
	unsigned request_checkpoint = (yaffs_auto_checkpoint >= 2);

	T(YAFFS_TRACE_OS | YAFFS_TRACE_SYNC | YAFFS_TRACE_BACKGROUND,
		(TSTR("yaffs_write_super%s\n"),
		request_checkpoint ? " checkpt" : ""));

	yaffs_do_sync_fs(sb, request_checkpoint);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 18))
	return 0;
#endif
}


#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
static int yaffs_sync_fs(struct super_block *sb, int wait)
#else
static int yaffs_sync_fs(struct super_block *sb)
#endif
{
	unsigned request_checkpoint = (yaffs_auto_checkpoint >= 1);

	T(YAFFS_TRACE_OS | YAFFS_TRACE_SYNC,
		(TSTR("yaffs_sync_fs%s\n"),
		request_checkpoint ? " checkpt" : ""));

	yaffs_do_sync_fs(sb, request_checkpoint);

	return 0;
}

#ifdef YAFFS_USE_OWN_IGET

static struct inode *yaffs_iget(struct super_block *sb, unsigned long ino)
{
	struct inode *inode;
	yaffs_obj_t *obj;
	yaffs_dev_t *dev = yaffs_SuperToDevice(sb);

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_iget for %lu\n"), ino));

	inode = iget_locked(sb, ino);
	if (!inode)
		return ERR_PTR(-ENOMEM);
	if (!(inode->i_state & I_NEW))
		return inode;

	/* NB This is called as a side effect of other functions, but
	 * we had to release the lock to prevent deadlocks, so
	 * need to lock again.
	 */

	yaffs_gross_lock(dev);

	obj = yaffs_find_by_number(dev, inode->i_ino);

	yaffs_fill_inode_from_obj(inode, obj);

	yaffs_gross_unlock(dev);

	unlock_new_inode(inode);
	return inode;
}

#else

static void yaffs_read_inode(struct inode *inode)
{
	/* NB This is called as a side effect of other functions, but
	 * we had to release the lock to prevent deadlocks, so
	 * need to lock again.
	 */

	yaffs_obj_t *obj;
	yaffs_dev_t *dev = yaffs_SuperToDevice(inode->i_sb);

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_read_inode for %d\n"), (int)inode->i_ino));

	if(current != yaffs_dev_to_lc(dev)->readdirProcess)
		yaffs_gross_lock(dev);

	obj = yaffs_find_by_number(dev, inode->i_ino);

	yaffs_fill_inode_from_obj(inode, obj);

	if(current != yaffs_dev_to_lc(dev)->readdirProcess)
		yaffs_gross_unlock(dev);
}

#endif

static YLIST_HEAD(yaffs_context_list);
struct semaphore yaffs_context_lock;

static void yaffs_put_super(struct super_block *sb)
{
	yaffs_dev_t *dev = yaffs_SuperToDevice(sb);

	T(YAFFS_TRACE_OS, (TSTR("yaffs_put_super\n")));

	T(YAFFS_TRACE_OS | YAFFS_TRACE_BACKGROUND,
		(TSTR("Shutting down yaffs background thread\n")));
	yaffs_bg_stop(dev);
	T(YAFFS_TRACE_OS | YAFFS_TRACE_BACKGROUND,
		(TSTR("yaffs background thread shut down\n")));

	yaffs_gross_lock(dev);

	yaffs_flush_super(sb,1);

	if (yaffs_dev_to_lc(dev)->putSuperFunc)
		yaffs_dev_to_lc(dev)->putSuperFunc(sb);


	yaffs_deinitialise(dev);

	yaffs_gross_unlock(dev);

	down(&yaffs_context_lock);
	ylist_del_init(&(yaffs_dev_to_lc(dev)->contextList));
	up(&yaffs_context_lock);

	if (yaffs_dev_to_lc(dev)->spareBuffer) {
		YFREE(yaffs_dev_to_lc(dev)->spareBuffer);
		yaffs_dev_to_lc(dev)->spareBuffer = NULL;
	}

	kfree(dev);
}


static void yaffs_MTDPutSuper(struct super_block *sb)
{
	struct mtd_info *mtd = yaffs_dev_to_mtd(yaffs_SuperToDevice(sb));

	if (mtd->sync)
		mtd->sync(mtd);

	put_mtd_device(mtd);
}


static void yaffs_touch_super(yaffs_dev_t *dev)
{
	struct super_block *sb = yaffs_dev_to_lc(dev)->superBlock;

	T(YAFFS_TRACE_OS, (TSTR("yaffs_touch_super() sb = %p\n"), sb));
	if (sb)
		sb->s_dirt = 1;
}

typedef struct {
	int inband_tags;
	int skip_checkpoint_read;
	int skip_checkpoint_write;
	int no_cache;
	int tags_ecc_on;
	int tags_ecc_overridden;
	int lazy_loading_enabled;
	int lazy_loading_overridden;
	int empty_lost_and_found;
	int empty_lost_and_found_overridden;
} yaffs_options;

#define MAX_OPT_LEN 30
static int yaffs_parse_options(yaffs_options *options, const char *options_str)
{
	char cur_opt[MAX_OPT_LEN + 1];
	int p;
	int error = 0;

	/* Parse through the options which is a comma seperated list */

	while (options_str && *options_str && !error) {
		memset(cur_opt, 0, MAX_OPT_LEN + 1);
		p = 0;

		while(*options_str == ',')
			options_str++;

		while (*options_str && *options_str != ',') {
			if (p < MAX_OPT_LEN) {
				cur_opt[p] = *options_str;
				p++;
			}
			options_str++;
		}

		if (!strcmp(cur_opt, "inband-tags"))
			options->inband_tags = 1;
		else if (!strcmp(cur_opt, "tags-ecc-off")){
			options->tags_ecc_on = 0;
			options->tags_ecc_overridden=1;
		} else if (!strcmp(cur_opt, "tags-ecc-on")){
			options->tags_ecc_on = 1;
			options->tags_ecc_overridden = 1;
		} else if (!strcmp(cur_opt, "lazy-loading-off")){
			options->lazy_loading_enabled = 0;
			options->lazy_loading_overridden=1;
		} else if (!strcmp(cur_opt, "lazy-loading-on")){
			options->lazy_loading_enabled = 1;
			options->lazy_loading_overridden = 1;
		} else if (!strcmp(cur_opt, "empty-lost-and-found-off")){
			options->empty_lost_and_found = 0;
			options->empty_lost_and_found_overridden=1;
		} else if (!strcmp(cur_opt, "empty-lost-and-found-on")){
			options->empty_lost_and_found = 1;
			options->empty_lost_and_found_overridden=1;
		} else if (!strcmp(cur_opt, "no-cache"))
			options->no_cache = 1;
		else if (!strcmp(cur_opt, "no-checkpoint-read"))
			options->skip_checkpoint_read = 1;
		else if (!strcmp(cur_opt, "no-checkpoint-write"))
			options->skip_checkpoint_write = 1;
		else if (!strcmp(cur_opt, "no-checkpoint")) {
			options->skip_checkpoint_read = 1;
			options->skip_checkpoint_write = 1;
		} else {
			printk(KERN_INFO "yaffs: Bad mount option \"%s\"\n",
					cur_opt);
			error = 1;
		}
	}

	return error;
}

static struct super_block *yaffs_internal_read_super(int yaffs_version,
						struct super_block *sb,
						void *data, int silent)
{
	int nBlocks;
	struct inode *inode = NULL;
	struct dentry *root;
	yaffs_dev_t *dev = 0;
	char devname_buf[BDEVNAME_SIZE + 1];
	struct mtd_info *mtd;
	int err;
	char *data_str = (char *)data;
	struct yaffs_LinuxContext *context = NULL;
	yaffs_param_t *param;

	int read_only = 0;

	yaffs_options options;

	unsigned mount_id;
	int found;
	struct yaffs_LinuxContext *context_iterator;
	struct ylist_head *l;

	sb->s_magic = YAFFS_MAGIC;
	sb->s_op = &yaffs_super_ops;
	sb->s_flags |= MS_NOATIME;

	read_only =((sb->s_flags & MS_RDONLY) != 0);


#ifdef YAFFS_COMPILE_EXPORTFS
	sb->s_export_op = &yaffs_export_ops;
#endif

	if (!sb)
		printk(KERN_INFO "yaffs: sb is NULL\n");
	else if (!sb->s_dev)
		printk(KERN_INFO "yaffs: sb->s_dev is NULL\n");
	else if (!yaffs_devname(sb, devname_buf))
		printk(KERN_INFO "yaffs: devname is NULL\n");
	else
		printk(KERN_INFO "yaffs: dev is %d name is \"%s\" %s\n",
		       sb->s_dev,
		       yaffs_devname(sb, devname_buf),
		       read_only ? "ro" : "rw");

	if (!data_str)
		data_str = "";

	printk(KERN_INFO "yaffs: passed flags \"%s\"\n", data_str);

	memset(&options, 0, sizeof(options));

	if (yaffs_parse_options(&options, data_str)) {
		/* Option parsing failed */
		return NULL;
	}


	sb->s_blocksize = PAGE_CACHE_SIZE;
	sb->s_blocksize_bits = PAGE_CACHE_SHIFT;

	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_read_super: Using yaffs%d\n"), yaffs_version));
	T(YAFFS_TRACE_OS,
		(TSTR("yaffs_read_super: block size %d\n"),
		(int)(sb->s_blocksize)));

	T(YAFFS_TRACE_ALWAYS,
		(TSTR("yaffs: Attempting MTD mount of %u.%u,\"%s\"\n"),
	       MAJOR(sb->s_dev), MINOR(sb->s_dev),
	       yaffs_devname(sb, devname_buf)));

	/* Check it's an mtd device..... */
	if (MAJOR(sb->s_dev) != MTD_BLOCK_MAJOR)
		return NULL;	/* This isn't an mtd device */

	/* Get the device */
	mtd = get_mtd_device(NULL, MINOR(sb->s_dev));
	if (!mtd) {
		T(YAFFS_TRACE_ALWAYS,
			(TSTR("yaffs: MTD device #%u doesn't appear to exist\n"),
			MINOR(sb->s_dev)));
		return NULL;
	}
	/* Check it's NAND */
	if (mtd->type != MTD_NANDFLASH) {
		T(YAFFS_TRACE_ALWAYS,
			(TSTR("yaffs: MTD device is not NAND it's type %d\n"),
			mtd->type));
		return NULL;
	}

	T(YAFFS_TRACE_OS, (TSTR(" erase %p\n"), mtd->erase));
	T(YAFFS_TRACE_OS, (TSTR(" read %p\n"), mtd->read));
	T(YAFFS_TRACE_OS, (TSTR(" write %p\n"), mtd->write));
	T(YAFFS_TRACE_OS, (TSTR(" readoob %p\n"), mtd->read_oob));
	T(YAFFS_TRACE_OS, (TSTR(" writeoob %p\n"), mtd->write_oob));
	T(YAFFS_TRACE_OS, (TSTR(" block_isbad %p\n"), mtd->block_isbad));
	T(YAFFS_TRACE_OS, (TSTR(" block_markbad %p\n"), mtd->block_markbad));
	T(YAFFS_TRACE_OS, (TSTR(" %s %d\n"), WRITE_SIZE_STR, WRITE_SIZE(mtd)));
	T(YAFFS_TRACE_OS, (TSTR(" oobsize %d\n"), mtd->oobsize));
	T(YAFFS_TRACE_OS, (TSTR(" erasesize %d\n"), mtd->erasesize));
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 29)
	T(YAFFS_TRACE_OS, (TSTR(" size %u\n"), mtd->size));
#else
	T(YAFFS_TRACE_OS, (TSTR(" size %lld\n"), mtd->size));
#endif

#ifdef CONFIG_YAFFS_AUTO_YAFFS2

	if (yaffs_version == 1 && WRITE_SIZE(mtd) >= 2048) {
		T(YAFFS_TRACE_ALWAYS,
			(TSTR("yaffs: auto selecting yaffs2\n")));
		yaffs_version = 2;
	}

	/* Added NCB 26/5/2006 for completeness */
	if (yaffs_version == 2 && !options.inband_tags && WRITE_SIZE(mtd) == 512) {
		T(YAFFS_TRACE_ALWAYS,
			(TSTR("yaffs: auto selecting yaffs1\n")));
		yaffs_version = 1;
	}

#endif

	if (yaffs_version == 2) {
		/* Check for version 2 style functions */
		if (!mtd->erase ||
		    !mtd->block_isbad ||
		    !mtd->block_markbad ||
		    !mtd->read ||
		    !mtd->write ||
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
		    !mtd->read_oob || !mtd->write_oob) {
#else
		    !mtd->write_ecc ||
		    !mtd->read_ecc || !mtd->read_oob || !mtd->write_oob) {
#endif
			T(YAFFS_TRACE_ALWAYS,
			  (TSTR("yaffs: MTD device does not support required "
			   "functions\n")));
			return NULL;
		}

		if ((WRITE_SIZE(mtd) < YAFFS_MIN_YAFFS2_CHUNK_SIZE ||
		    mtd->oobsize < YAFFS_MIN_YAFFS2_SPARE_SIZE) &&
		    !options.inband_tags) {
			T(YAFFS_TRACE_ALWAYS,
			  (TSTR("yaffs: MTD device does not have the "
			   "right page sizes\n")));
			return NULL;
		}
	} else {
		/* Check for V1 style functions */
		if (!mtd->erase ||
		    !mtd->read ||
		    !mtd->write ||
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
		    !mtd->read_oob || !mtd->write_oob) {
#else
		    !mtd->write_ecc ||
		    !mtd->read_ecc || !mtd->read_oob || !mtd->write_oob) {
#endif
			T(YAFFS_TRACE_ALWAYS,
			  (TSTR("yaffs: MTD device does not support required "
			   "functions\n")));
			return NULL;
		}

		if (WRITE_SIZE(mtd) < YAFFS_BYTES_PER_CHUNK ||
		    mtd->oobsize != YAFFS_BYTES_PER_SPARE) {
			T(YAFFS_TRACE_ALWAYS,
			  (TSTR("yaffs: MTD device does not support have the "
			   "right page sizes\n")));
			return NULL;
		}
	}

	/* OK, so if we got here, we have an MTD that's NAND and looks
	 * like it has the right capabilities
	 * Set the yaffs_dev_t up for mtd
	 */

	if (!read_only && !(mtd->flags & MTD_WRITEABLE)){
		read_only = 1;
		printk(KERN_INFO "yaffs: mtd is read only, setting superblock read only");
		sb->s_flags |= MS_RDONLY;
	}

	dev = kmalloc(sizeof(yaffs_dev_t), GFP_KERNEL);
	context = kmalloc(sizeof(struct yaffs_LinuxContext),GFP_KERNEL);
	
	if(!dev || !context ){
		if(dev)
			kfree(dev);
		if(context)
			kfree(context);
		dev = NULL;
		context = NULL;
	}

	if (!dev) {
		/* Deep shit could not allocate device structure */
		T(YAFFS_TRACE_ALWAYS,
		  (TSTR("yaffs_read_super: Failed trying to allocate "
		   "yaffs_dev_t. \n")));
		return NULL;
	}
	memset(dev, 0, sizeof(yaffs_dev_t));
	param = &(dev->param);

	memset(context,0,sizeof(struct yaffs_LinuxContext));
	dev->os_context = context;
	YINIT_LIST_HEAD(&(context->contextList));
	context->dev = dev;
	context->superBlock = sb;

	dev->read_only = read_only;

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
	sb->s_fs_info = dev;
#else
	sb->u.generic_sbp = dev;
#endif
	
	dev->driver_context = mtd;
	param->name = mtd->name;

	/* Set up the memory size parameters.... */

	nBlocks = YCALCBLOCKS(mtd->size, (YAFFS_CHUNKS_PER_BLOCK * YAFFS_BYTES_PER_CHUNK));

	param->start_block = 0;
	param->end_block = nBlocks - 1;
	param->chunks_per_block = YAFFS_CHUNKS_PER_BLOCK;
	param->total_bytes_per_chunk = YAFFS_BYTES_PER_CHUNK;
	param->n_reserved_blocks = 5;
	param->n_caches = (options.no_cache) ? 0 : 10;
	param->inband_tags = options.inband_tags;

#ifdef CONFIG_YAFFS_DISABLE_LAZY_LOAD
	param->disable_lazy_load = 1;
#endif
#ifdef CONFIG_YAFFS_XATTR
	param->enable_xattr = 1;
#endif
	if(options.lazy_loading_overridden)
		param->disable_lazy_load = !options.lazy_loading_enabled;

#ifdef CONFIG_YAFFS_DISABLE_TAGS_ECC
	param->no_tags_ecc = 1;
#endif

#ifdef CONFIG_YAFFS_DISABLE_BACKGROUND
#else
	param->defered_dir_update = 1;
#endif

	if(options.tags_ecc_overridden)
		param->no_tags_ecc = !options.tags_ecc_on;

#ifdef CONFIG_YAFFS_EMPTY_LOST_AND_FOUND
	param->empty_lost_n_found = 1;
#endif

#ifdef CONFIG_YAFFS_DISABLE_BLOCK_REFRESHING
	param->refresh_period = 0;
#else
	param->refresh_period = 500;
#endif

#ifdef CONFIG_YAFFS__ALWAYS_CHECK_CHUNK_ERASED
	param->always_check_erased = 1;
#endif

	if(options.empty_lost_and_found_overridden)
		param->empty_lost_n_found = options.empty_lost_and_found;

	/* ... and the functions. */
	if (yaffs_version == 2) {
		param->write_chunk_tags_fn =
		    nandmtd2_WriteChunkWithTagsToNAND;
		param->read_chunk_tags_fn =
		    nandmtd2_ReadChunkWithTagsFromNAND;
		param->bad_block_fn = nandmtd2_MarkNANDBlockBad;
		param->query_block_fn = nandmtd2_QueryNANDBlock;
		yaffs_dev_to_lc(dev)->spareBuffer = YMALLOC(mtd->oobsize);
		param->is_yaffs2 = 1;
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
		param->total_bytes_per_chunk = mtd->writesize;
		param->chunks_per_block = mtd->erasesize / mtd->writesize;
#else
		param->total_bytes_per_chunk = mtd->oobblock;
		param->chunks_per_block = mtd->erasesize / mtd->oobblock;
#endif
		nBlocks = YCALCBLOCKS(mtd->size, mtd->erasesize);

		param->start_block = 0;
		param->end_block = nBlocks - 1;
	} else {
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
		/* use the MTD interface in yaffs_mtdif1.c */
		param->write_chunk_tags_fn =
			nandmtd1_WriteChunkWithTagsToNAND;
		param->read_chunk_tags_fn =
			nandmtd1_ReadChunkWithTagsFromNAND;
		param->bad_block_fn = nandmtd1_MarkNANDBlockBad;
		param->query_block_fn = nandmtd1_QueryNANDBlock;
#else
		param->write_chunk_fn = nandmtd_WriteChunkToNAND;
		param->read_chunk_fn = nandmtd_ReadChunkFromNAND;
#endif
		param->is_yaffs2 = 0;
	}
	/* ... and common functions */
	param->erase_fn = nandmtd_EraseBlockInNAND;
	param->initialise_flash_fn = nandmtd_InitialiseNAND;

	yaffs_dev_to_lc(dev)->putSuperFunc = yaffs_MTDPutSuper;

	param->sb_dirty_fn = yaffs_touch_super;
	param->gc_control = yaffs_gc_control_callback;

	yaffs_dev_to_lc(dev)->superBlock= sb;
	

#ifndef CONFIG_YAFFS_DOES_ECC
	param->use_nand_ecc = 1;
#endif

#ifdef CONFIG_YAFFS_DISABLE_WIDE_TNODES
	param->wide_tnodes_disabled = 1;
#endif

	param->skip_checkpt_rd = options.skip_checkpoint_read;
	param->skip_checkpt_wr = options.skip_checkpoint_write;

	down(&yaffs_context_lock);
	/* Get a mount id */
	found = 0;
	for(mount_id=0; ! found; mount_id++){
		found = 1;
		ylist_for_each(l,&yaffs_context_list){
			context_iterator = ylist_entry(l,struct yaffs_LinuxContext,contextList);
			if(context_iterator->mount_id == mount_id)
				found = 0;
		}
	}
	context->mount_id = mount_id;

	ylist_add_tail(&(yaffs_dev_to_lc(dev)->contextList), &yaffs_context_list);
	up(&yaffs_context_lock);

        /* Directory search handling...*/
        YINIT_LIST_HEAD(&(yaffs_dev_to_lc(dev)->searchContexts));
        param->remove_obj_fn = yaffs_remove_obj_callback;

	init_MUTEX(&(yaffs_dev_to_lc(dev)->grossLock));

	yaffs_gross_lock(dev);

	err = yaffs_guts_initialise(dev);

	T(YAFFS_TRACE_OS,
	  (TSTR("yaffs_read_super: guts initialised %s\n"),
	   (err == YAFFS_OK) ? "OK" : "FAILED"));
	   
	if(err == YAFFS_OK)
		yaffs_bg_start(dev);
		
	if(!context->bgThread)
		param->defered_dir_update = 0;


	/* Release lock before yaffs_get_inode() */
	yaffs_gross_unlock(dev);

	/* Create root inode */
	if (err == YAFFS_OK)
		inode = yaffs_get_inode(sb, S_IFDIR | 0755, 0,
					yaffs_root(dev));

	if (!inode)
		return NULL;

	inode->i_op = &yaffs_dir_inode_operations;
	inode->i_fop = &yaffs_dir_operations;

	T(YAFFS_TRACE_OS, (TSTR("yaffs_read_super: got root inode\n")));

	root = d_alloc_root(inode);

	T(YAFFS_TRACE_OS, (TSTR("yaffs_read_super: d_alloc_root done\n")));

	if (!root) {
		iput(inode);
		return NULL;
	}
	sb->s_root = root;
	sb->s_dirt = !dev->is_checkpointed;
	T(YAFFS_TRACE_ALWAYS,
		(TSTR("yaffs_read_super: is_checkpointed %d\n"),
		dev->is_checkpointed));

	T(YAFFS_TRACE_OS, (TSTR("yaffs_read_super: done\n")));
	return sb;
}


#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
static int yaffs_internal_read_super_mtd(struct super_block *sb, void *data,
					 int silent)
{
	return yaffs_internal_read_super(1, sb, data, silent) ? 0 : -EINVAL;
}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
static int yaffs_read_super(struct file_system_type *fs,
			    int flags, const char *dev_name,
			    void *data, struct vfsmount *mnt)
{

	return get_sb_bdev(fs, flags, dev_name, data,
			   yaffs_internal_read_super_mtd, mnt);
}
#else
static struct super_block *yaffs_read_super(struct file_system_type *fs,
					    int flags, const char *dev_name,
					    void *data)
{

	return get_sb_bdev(fs, flags, dev_name, data,
			   yaffs_internal_read_super_mtd);
}
#endif

static struct file_system_type yaffs_fs_type = {
	.owner = THIS_MODULE,
	.name = "yaffs",
	.get_sb = yaffs_read_super,
	.kill_sb = kill_block_super,
	.fs_flags = FS_REQUIRES_DEV,
};
#else
static struct super_block *yaffs_read_super(struct super_block *sb, void *data,
					    int silent)
{
	return yaffs_internal_read_super(1, sb, data, silent);
}

static DECLARE_FSTYPE(yaffs_fs_type, "yaffs", yaffs_read_super,
		      FS_REQUIRES_DEV);
#endif


#ifdef CONFIG_YAFFS_YAFFS2

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 5, 0))
static int yaffs2_internal_read_super_mtd(struct super_block *sb, void *data,
					  int silent)
{
	return yaffs_internal_read_super(2, sb, data, silent) ? 0 : -EINVAL;
}

#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 17))
static int yaffs2_read_super(struct file_system_type *fs,
			int flags, const char *dev_name, void *data,
			struct vfsmount *mnt)
{
	return get_sb_bdev(fs, flags, dev_name, data,
			yaffs2_internal_read_super_mtd, mnt);
}
#else
static struct super_block *yaffs2_read_super(struct file_system_type *fs,
					     int flags, const char *dev_name,
					     void *data)
{

	return get_sb_bdev(fs, flags, dev_name, data,
			   yaffs2_internal_read_super_mtd);
}
#endif

static struct file_system_type yaffs2_fs_type = {
	.owner = THIS_MODULE,
	.name = "yaffs2",
	.get_sb = yaffs2_read_super,
	.kill_sb = kill_block_super,
	.fs_flags = FS_REQUIRES_DEV,
};
#else
static struct super_block *yaffs2_read_super(struct super_block *sb,
					     void *data, int silent)
{
	return yaffs_internal_read_super(2, sb, data, silent);
}

static DECLARE_FSTYPE(yaffs2_fs_type, "yaffs2", yaffs2_read_super,
		      FS_REQUIRES_DEV);
#endif

#endif				/* CONFIG_YAFFS_YAFFS2 */

static struct proc_dir_entry *my_proc_entry;
static struct proc_dir_entry *debug_proc_entry;

static char *yaffs_dump_dev_part0(char *buf, yaffs_dev_t * dev)
{
	buf += sprintf(buf, "start_block.......... %d\n", dev->param.start_block);
	buf += sprintf(buf, "end_block............ %d\n", dev->param.end_block);
	buf += sprintf(buf, "total_bytes_per_chunk %d\n", dev->param.total_bytes_per_chunk);
	buf += sprintf(buf, "use_nand_ecc......... %d\n", dev->param.use_nand_ecc);
	buf += sprintf(buf, "no_tags_ecc.......... %d\n", dev->param.no_tags_ecc);
	buf += sprintf(buf, "is_yaffs2............ %d\n", dev->param.is_yaffs2);
	buf += sprintf(buf, "inband_tags.......... %d\n", dev->param.inband_tags);
	buf += sprintf(buf, "empty_lost_n_found... %d\n", dev->param.empty_lost_n_found);
	buf += sprintf(buf, "disable_lazy_load.... %d\n", dev->param.disable_lazy_load);
	buf += sprintf(buf, "refresh_period....... %d\n", dev->param.refresh_period);
	buf += sprintf(buf, "n_caches............. %d\n", dev->param.n_caches);
	buf += sprintf(buf, "n_reserved_blocks.... %d\n", dev->param.n_reserved_blocks);
	buf += sprintf(buf, "always_check_erased.. %d\n", dev->param.always_check_erased);

	buf += sprintf(buf, "\n");

	return buf;
}


static char *yaffs_dump_dev_part1(char *buf, yaffs_dev_t * dev)
{
	buf += sprintf(buf, "data_bytes_per_chunk. %d\n", dev->data_bytes_per_chunk);
	buf += sprintf(buf, "chunk_grp_bits....... %d\n", dev->chunk_grp_bits);
	buf += sprintf(buf, "chunk_grp_size....... %d\n", dev->chunk_grp_size);
	buf += sprintf(buf, "n_erased_blocks...... %d\n", dev->n_erased_blocks);
	buf += sprintf(buf, "blocks_in_checkpt.... %d\n", dev->blocks_in_checkpt);
	buf += sprintf(buf, "\n");
	buf += sprintf(buf, "n_tnodes............. %d\n", dev->n_tnodes);
	buf += sprintf(buf, "n_obj................ %d\n", dev->n_obj);
	buf += sprintf(buf, "n_free_chunks........ %d\n", dev->n_free_chunks);
	buf += sprintf(buf, "\n");
	buf += sprintf(buf, "n_page_writes........ %u\n", dev->n_page_writes);
	buf += sprintf(buf, "n_page_reads......... %u\n", dev->n_page_reads);
	buf += sprintf(buf, "n_erasures........... %u\n", dev->n_erasures);
	buf += sprintf(buf, "n_gc_copies.......... %u\n", dev->n_gc_copies);
	buf += sprintf(buf, "all_gcs.............. %u\n", dev->all_gcs);
	buf += sprintf(buf, "passive_gc_count..... %u\n", dev->passive_gc_count);
	buf += sprintf(buf, "oldest_dirty_gc_count %u\n", dev->oldest_dirty_gc_count);
	buf += sprintf(buf, "n_gc_blocks.......... %u\n", dev->n_gc_blocks);
	buf += sprintf(buf, "bg_gcs............... %u\n", dev->bg_gcs);
	buf += sprintf(buf, "n_retired_writes..... %u\n", dev->n_retired_writes);
	buf += sprintf(buf, "nRetireBlocks........ %u\n", dev->n_retired_blocks);
	buf += sprintf(buf, "n_ecc_fixed.......... %u\n", dev->n_ecc_fixed);
	buf += sprintf(buf, "n_ecc_unfixed........ %u\n", dev->n_ecc_unfixed);
	buf += sprintf(buf, "n_tags_ecc_fixed..... %u\n", dev->n_tags_ecc_fixed);
	buf += sprintf(buf, "n_tags_ecc_unfixed... %u\n", dev->n_tags_ecc_unfixed);
	buf += sprintf(buf, "cache_hits........... %u\n", dev->cache_hits);
	buf += sprintf(buf, "n_deleted_files...... %u\n", dev->n_deleted_files);
	buf += sprintf(buf, "n_unlinked_files..... %u\n", dev->n_unlinked_files);
	buf += sprintf(buf, "refresh_count........ %u\n", dev->refresh_count);
	buf += sprintf(buf, "n_bg_deletions....... %u\n", dev->n_bg_deletions);

	return buf;
}

static int yaffs_proc_read(char *page,
			   char **start,
			   off_t offset, int count, int *eof, void *data)
{
	struct ylist_head *item;
	char *buf = page;
	int step = offset;
	int n = 0;

	/* Get proc_file_read() to step 'offset' by one on each sucessive call.
	 * We use 'offset' (*ppos) to indicate where we are in dev_list.
	 * This also assumes the user has posted a read buffer large
	 * enough to hold the complete output; but that's life in /proc.
	 */

	*(int *)start = 1;

	/* Print header first */
	if (step == 0)
		buf += sprintf(buf, "Multi-version YAFFS built:" __DATE__ " " __TIME__"\n");
	else if (step == 1)
		buf += sprintf(buf,"\n");
	else {
		step-=2;
		
		down(&yaffs_context_lock);

		/* Locate and print the Nth entry.  Order N-squared but N is small. */
		ylist_for_each(item, &yaffs_context_list) {
			struct yaffs_LinuxContext *dc = ylist_entry(item, struct yaffs_LinuxContext, contextList);
			yaffs_dev_t *dev = dc->dev;

			if (n < (step & ~1)) {
				n+=2;
				continue;
			}
			if((step & 1)==0){
				buf += sprintf(buf, "\nDevice %d \"%s\"\n", n, dev->param.name);
				buf = yaffs_dump_dev_part0(buf, dev);
			} else
				buf = yaffs_dump_dev_part1(buf, dev);
			
			break;
		}
		up(&yaffs_context_lock);
	}

	return buf - page < count ? buf - page : count;
}

static int yaffs_stats_proc_read(char *page,
				char **start,
				off_t offset, int count, int *eof, void *data)
{
	struct ylist_head *item;
	char *buf = page;
	int n = 0;

	down(&yaffs_context_lock);

	/* Locate and print the Nth entry.  Order N-squared but N is small. */
	ylist_for_each(item, &yaffs_context_list) {
		struct yaffs_LinuxContext *dc = ylist_entry(item, struct yaffs_LinuxContext, contextList);
		yaffs_dev_t *dev = dc->dev;

		int erasedChunks;

		erasedChunks = dev->n_erased_blocks * dev->param.chunks_per_block;
		
		buf += sprintf(buf,"%d, %d, %d, %u, %u, %u, %u\n",
				n, dev->n_free_chunks, erasedChunks,
				dev->bg_gcs, dev->oldest_dirty_gc_count,
				dev->n_obj, dev->n_tnodes);
	}
	up(&yaffs_context_lock);


	return buf - page < count ? buf - page : count;
}

/**
 * Set the verbosity of the warnings and error messages.
 *
 * Note that the names can only be a..z or _ with the current code.
 */

static struct {
	char *mask_name;
	unsigned mask_bitfield;
} mask_flags[] = {
	{"allocate", YAFFS_TRACE_ALLOCATE},
	{"always", YAFFS_TRACE_ALWAYS},
	{"background", YAFFS_TRACE_BACKGROUND},
	{"bad_blocks", YAFFS_TRACE_BAD_BLOCKS},
	{"buffers", YAFFS_TRACE_BUFFERS},
	{"bug", YAFFS_TRACE_BUG},
	{"checkpt", YAFFS_TRACE_CHECKPOINT},
	{"deletion", YAFFS_TRACE_DELETION},
	{"erase", YAFFS_TRACE_ERASE},
	{"error", YAFFS_TRACE_ERROR},
	{"gc_detail", YAFFS_TRACE_GC_DETAIL},
	{"gc", YAFFS_TRACE_GC},
	{"lock", YAFFS_TRACE_LOCK},
	{"mtd", YAFFS_TRACE_MTD},
	{"nandaccess", YAFFS_TRACE_NANDACCESS},
	{"os", YAFFS_TRACE_OS},
	{"scan_debug", YAFFS_TRACE_SCAN_DEBUG},
	{"scan", YAFFS_TRACE_SCAN},
	{"tracing", YAFFS_TRACE_TRACING},
	{"sync", YAFFS_TRACE_SYNC},
	{"write", YAFFS_TRACE_WRITE},

	{"verify", YAFFS_TRACE_VERIFY},
	{"verify_nand", YAFFS_TRACE_VERIFY_NAND},
	{"verify_full", YAFFS_TRACE_VERIFY_FULL},
	{"verify_all", YAFFS_TRACE_VERIFY_ALL},

	{"all", 0xffffffff},
	{"none", 0},
	{NULL, 0},
};

#define MAX_MASK_NAME_LENGTH 40
static int yaffs_proc_write_trace_options(struct file *file, const char *buf,
					 unsigned long count, void *data)
{
	unsigned rg = 0, mask_bitfield;
	char *end;
	char *mask_name;
	const char *x;
	char substring[MAX_MASK_NAME_LENGTH + 1];
	int i;
	int done = 0;
	int add, len = 0;
	int pos = 0;

	rg = yaffs_trace_mask;

	while (!done && (pos < count)) {
		done = 1;
		while ((pos < count) && isspace(buf[pos]))
			pos++;

		switch (buf[pos]) {
		case '+':
		case '-':
		case '=':
			add = buf[pos];
			pos++;
			break;

		default:
			add = ' ';
			break;
		}
		mask_name = NULL;

		mask_bitfield = simple_strtoul(buf + pos, &end, 0);

		if (end > buf + pos) {
			mask_name = "numeral";
			len = end - (buf + pos);
			pos += len;
			done = 0;
		} else {
			for (x = buf + pos, i = 0;
			    (*x == '_' || (*x >= 'a' && *x <= 'z')) &&
			    i < MAX_MASK_NAME_LENGTH; x++, i++, pos++)
				substring[i] = *x;
			substring[i] = '\0';

			for (i = 0; mask_flags[i].mask_name != NULL; i++) {
				if (strcmp(substring, mask_flags[i].mask_name) == 0) {
					mask_name = mask_flags[i].mask_name;
					mask_bitfield = mask_flags[i].mask_bitfield;
					done = 0;
					break;
				}
			}
		}

		if (mask_name != NULL) {
			done = 0;
			switch (add) {
			case '-':
				rg &= ~mask_bitfield;
				break;
			case '+':
				rg |= mask_bitfield;
				break;
			case '=':
				rg = mask_bitfield;
				break;
			default:
				rg |= mask_bitfield;
				break;
			}
		}
	}

	yaffs_trace_mask = rg | YAFFS_TRACE_ALWAYS;

	printk(KERN_DEBUG "new trace = 0x%08X\n", yaffs_trace_mask);

	if (rg & YAFFS_TRACE_ALWAYS) {
		for (i = 0; mask_flags[i].mask_name != NULL; i++) {
			char flag;
			flag = ((rg & mask_flags[i].mask_bitfield) ==
				mask_flags[i].mask_bitfield) ? '+' : '-';
			printk(KERN_DEBUG "%c%s\n", flag, mask_flags[i].mask_name);
		}
	}

	return count;
}


static int yaffs_proc_write(struct file *file, const char *buf,
					 unsigned long count, void *data)
{
        return yaffs_proc_write_trace_options(file, buf, count, data);
}

/* Stuff to handle installation of file systems */
struct file_system_to_install {
	struct file_system_type *fst;
	int installed;
};

static struct file_system_to_install fs_to_install[] = {
	{&yaffs_fs_type, 0},
	{&yaffs2_fs_type, 0},
	{NULL, 0}
};

static int __init init_yaffs_fs(void)
{
	int error = 0;
	struct file_system_to_install *fsinst;

	T(YAFFS_TRACE_ALWAYS,
	  (TSTR("yaffs built " __DATE__ " " __TIME__ " Installing. \n")));

#ifdef CONFIG_YAFFS_ALWAYS_CHECK_CHUNK_ERASED
	T(YAFFS_TRACE_ALWAYS,
	  (TSTR(" \n\n\n\nYAFFS-WARNING CONFIG_YAFFS_ALWAYS_CHECK_CHUNK_ERASED selected.\n\n\n\n")));
#endif




	init_MUTEX(&yaffs_context_lock);

	/* Install the proc_fs entries */
	my_proc_entry = create_proc_entry("yaffs",
					       S_IRUGO | S_IFREG,
					       YPROC_ROOT);

	if (my_proc_entry) {
		my_proc_entry->write_proc = yaffs_proc_write;
		my_proc_entry->read_proc = yaffs_proc_read;
		my_proc_entry->data = NULL;
	} else
		return -ENOMEM;

	debug_proc_entry = create_proc_entry("yaffs_stats",
					       S_IRUGO | S_IFREG,
					       YPROC_ROOT);

	if (debug_proc_entry) {
		debug_proc_entry->write_proc = NULL;
		debug_proc_entry->read_proc = yaffs_stats_proc_read;
		debug_proc_entry->data = NULL;
	} else
		return -ENOMEM;

	/* Now add the file system entries */

	fsinst = fs_to_install;

	while (fsinst->fst && !error) {
		error = register_filesystem(fsinst->fst);
		if (!error)
			fsinst->installed = 1;
		fsinst++;
	}

	/* Any errors? uninstall  */
	if (error) {
		fsinst = fs_to_install;

		while (fsinst->fst) {
			if (fsinst->installed) {
				unregister_filesystem(fsinst->fst);
				fsinst->installed = 0;
			}
			fsinst++;
		}
	}

	return error;
}

static void __exit exit_yaffs_fs(void)
{

	struct file_system_to_install *fsinst;

	T(YAFFS_TRACE_ALWAYS,
		(TSTR("yaffs built " __DATE__ " " __TIME__ " removing. \n")));

	remove_proc_entry("yaffs", YPROC_ROOT);
	remove_proc_entry("yaffs_stats", YPROC_ROOT);

	fsinst = fs_to_install;

	while (fsinst->fst) {
		if (fsinst->installed) {
			unregister_filesystem(fsinst->fst);
			fsinst->installed = 0;
		}
		fsinst++;
	}
}

module_init(init_yaffs_fs)
module_exit(exit_yaffs_fs)

MODULE_DESCRIPTION("YAFFS2 - a NAND specific flash file system");
MODULE_AUTHOR("Charles Manning, Aleph One Ltd., 2002-2010");
MODULE_LICENSE("GPL");
