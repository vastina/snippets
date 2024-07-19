#include "../include/tools.h"

int main(){
    int servesock = CreateSeverSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    char buffer[BUFSIZ];
    memset(buffer, 0 ,sizeof(buffer));
    //char buffer[BUFSIZ];
    FILE* fp;
    fp = fopen("test1.txt", "rb");
    if(fp == NULL) printf("no such a file\n");
    
    while(true){
        int client = accept(servesock, NULL, NULL);
        while (true)
    	{
            if (read(client, buffer, sizeof(buffer) ) > 0)
		    {
                if(quitjudge(buffer)){
                    printf("client will be disconnected\nsend the last message:");
                    scanf("%s", buffer);
                    write(client, buffer, sizeof(char)*strlen(buffer) );
                    break;
                }
                else if(sendfile(buffer)){
                    printf("sendfile..................\n");
                    int read_count = fread((void*)buffer, sizeof(char), sizeof(buffer), fp);
                    if(BUFSIZ > read_count){
                        write(client, buffer, read_count);
                    }
                    else write(client, buffer, BUFSIZ);
                    printf("send file done\n");
                }
                else printf("recive message:%s \n", buffer);
		    }
            memset(buffer, 0, sizeof(char)*strlen(buffer));

            printf("send message or quit(enter quit or q to quit) : ");
		    scanf("%s", buffer);
		    if (-1 == write(client, buffer, sizeof(char)*strlen(buffer)))
		    {
                errorhandling("fail to send with code: %d \n", errno );
		    }
            if(quitjudge(buffer)) break;
            memset(buffer, 0, sizeof(buffer));
	    }
        printf("quit or not [q/n] :"); scanf("%s", buffer);
        if(quitjudge(buffer)) break;
    }
    

    close(servesock);
    return 0;
}