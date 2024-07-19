#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <future>
#include <format>
#include <unistd.h>
#include <fcntl.h>

#define VASTINA_CPP
#include "../include/tools.h"
//#include "../include/ThreadPool.hpp"
//#include "../include/threadtest.hpp"

int main(){
    //ThreadPool pool(4);
    int filefd = open("msg.txt", O_WRONLY | O_APPEND | O_CREAT, 0644);
    std::vector< std::future<int> > results{};
    std::vector< std::thread> workers{};
    std::vector< std::packaged_task<int(int)> > tasks{};
    int servesock = CreateSeverSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    for(int i = 0; i < 4; ++i) {
        tasks.emplace_back(std::packaged_task<int(int)>( [servesock, filefd](int k)->int{
            int client = 0;
            while(!client) client = accept(servesock, NULL, NULL);
            char buffer[10];
            memset(buffer, 0 ,sizeof(buffer));
            
            bool flag = true;
            while (flag)
            {
                if (read(client, buffer, sizeof(buffer) ) > 0)
                {
                    if(quitjudge(buffer)){
                        strcpy(buffer, "q\0");
                        if(write(client, buffer, sizeof(char)*strlen(buffer) ) );
                        break;
                    }
                    else if(sendfile(buffer)){
                        strcpy(buffer, "file\0");
                        if(write(client, buffer, sizeof(char)*strlen(buffer) ) );
                        FILE* fp;
                        fp = fopen(std::format("{}.txt",k).c_str(), "rb");
                        if(fp == NULL){
                            printf("no such a file, send an empty one\n");
                            memset(buffer, 0, sizeof(char)*strlen(buffer));
                            if(write(client, buffer, sizeof(char)*strlen(buffer) ) );
                        } 
                        else{
                            printf("sendfile..................\n");
                            int read_count = fread((void*)buffer, sizeof(char), sizeof(buffer), fp);
                            //if(10 > read_count){
                                if(write(client, buffer, read_count) );
                            //}
                            //else write(client, buffer, 10) ;
                            printf("send file done\n");
                            fclose(fp);
                        } 
                    }
                    else{
                        write(filefd, buffer, strlen(buffer));
                        flag = false;
                        std::cout << "recive message from " << client << ":" << buffer << std::endl;
                        // printf("recive message from %d:%s \n",client, buffer);
                        // memset(buffer, 0, sizeof(char)*strlen(buffer));
                        // strcpy(buffer, "vastina\n");
                        // if (-1 == write(client, buffer, sizeof(char)*strlen(buffer)))
                        // {
                        //     errorhandling("fail to send with code:{} \n", errno );
                        // }
                    } 
                    memset(buffer, 0, sizeof(char)*strlen(buffer));
                }
            }
            close(servesock);
            return k*k;
        }) );
    }

    for(int i = 0; i < 4; ++i){
        tasks.emplace_back(std::packaged_task<int(int)>( [&servesock](int k)->int{
            std::this_thread::sleep_for(std::chrono::seconds(1));

            int clientsock = CreateClientSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            char buffer[10];
            for(int i=0; i<10; i++){
                buffer[i] = 'a'+rand()%26;
            }   buffer[10-1] = 0;
            write(clientsock, buffer, 10);
            
            return k*k;
        }) );
    }

    for(int count=0;auto& task: tasks)
    {
        ++count;
        results.emplace_back(task.get_future() );
        workers.emplace_back(
            [count, &task] 
            { 
                task(count); 
            } 
        );
    }
    
    for(auto& worker: workers) worker.join();
    for(int count=0;auto& result:results)
        print("({}, {})\n", ++count,result.get());

    return 0;
}