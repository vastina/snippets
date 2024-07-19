#ifndef _CHATBOX_H_
#define _CHATBOX_H_

#include <deque>
#include <SFML/Graphics.hpp>//put deque after this will cause compile problem
#include <string>

#include "tools.h"

class ChatBox {
//private:
public:
    std::deque<std::string> messages;
    std::size_t maxMessages ; 
    const unsigned int fontSize = 10;
    //sf::RenderWindow window;    sf::Font font;
    void addMessage(const std::string &message) ;
    void draw(sf::RenderWindow& window, const sf::Font& font) ;
//public:
    ChatBox(): messages{ },maxMessages{20}
        //window{sf::RenderWindow(sf::VideoMode(800, 600), "Chat Box")}
    { //if (!font.loadFromFile("/usr/share/fonts/truetype/arial.ttf"))
        //throw std::runtime_error("can not load arial.ttf"); 
    }

    //void addnewMessage(std::string message) ;
    void addnewMessage(std::string message,sf::RenderWindow& window, const sf::Font& font);

};

inline void ChatBox::addMessage(const std::string &message) {
    messages.push_back(message);
    if (messages.size() > maxMessages) {
        messages.pop_front();
    }
}

inline void ChatBox::draw(sf::RenderWindow& window, const sf::Font& font) {
    for (std::size_t i = 0; i < messages.size(); ++i) {
        sf::Text text(messages[i], font, fontSize);
        text.setPosition(10.0f, float(i * (fontSize + 5) + 10));
        window.draw(text) ;
    }
}   

inline void ChatBox::addnewMessage(std::string message,sf::RenderWindow& window, const sf::Font& font){
    // std::string receivedMessage="";
    // for(size_t i=0; i<strlen(message); ++i) receivedMessage += message[i];
    gettimebystring(message);
    messages.push_back(message);
    if (messages.size() > maxMessages) {
        messages.pop_front();
    }
    //window.clear();
    for (std::size_t i = 0; i < messages.size(); ++i) {
        sf::Text text(messages[i], font, fontSize);
        text.setPosition(10.0f, float(i * (fontSize + 5) + 10));
        window.draw(text) ;
    }
    //window.display(); 
}



#endif