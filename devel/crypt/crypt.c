#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/string.h>
#include <linux/tcp.h>

extern int (*encrypt_packet)(struct iovec *iov, int key);
extern int (*decrypt_packet)(struct sk_buff *skb, int key);

static int my_encrypt_packet(struct iovec *iov, int key) {
	int i = 0;
	char *buf = iov->iov_base;

	printk(KERN_INFO "Before encryption: %s", buf);

	for (i = 0; i < iov->iov_len - 1; i++) {
		buf[i] = (buf[i] + key) % 255;
	}

	printk(KERN_INFO "After  encryption: %s", buf);
	return 0;
}

static int my_decrypt_packet(struct sk_buff *skb, int key) {
	int i = 0;
	struct sk_buff *skb_cloned;
	char *data;

	if (skb_linearize(skb) == 0) {
		printk(KERN_DEBUG "skb is non-linear");
	}
	else {
		printk(KERN_DEBUG "skb is linear");
	}

	skb_cloned = skb_clone(skb, 0);
	data = skb_pull(skb_cloned, tcp_hdrlen(skb_cloned));

	printk(KERN_INFO "Before decryption: %s", data);

	for (i = 0;i < strlen(data) - 1; i++) {
		data[i] = data[i] - key;
	}

	printk(KERN_INFO "After  decryption: %s", data);
	return 0;
}

static int __init init_crypt(void) {
	printk("Crypt init\n");
	encrypt_packet = &my_encrypt_packet;
	decrypt_packet = &my_decrypt_packet;
	return 0;
}

static void __exit exit_crypt(void) {
	encrypt_packet = NULL;
	decrypt_packet = NULL;
	printk("Crypt exit\n");
}

module_init(init_crypt);
module_exit(exit_crypt);
