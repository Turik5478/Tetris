#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Tile: public sf::Drawable
{
public:
    enum FigureType
    {
        TShape = 0, JShape, IShape, OShape, LShape, ZShape, SShape
    };
    Tile();
    void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void onEvent(sf::Event& event);
    void onUpdate();
    static void newGame();
    static void loseAnimation();
    static void saveMaxScore();
    static Tile* getLastInstance();
    static std::vector<Tile*>& getMegaTile();
    inline static bool lose = false;
    static bool loadMaxScore();
    inline static int score = 0;
    inline static int mScore = 0;
    inline static int colorNum = 1;
    inline static std::vector<sf::RectangleShape> nextFigures;
private:
    float moveToBorder();
    int checkLines();
    void clearLine(int row, std::vector<std::pair<int,int>>& toDelete);
    void ghostFiguresAnimate();
    void moveDown(std::vector<sf::RectangleShape>& Figures);
    void nextFigure(sf::Color color);
    void clearLineAnimation(std::vector<std::pair<int, int>>& toAnimate);
    bool isOutOfBorder();
    bool ghostIsOutOfScreen();
    bool intersects(int index, std::vector<sf::RectangleShape>& Figures);
    std::vector<sf::RectangleShape>& getFigures();
    void setPositions(FigureType& type);
    inline static std::vector<Tile*> megaTile;
    std::vector<sf::RectangleShape> figures;
    std::vector<sf::RectangleShape> ghostFigures;
    sf::Vector2f positions[4];
    sf::Vector2f velocity;
    sf::RectangleShape shape;
    enum Direction
    {
        Left, Right, None
    };
    FigureType figureType;
    inline static FigureType nextFigureType = TShape;
    Direction direction = None;
    Direction rotateDirection = None;
    sf::Clock movementReload;
    sf::Clock reloadL;
    sf::Clock reloadR;
    sf::Clock reloadRL;
    sf::Clock reloadRR;
    inline static sf::Color nextColor = sf::Color(255, 0, 0);
    bool skip = false;
    bool ghostFiguresDraw = true;
    inline static bool tvoiPerviyRaz = false;
    inline static sf::SoundBuffer difficultyBuffer, fallBuffer, loseBuffer, clearLineBuffer;
    inline static sf::Sound difficultySound, fallSound, loseSound, clearLineSound;

};
