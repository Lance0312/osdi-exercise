#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stropts.h>
#include <linux/fs.h>

int main() {
	int fd;
	fd = open("/dev/ram0",O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Cannot open device /dev/ram0");
	}

	ioctl(fd, BLKROLLBACK, 0);
	close(fd);
}
