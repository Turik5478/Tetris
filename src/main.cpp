#include <SFML/Graphics.hpp>
#include "Tile.h"
#include "Global.h"
#include "Core.h"
#include "DiscordStatus.h"

int main()
{
    DiscordStatus::getInstance()->Start();
    sf::RenderWindow window(sf::VideoMode(2 * CELL_SIZE * COLUMNS * SCREEN_RESIZE, CELL_SIZE * ROWS * SCREEN_RESIZE),
            "Tetris T.V. Edition", sf::Style::Close);
    window.setFramerateLimit(60);
    Core::window = &window;

    sf::Image icon;
    icon.loadFromFile("./resources/icon.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    sf::RectangleShape backgr;
    backgr.setSize({CELL_SIZE * SCREEN_RESIZE, CELL_SIZE * SCREEN_RESIZE});
    sf::Color backgrColor(40, 40, 40);
    backgr.setFillColor(backgrColor);
    backgr.setOutlineThickness(SCREEN_RESIZE);
    backgr.setOutlineColor(sf::Color::Black);

    sf::RectangleShape NextTile;
    NextTile.setPosition(CELL_SIZE * COLUMNS * SCREEN_RESIZE + 3 * CELL_SIZE * SCREEN_RESIZE, CELL_SIZE *
                                                                                              SCREEN_RESIZE);
    NextTile.setSize({5 * CELL_SIZE * SCREEN_RESIZE, 5 * CELL_SIZE * SCREEN_RESIZE});
    NextTile.setOutlineThickness(SCREEN_RESIZE);
    NextTile.setOutlineColor(sf::Color(40, 40, 40));

    sf::Event event;

    sf::Font font;
    font.loadFromFile("./resources/fonts/Ticketing.ttf");
    sf::Text scoreInfo;
    scoreInfo.setFont(font);
    scoreInfo.setCharacterSize(100);
    scoreInfo.setPosition(CELL_SIZE * COLUMNS * SCREEN_RESIZE + 2 * CELL_SIZE * SCREEN_RESIZE, 10 * CELL_SIZE *
                                                                                               SCREEN_RESIZE);
    sf::Text mScoreInfo = scoreInfo;
    mScoreInfo.move(0, scoreInfo.getCharacterSize());

    sf::Text version;
    version.setString("Current version " + std::to_string(Core::majorVersion) + "." + std::to_string
            (Core::minorVersion));
    version.setFont(font);
    version.setPosition(window.getSize().x - version.getGlobalBounds().width - SCREEN_RESIZE,
            window.getSize().y - version
                    .getCharacterSize());
    version.setFillColor(sf::Color(121, 121, 121));

    Tile::loadMaxScore();

    sf::RenderTexture renderTexture;
    renderTexture.create(window.getSize().x, window.getSize().y);
    Core::texture = &renderTexture;

    while (window.isOpen())
    {
        DiscordStatus::getInstance()->setTopText("Score: " + std::to_string(Tile::score));
        DiscordStatus::getInstance()->setBottomText("MAX Score: " + std::to_string(Tile::mScore));
        DiscordStatus::getInstance()->update();
        if (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
        }
        if (!Tile::lose)
        {
            Tile::getLastInstance()->onEvent(event);
            Tile::getLastInstance()->onUpdate();
        }
        else
            Tile::loseAnimation();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
            Tile::newGame();

        scoreInfo.setString("Score  " + std::to_string(Tile::score));
        mScoreInfo.setString("MAX  " + std::to_string(Tile::mScore));
        window.clear();
        Core::texture->clear();
        for (int i = 0; i < COLUMNS; ++i)
            for (int j = 0; j < ROWS; ++j)
            {
                backgr.setPosition(i * CELL_SIZE * SCREEN_RESIZE, j * CELL_SIZE * SCREEN_RESIZE);
                Core::texture->draw(backgr);
            }
        for (auto i: Tile::getMegaTile())
            Core::texture->draw(*i);
        for (int i = COLUMNS + 5; i < 2 * COLUMNS - 5; ++i)
            for (int j = 1; j < 6; ++j)
            {
                backgr.setPosition(i * CELL_SIZE * SCREEN_RESIZE, j * CELL_SIZE * SCREEN_RESIZE);
                Core::texture->draw(backgr);
            }
        for (int i = 0; i < 4; ++i)
            Core::texture->draw(Tile::nextFigures[i]);
        Core::texture->draw(scoreInfo);
        Core::texture->draw(mScoreInfo);
        Core::texture->draw(version);
        Core::texture->display();
        sf::Sprite sprite(Core::texture->getTexture());
        window.draw(sprite);
        window.display();
    }
    Tile::saveMaxScore();
    DiscordStatus::getInstance()->Stop();
}