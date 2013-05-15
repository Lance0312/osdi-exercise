/* file-mmu.c: ramfs MMU-based file operations
 *
 * Resizable simple ram filesystem for Linux.
 *
 * Copyright (C) 2000 Linus Torvalds.
 *               2000 Transmeta Corp.
 *
 * Usage limits added by David Gibson, Linuxcare Australia.
 * This file is released under the GPL.
 */

/*
 * NOTE! This filesystem is probably most useful
 * not as a real filesystem, but as an example of
 * how virtual filesystems can be written.
 *
 * It doesn't get much simpler than this. Consider
 * that this file implements the full semantics of
 * a POSIX-compliant read-write filesystem.
 *
 * Note in particular how the filesystem does not
 * need to implement any data structures of its own
 * to keep track of the virtual data: using the VFS
 * caches is sufficient.
 */

#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/ramfs.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/vmalloc.h>

#include "internal.h"

extern char encrypt_flag[1024];
static int key = 0x25;

ssize_t my_do_sync_read(struct file *filp, char __user *buf, size_t len, loff_t *ppos) {
	int i = 0;
	ssize_t ret;

	printk(KERN_INFO "my_do_sync_read %s", encrypt_flag);

	ret = do_sync_read(filp, buf, len, ppos);

	if (strncmp(encrypt_flag, "1", 1) == 0) {
		for (i = 0;i < strlen(buf);i++) {
			buf[i] = buf[i] ^ key;
		}
		printk(KERN_INFO "buf: %s", buf);
	}


	return ret;
}

ssize_t my_do_sync_write(struct file *filp, char __user *buf, size_t len, loff_t *ppos) {
	int i = 0;
	char *tmp;
	ssize_t ret;

	printk(KERN_INFO "my_do_sync_write %s", encrypt_flag);

	tmp = vmalloc(len);
	memcpy(tmp, buf, len);

	if (strncmp(encrypt_flag, "1", 1) == 0) {
		for (i = 0;i < strlen(tmp);i++) {
			buf[i] = buf[i] ^ key;
		}
		printk(KERN_INFO "buf: %s", buf);
	}


	ret = do_sync_write(filp, buf, len, ppos);

	vfree(tmp);

	return ret;
}

const struct address_space_operations ramfs_aops = {
	.readpage	= simple_readpage,
	.write_begin	= simple_write_begin,
	.write_end	= simple_write_end,
	.set_page_dirty = __set_page_dirty_no_writeback,
};

const struct file_operations ramfs_file_operations = {
	.read		= my_do_sync_read,
	.aio_read	= generic_file_aio_read,
	.write		= my_do_sync_write,
	.aio_write	= generic_file_aio_write,
	.mmap		= generic_file_mmap,
	.fsync		= simple_sync_file,
	.splice_read	= generic_file_splice_read,
	.splice_write	= generic_file_splice_write,
	.llseek		= generic_file_llseek,
};

const struct inode_operations ramfs_file_inode_operations = {
	.getattr	= simple_getattr,
};
