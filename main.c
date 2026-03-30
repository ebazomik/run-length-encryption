#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>

#define PAGE_SIZE 4096

void charCountToBuffer(char c, int count, char *buff, int *seek){
	char b[10];
	buff[(*seek)++] = c;
	int len = sprintf(b, "%d", count);
	for(int j = 0; j < len; j++){
		buff[(*seek)++] = b[j];
	}
}

int compress(int fd, int fdOut){
	
	char buffR[PAGE_SIZE]; 
	int bytesReaded = 0;
	int totalBytesWrited = 0;

	while((bytesReaded = read(fd, buffR, PAGE_SIZE)) != 0){
			
		char buffW[PAGE_SIZE * 2];
		int charCount = 1;
		int seek = 0;
		
		for(int i = 1; i < bytesReaded; i++){
			if(buffR[i] == buffR[i-1]){
				charCount++;
				if(i == PAGE_SIZE -1){
					charCountToBuffer(buffR[i], charCount, buffW, &seek);
				}
			} else {
				charCountToBuffer(buffR[i-1], charCount, buffW, &seek);	
				charCount = 1;
			}

		}

		int bytesWrited = write(fdOut, buffW, seek);
		if(bytesWrited == -1) return -1;

		totalBytesWrited += bytesWrited;
	}

	return totalBytesWrited;
}

int decompress(int fd, int fdOut){
		
		char buffR[PAGE_SIZE];
		int bytesReaded = 0;
		int totalBytesWrited;

		while((bytesReaded = read(fd, buffR, PAGE_SIZE)) != 0){
			
			char buffW[PAGE_SIZE + 1] = "";
			int seek = 0;
			char chWrite;

			for(int i = 0; i < bytesReaded; i++){

					char accumulator[256] = "";
					int accseek = 0;

					if(!isdigit(buffR[i])){
						chWrite = buffR[i];
					} else {
						int j = i;
						while(isdigit(buffR[j])){
							accumulator[accseek++] = buffR[j];
							j++;
						}
						i = j-1;
					}
					
					if(accumulator[0] != '\0'){
						int count = atoi(accumulator);
						for(int k = 0; k < count; k++){
							buffW[seek++] = chWrite;
						}
					}

			}

			buffW[seek] = '\n';
			int bytesWrited = write(fdOut, buffW, seek);
			if(bytesWrited == -1) return -1; 
			totalBytesWrited += bytesWrited;
		}

    return totalBytesWrited;
}


int main(int argc, char **argv){

    int opt;
    int operation = 1;
    char *source = NULL;
    char *destination = NULL;


    while((opt = getopt(argc, argv, "CDs:d:")) != -1){
        switch(opt){
            case 'C':
                operation = 1;
                break;
            case 'D':
                operation = 0;
                break;
            case 's':
                source = optarg;
                break;
            case 'd':
                destination = optarg;
                break;
        }
    }

    if(operation > 1 || operation < 0 || source == NULL || destination == NULL){
        printf("Usage: %s -C [compress] or -D [de-compress] -s [source file] -d [destination file]\n", argv[0]);
        printf("Default option is compress mode, you can use directly: %s -s [source file] -d [destination file] \n", argv[0]);
        return 1;
    }

    int fd = open(source, O_RDONLY);
	int fdOut = open(destination, O_CREAT | O_WRONLY | O_TRUNC, 0666);
	

    if(fd == -1 || fdOut == -1){
        printf("%s\n", strerror(errno));
        return 1;
    }

    if(operation){
        int byteCompressed = compress(fd, fdOut);
        if(byteCompressed == -1){
            printf("Error when trying compress %s\n", source);
						printf("%s\n", strerror(errno));
            return 1;
        } else if(byteCompressed == 0){
            printf("File %s was processed without compress\n", source);
            return 0;
        }
    } else {
        int fullBytes = decompress(fd, fdOut);
        if(fullBytes == -1){
            printf("Error when trying de-compress %s\n", source);
            return 1;
        }
    }

		close(fd);
		close(fdOut);


    return 0;
}
