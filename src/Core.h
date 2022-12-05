#pragma once
#include <SFML/Graphics.hpp>

class Core
{
public:
    Core() = delete;
    inline static sf::RenderWindow* window;
    inline static sf::RenderTexture* texture;
    inline static unsigned short majorVersion = 0;
    inline static unsigned short minorVersion = 2;
};


