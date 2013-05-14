#include <linux/module.h>
#include <linux/init.h>
#include <linux/buffer_head.h>
#include <linux/blkdev.h>
#include <linux/random.h>

static int set_size = 512;
static struct block_device *bdev;

static int __init init_generator(void) {
	int i = 0;
	int random_number = 0;
	printk("Generator init\n");
	bdev = open_by_devnum(MKDEV(8,17), 0x08000);
	if(IS_ERR(bdev)) {
		return -EIO;
	}
	if(set_blocksize(bdev,set_size)) {
		printk("set block size error\n");
		return -EIO;
	}
	// Design your write pattern here!!
	for(i=0; i<100; i++){
		get_random_bytes(&random_number, sizeof(random_number));
		
		__breadahead(bdev, (random_number%100)*4096, set_size);
	}
	return 0;
}

static void __exit exit_generator(void) {
	printk("Generator exit\n");
	if (bdev) {
		blkdev_put(bdev, 0x08000);
		bdev = NULL;
	}
}

module_init(init_generator);
module_exit(exit_generator);
