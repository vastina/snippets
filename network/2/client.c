#include <time.h>

#include "../include/tools.h"


int main(){
    int clntsock = CreateClientSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    char buffer[BUFSIZ];
    //char buffer[BUFSIZ];
    FILE* fp;
    fp = fopen("test2.txt", "wb");
    if(fp == NULL) printf("no such a file\n");

    while(true)
    {
        printf("send message or quit(enter quit or q to quit) : ");
		scanf("%s", buffer);
		if (-1 == write(clntsock, buffer, strlen(buffer)))
		{
            errorhandling("fail to send with code: %d\n", errno);
		}
        if(quitjudge(buffer)){
            shutdown(clntsock, SHUT_WR);
            break;
        } 
        else if(sendfile(buffer)){
            int read_count = 0;
            if((read_count = read(clntsock, buffer, BUFSIZ) )> 0)
                fwrite((void*)buffer, sizeof(char), read_count, fp);
            fclose(fp);
            printf("file received\n");
        }
        memset(buffer, 0, sizeof(buffer));
        
		if (read(clntsock, buffer, sizeof(char)*BUFSIZ) > 0)
		{
            if(quitjudge(buffer)){
                printf("serve disconnect\n");
                break;
            }
            printf("recive message:%s \n", buffer);
			memset(buffer, 0, sizeof(buffer));
		}	
    }
    printf("will be closed in one minute.......\n");
    int t = clock();
    while(clock()-t < 50) read(clntsock, buffer, BUFSIZ);
    printf("last message from serve: %s \n", buffer);

    close(clntsock);
    return 0;
}