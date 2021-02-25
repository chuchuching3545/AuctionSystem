#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
int main(int argc, char **argv){
	int bid_list[21] = {20, 18, 5, 21, 8, 7, 2, 19, 14, 13, 9, 1, 6, 10, 16, 11, 4, 12, 15, 17, 3};
	for(int i = 0; i < 10; i++){
		printf("%s %d\n", argv[1], bid_list[atoi(argv[1]) + i - 1] * 100);
		fflush(stdout);
	}
	return 0;
}