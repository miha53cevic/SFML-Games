//
// Author: Mihael Petricevic
// Contact: miha53cevic@gmail.com
// Date: 22.5.2019.
//

#include <vector>

#include "mihaSimpleSFML.h"
#include "../Server/UdpHandler.hpp"

class Client : public mihaSimpleSFML
{
public:
    Client(sf::IpAddress adress, unsigned short int port)
    {
        handler.setBlocking(false);

        // Open any free port and send a connection request to server
        handler.try_connect(sf::Socket::AnyPort);
        handler.ConnectionRequest(adress, port);

        if (handler.WaitForConnection(15))  connected = true;
        else                                connected = false;
    }

private:
    UdpHandler handler;

    bool connected;

    sf::CircleShape     ball;
    sf::RectangleShape  player;

    sf::Vector2f        lastPlayerPos;
    
    unsigned int        Scores[2];

    sf::RectangleShape  enemy;

    sf::Font font;
    sf::Text text;

    bool OnUserCreate() override
    {
        // Setup font
        if (!font.loadFromFile("Client/fonts/arial.ttf"))
    	{
	    	// error...
	    }

        // Setup text
	    text.setFont(font);
	    text.setCharacterSize(64);
        text.setFillColor(sf::Color::Black);
	    text.setOutlineColor(sf::Color::White);
	    text.setOutlineThickness(2);
	
	    text.setPosition(ScreenWidth() / 2, ScreenHeight() / 2 - 64);
        text.setString("0");

        // Setup game
        ball.setFillColor(sf::Color::Yellow);
        ball.setRadius(10.0f);
        ball.setOrigin(ball.getRadius(), ball.getRadius());
        ball.setPosition(ScreenWidth() / 2, ScreenHeight() / 2);
 
        player.setFillColor(sf::Color::White);
        player.setSize(sf::Vector2f(20, 100));

        enemy.setFillColor(sf::Color::White);
        enemy.setSize(sf::Vector2f(20, 100));
        enemy.setPosition(sf::Vector2f(ScreenWidth() - 20, 0));

        return true;
    }

    bool OnUserUpdate(sf::Time elapsed) override
    {
        // If a connection has been established
        if (connected)
        {
            // Get ballPos from server
            sf::Packet data;
            handler.Receive(data);  

            float x, y;
            if (data >> x)
            {
                if (data >> y)
                {
                    ball.setPosition(x, y);

                    int score;
                    if (data >> Scores[0])
                    {
                        if (data >> Scores[1])
                        {
                            if (data >> x)
                            {
                                if (data >> y)
                                {
                                    enemy.setPosition(x, y);
                                }
                            }
                        }
                    }
                }
            }

            // Send player pos to server
            // only if the player has changed his position
            if (lastPlayerPos != player.getPosition())
            {
                data.clear();
                data << player.getPosition().x  << player.getPosition().y;
                handler.Send(data, *handler.getIPAdress(), *handler.getPort());

                // last send position
                lastPlayerPos = player.getPosition();
            }
        }

        // Controls
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        {
            if (player.getPosition().y >= 0)
                player.move(0, -200.0f * elapsed.asSeconds());
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        {
            if (player.getPosition().y + player.getSize().y < ScreenHeight())
                player.move(0, 200.0f * elapsed.asSeconds());
        }

        // Drawing routines
        Draw(ball);
        Draw(player);
        Draw(enemy);
        
        // Drawing score
        text.setPosition(sf::Vector2f(ScreenWidth() / 2 - 64, ScreenHeight() / 5));
        text.setString(std::to_string(Scores[0]));
        Draw(text);

        text.setPosition(sf::Vector2f(ScreenWidth() / 2 + 64, ScreenHeight() / 5));
        text.setString(std::to_string(Scores[1]));
        Draw(text);

        return true;
    }
};