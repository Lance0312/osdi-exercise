#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

char* global_buffer;

int main() {
	int n;
	int i = 16000000;
	char* local_buffer;


	printf("%d\n", getpid());
	printf("Stage 1, press enter to continue\n");
	getchar();

	global_buffer = (char*)malloc(i);
	local_buffer = (char*)malloc(i);
	printf("Stage 2, press enter to continue\n");
	getchar();

	for (n=0;n<i;n++) {
		global_buffer[n] = n % 26 + 'a';
		local_buffer[n] = n % 26 + 'a';
	}
	global_buffer[n] = '\0';
	local_buffer[n] = '\0';
	printf("Stage 3, press enter to continue\n");
	getchar();


	return 0;
}
