#include "../include/tools.h"

int main(){
    int servesock = CreateSeverSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    char recvBuf[BUFSIZ];
    char sendBuf[BUFSIZ];
    
    while(true){
    int client = accept(servesock, NULL, NULL);
    while (true)
	{
        if (read(client, recvBuf, sizeof(recvBuf) ) > 0)
		{
            if(quitjudge(recvBuf)){
                printf("client disconnect\n");
                break;
            }
            printf("recive message:%s\n", recvBuf);
			memset(recvBuf, 0, sizeof(char)*strlen(recvBuf));
		}

        printf("send message or quit(enter quit or q to quit) : ");
		scanf("%s", sendBuf);
		if (-1 == write(client, sendBuf, sizeof(char)*strlen(sendBuf)))
		{
            errorhandling("fail to send with code: %d \n", errno );
		}
        if(quitjudge(sendBuf)) break;
        memset(sendBuf, 0, sizeof(sendBuf));
	}
    char quit[5];
    printf("quit or not :"); scanf("%s", quit);
    if(quitjudge(quit)) break;
    }
    

    close(servesock);
    return 0;
}