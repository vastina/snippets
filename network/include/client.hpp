#ifndef _CLIENT_SOCK_H_
#define _CLIENT_SOCK_H_

#include <format>
#include <strings.h>
#include <sys/socket.h>
#include <sys/unistd.h>
#include <arpa/inet.h>
#include <mutex>
#include <deque>

#include <SFML/Graphics.hpp>

#include "tools.h"
#include "loger.hpp"
#include "Taskpool.hpp"


static const char* localaddr = "127.0.0.1";

class client{
private:
    int clientsock;
    char readbuffer[BUFSIZ], sendbuffer[BUFSIZ];
    Taskpool tpool;
    std::condition_variable condition;
    struct _stop{
        bool stop;
        std::mutex smutex;
    } stop_;


    //ChatBox *chatbox;
    struct _chatbox{
        std::deque<std::string> messages;
        std::size_t maxMessages = 20; 
    } chatbox;
    constexpr static unsigned int fontSize = 15;
public:

    void init();
    void setclientsock(int af,int type,int protocol, short port);
    void connect_(int af,int type,int protocol, unsigned short port);
    template<typename F, typename...Args>
        void submit(F&& f, Args&&... args, IMPORTANCE level);

    //void manage(const char* request);
    void send_message();
    //void getfile(const char* filename);
    bool recive_message();
    void deal_message(sf::RenderWindow& window, const sf::Font& font);

    void sender();
    void reader();
    void fucksfml(std::string message,sf::RenderWindow& window, const sf::Font& font);
    
    inline void end(){
        tpool.setstoped();
        tpool.lastwork();
        close(clientsock);
    }

};

inline void client::sender(){
    while(true){
        printf("\nsend message> ");
        scanf("%s", sendbuffer);
        {
            if(strcmp(sendbuffer, EXIT_STR.data()) == 0)
            {
                std::unique_lock<std::mutex> lock(stop_.smutex);
                stop_.stop = true;
            }
        }
        if(stop_.stop) return;
        send_message();
        bzero(sendbuffer, BUFSIZ);
        showtime();
    }  
}

inline void client::reader(){
    sf::RenderWindow window{ sf::RenderWindow(sf::VideoMode(800,600), "recevie msg") };  
    sf::Font font;
    window.display();
    if (!font.loadFromFile("/usr/share/fonts/truetype/ubuntu/Ubuntu-Th.ttf"))
        throw std::runtime_error("can not load Ubuntu-Th.ttf"); 
    while (window.isOpen()) {
        //window.clear();
        if(recive_message()){
            deal_message(window, font);
        }
        {
            std::unique_lock<std::mutex> lock(stop_.smutex);
            if(stop_.stop) break;
        }
    }
    
    window.close();
}



inline void client::send_message(){
    if (-1 == write(clientsock, sendbuffer, strlen(sendbuffer)))
    {
        submit([]{
            vastina_log::logtest(std::format("fail to send with code: {}\n", errno).c_str());
        }, IMPORTANCE::lowest);
    } 
}

inline bool client::recive_message(){
    int bufferlen = -1;
    {
        if( (bufferlen = read(clientsock, (void*)readbuffer, sizeof(readbuffer))) > 0)
        {
            readbuffer[bufferlen] = '\0';
        }
    }
    if(bufferlen < 0) return false;
    return true;
}

inline void client::deal_message(sf::RenderWindow& window, const sf::Font& font){
    fucksfml(std::format("recive message: {} \t", readbuffer), window, font);
    memset(readbuffer, 0, sizeof(readbuffer));
}

inline void client::fucksfml(std::string message,sf::RenderWindow& window, const sf::Font& font){
    gettimebystring(message);
    chatbox.messages.push_back(message);
    if (chatbox.messages.size() > chatbox.maxMessages) {
        chatbox.messages.pop_front();
    }
    window.clear();
    for (std::size_t i = 0; i < chatbox.messages.size(); ++i) {
        sf::Text text(chatbox.messages[i], font, fontSize);
        text.setPosition(10.0f, float(i * (2*fontSize + 5) + 10));
        window.draw(text) ;
    }
    window.display(); 
}


inline void client::init(){
    memset(readbuffer, 0, sizeof(readbuffer));
    memset(sendbuffer, 0, sizeof(readbuffer));
    //chatbox = new ChatBox();
    tpool.start();
}

inline void client::setclientsock(int af,int type,int protocol, short port = -1){
    clientsock = socket(af, type, protocol);
    struct sockaddr_in clientaddr;
    memset(&clientaddr, 0, sizeof(clientaddr));
    clientaddr.sin_family = af;
    clientaddr.sin_addr.s_addr = inet_addr(localaddr);
    if(port == -1)
	clientaddr.sin_port = htons(INADDR_ANY) ;
    else clientaddr.sin_port = htons(port);
    if(bind(clientsock, (struct sockaddr*)&clientaddr, sizeof(clientaddr))== -1){
        errorhandling("bind fail error code: {}\n", errno);
        submit([]{
            vastina_log::logtest("bind fail");
        }, IMPORTANCE::lowest);
    }   
}

inline void client::connect_(int af,int type,int protocol, unsigned short port = 8888){
    struct sockaddr_in serveaddr;
    memset(&serveaddr, 0, sizeof(serveaddr));
    serveaddr.sin_family = AF_INET;
    serveaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serveaddr.sin_port = htons(port) ;
    if(connect(clientsock, (struct sockaddr*)&serveaddr, sizeof(serveaddr)) == -1){
        submit([]{
            vastina_log::logtest(std::format("connect fail {}\n", errno).c_str());
        }, IMPORTANCE::lowest);
    }
}

template<typename F, typename...Args>
void client::submit(F&& f, Args&&... args, IMPORTANCE level){
    if(level == IMPORTANCE::maintk){
        //todo
    }
    else tpool.submittask(f, args..., level);
};

#endif