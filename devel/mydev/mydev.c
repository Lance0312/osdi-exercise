#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/string.h>

#define MAJOR_NUM 60

char buffer[1024];

static int drv_open(struct inode *inode, struct file *filp) {
	memset(buffer, 0, 1024);
	return 0;
}

static ssize_t drv_read(struct file *filp, char *user_buffer, size_t count, loff_t *ppos) {
	if (count > 1024) {
		count = 1024;
	}
	strncpy(user_buffer, buffer, count);
	return count;
}

static ssize_t drv_write(struct file *filp, const char *user_buffer, size_t count, loff_t *ppos) {
	if (count > 1024) {
		count = 1024;
	}
	strncpy(buffer, user_buffer, count);
	return count;
}

static int drv_release(struct inode *inode, struct file *filp) {
	return 0;
}

struct file_operations drv_fops = {
	read:           drv_read,
	write:          drv_write,
	open:           drv_open,
	release:        drv_release,
};

static int mydev_init(void) {
	if (register_chrdev(MAJOR_NUM, "mydev", &drv_fops) < 0) {
		printk("Cannot register device mydev\n");
		return (-EBUSY);
	}
	printk("/****** Welcome to mydev_0156004 ******/\n");
	return 0;
}

static void mydev_exit(void) {
	unregister_chrdev(MAJOR_NUM, "mydev");
	printk("/****** Bye ******/\n");
}

module_init(mydev_init);
module_exit(mydev_exit);
