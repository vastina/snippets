#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <future>

#define VASTINA_CPP
#include "../include/tools.h"


int main(int argc, char** argv){
    printf("port to connect: 8888,\n");
    int clientsock = CreateClientSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,
         argc > 1 ? std::atoi(argv[1]) : 1145);
    //char buffer[BUFSIZ]; data race 
    bool stop = false;
    std::mutex smutex;
    std::vector< std::future<int> > results{};
    std::vector< std::thread> workers{};
    std::vector< std::packaged_task<int()> > tasks{};

    tasks.emplace_back(std::packaged_task<int()>([&]{
        //int clientsock = CreateClientSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        char buffer1[BUFSIZ];    
        while(true){
            printf("\nsend message or quit(enter quit or q to quit) : ");
            scanf("%s", buffer1);

            if (-1 == write(clientsock, buffer1, strlen(buffer1)))
            {
                errorhandling("\nfail to send with code: %d\n", errno);
            }

            if(quitjudge(buffer1)){
                {
                    std::lock_guard<std::mutex> lock(smutex);
                    stop = true;
                }
                break;
            } 
        }   return 1;
    }) );
    tasks.emplace_back(std::packaged_task<int()>([&]{//&smutex,&clientsock, &stop
        char buffer[BUFSIZ];
        while(!stop){
            if (read(clientsock, buffer, sizeof(char)*BUFSIZ) > 0)
		    {  
                if(quitjudge(buffer)){
                    printf("\nserve disconnect\n");
                    break;
                }
                else if(sendfile(buffer)){
                    FILE* fp;
                    fp = fopen("b.txt", "wb");
                    if(fp == NULL){
                        system("touch b.txt");
                        fp = fopen("b.txt", "wb");
                    } 
                    int read_count = 0;
                    {
                        std::lock_guard<std::mutex> lock(smutex);
                        if((read_count = read(clientsock, buffer, BUFSIZ) )> 0)
                            fwrite((void*)buffer, sizeof(char), read_count, fp);
                    }   
                    fclose(fp);
                    printf("\nfile received\n");
                }
                else{
                    print("\nrecive message:{} \n", buffer);
                }
                memset(buffer, 0, sizeof(buffer));
		    }	
        }   return 2;
    }));

    for(int count=0;auto& task: tasks)
    {
        results.emplace_back(task.get_future() );
        workers.emplace_back(
            [&count, &task] 
            { 
                task(); 
            } 
        );
    }
    for(auto& worker: workers) worker.join();
    for(int count=0;auto& result:results)
        print("({}, {})\n", ++count,result.get());

    close(clientsock);
    return 0;
}