#include "../include/tools.h"

int main(){
    int clientsock = CreateClientSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    char recvBuf[BUFSIZ];
    char sendBuf[BUFSIZ];

    while(true)
    {
        printf("send message or quit(enter quit or q to quit) : ");
		scanf("%s", sendBuf);
		if (-1 == write(clientsock, sendBuf, strlen(sendBuf)))
		{
            errorhandling("fail to send with code: %d\n", errno);
		}
        if(quitjudge(sendBuf)) break;
        memset(sendBuf, 0, sizeof(sendBuf));
        
		if (read(clientsock, recvBuf, sizeof(char)*BUFSIZ) > 0)
		{
            if(quitjudge(recvBuf)){
                printf("serve disconnect\n");
                break;
            }
            printf("recive message:%s \n", recvBuf);
			memset(recvBuf, 0, sizeof(recvBuf));
		}	
    }

    close(clientsock);
    return 0;
}