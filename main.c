#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

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

			printf("seek: %d\n", seek);

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

int decompress(int fd, char *destination){
    return 0;
}


int main(int argc, char **argv){

    int opt;
    int operation = 1;
    char *source = NULL;
    char *destination = NULL;


    while((opt = getopt(argc, argv, "cds:t:")) != -1){
        switch(opt){
            case 'c':
                operation = 1;
                break;
            case 'd':
                operation = 0;
                break;
            case 's':
                source = optarg;
                break;
            case 't':
                destination = optarg;
                break;
        }
    }

    if(operation > 1 || operation < 0 || source == NULL || destination == NULL){
        printf("Usage: %s -c [compress] or -d [de-compress] -s [source file] -t [destination to file]\n", argv[0]);
        printf("Default option is compress, you can use directly: %s -s [source file] -t [destination to file] \n", argv[0]);
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
        int fullBytes = decompress(fd, destination);
        if(fullBytes == -1){
            printf("Error when trying de-compress %s\n", source);
            return 1;
        }
    }

		close(fd);
		close(fdOut);


    return 0;
}
