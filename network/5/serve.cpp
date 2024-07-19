#include <chrono>
#include <cstring>
#include <mutex>
#include <strings.h>
#include <vector>
#include <thread>
#include <future>

#include "tools.h"

constexpr unsigned short PORT{1644};

std::vector< std::future<int> > results{};
std::vector< std::thread> workers{};
std::vector< std::packaged_task<int()> > tasks{};

template<typename ty>
struct safe_data{
    ty data_;
    std::mutex mt_;

    const ty &getter(){
        std::unique_lock<std::mutex> lock(mt_);
        return data_;
    }
    void setter(const ty& data){
        std::unique_lock<std::mutex> lock(mt_);
        data_ = data;
    }
};

int main(){
    int servesock = CreateSeverSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, PORT);
    safe_data<bool> controler{true, {}};
    safe_data<int> client{0, {}};

    tasks.emplace_back(std::packaged_task<int()>( [&client ,servesock, &controler]()->int{

        
        while(!client.getter()) client.setter(accept(servesock, NULL, NULL)) ;

        char buffer[BUFSIZ];
        memset(buffer, 0 ,sizeof(buffer));

        auto start = std::chrono::system_clock::now();

        while (controler.getter())
        {
            auto res = read(client.getter(), buffer, sizeof(buffer) );
            if ( res > 0){
                print("recive message from {}:{} \n",client.getter(), buffer);

                if(0 == strcmp(buffer, EXIT_STR.data())){
                    controler.setter(false);
                    break;
                }

                if (-1 == write(client.getter(), buffer, sizeof(buffer))){
                    errorhandling("fail to send with code:{} \n", errno );
                    return -1;
                }   

                bzero(buffer, sizeof(buffer)); 
            }
            else if(res < 0){
                errorhandling("fail to read with code:{} \n", errno );
                return -1;
            }
        }
        close(servesock);
        return 0;
    }) );

    tasks.emplace_back(std::packaged_task<int()>( [&client ,servesock, &controler](){

        // data race here
        while(!client.getter());

        while(controler.getter()){
            if (-1 == write(client.getter(), "123456", 7)){
                errorhandling("fail to send with code:{} \n", errno );
                return -1;
            }
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }

        return 0;
    }));

    for(auto&& task: tasks)
    {
        results.emplace_back(task.get_future() );
        workers.emplace_back(
            [&task] 
            {
                task(); 
            }
        );
    }
    
    for(auto&& worker: workers) worker.join();
    
    for(int count=0;auto& result:results)
        print("({}, {})\n", ++count,result.get());

    return 0;
}