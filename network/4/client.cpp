#include <vector>
#include <future>

#include "../include/tools.h"
#include "../include/client.hpp"

constexpr unsigned short PORT{1644};
std::vector< std::future<int> > results{};
std::vector< std::thread> workers{};
std::vector< std::packaged_task<int()> > tasks{};

int main(int argc, char** argv){

    vastina_log::logtest("start");

    client *client_ = new client();

    client_->init();

    client_->setclientsock(AF_INET, SOCK_STREAM, IPPROTO_TCP, -1);
    client_->connect_(AF_INET, SOCK_STREAM, IPPROTO_TCP, argc > 1 ? std::atoi(argv[1]) : PORT);

    tasks.emplace_back(([&client_]()->int{
        client_->reader();
        return 0;
    } ) );

    tasks.emplace_back(([&client_]()->int{
        client_->sender();
        return 0;
    } ) );


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

    client_->end();

    return 0;
}
