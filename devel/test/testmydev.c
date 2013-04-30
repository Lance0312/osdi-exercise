#include <stdio.h>
#include <string.h>
int main()
{
   char buf[1024];
   FILE *fp = fopen("/dev/mydev", "w+");
   if(fp == NULL) {
      printf("can't open device!\n");
      return -1;
   }
   memset(buf, 0, sizeof(buf));
   fwrite("abcde", 5, 1, fp);
   fread(buf, sizeof(buf), 1, fp);
   fclose(fp);
   printf("buf: %s\n", buf);
   return 0;
}
