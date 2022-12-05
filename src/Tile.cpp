#include <fstream>
#include <algorithm>
#include <cmath>
#include "Tile.h"
#include "Global.h"
#include "Core.h"

Tile::Tile()
{
    static bool isInit = false;
    if (!isInit) {
        fallBuffer.loadFromFile("./resources/music/fall.wav");
        loseBuffer.loadFromFile("./resources/music/lose.wav");
        clearLineBuffer.loadFromFile("./resources/music/clearLine.wav");
        difficultySound.setBuffer(difficultyBuffer);
        fallSound.setBuffer(fallBuffer);
        loseSound.setBuffer(loseBuffer);
        clearLineSound.setBuffer(clearLineBuffer);
        fallSound.setVolume(40);
        loseSound.setVolume(10);
        clearLineSound.setVolume(20);
        isInit = true;
    }

    figureType = nextFigureType;
    colorNum = rand() % 7 + 1;
    sf::Color currentColor = nextColor;

    switch (colorNum) {
        case 1: {
            nextColor = sf::Color(255, 0, 0);
            break;
        }

        case 2: {
            nextColor = sf::Color(255, 165, 0);
            break;
        }
        case 3: {
            nextColor = sf::Color(255, 255, 0);
            break;
        }
        case 4: {
            nextColor = sf::Color(0, 255, 0);
            break;
        }
        case 5: {
            nextColor = sf::Color(2, 187, 253);
            break;
        }
        case 6: {
            nextColor = sf::Color(0, 0, 255);
            break;
        }
        case 7: {
            nextColor = sf::Color(255, 0, 255);
            break;
        }
    }

    shape.setFillColor(currentColor);
    shape.setSize({CELL_SIZE * SCREEN_RESIZE - SCREEN_RESIZE, CELL_SIZE * SCREEN_RESIZE - SCREEN_RESIZE});
    shape.setOutlineThickness(SCREEN_RESIZE);
    shape.setOutlineColor(sf::Color::Black);

    for (int i = 0; i < 4; ++i)
        figures.push_back(shape);

    shape.setFillColor(sf::Color(110, 110, 110));

    for (int i = 0; i < 4; ++i)
        ghostFigures.push_back(shape);

    setPositions(figureType);

    for (int i = 0; i < 4; ++i) {
        figures[i].setPosition((positions[i].x + 4) * CELL_SIZE * SCREEN_RESIZE,
                               (positions[i].y - 1) * CELL_SIZE * SCREEN_RESIZE);
        ghostFigures[i].setPosition((positions[i].x + 4) * CELL_SIZE * SCREEN_RESIZE,
                                    (positions[i].y - 1) * CELL_SIZE * SCREEN_RESIZE);
        if (megaTile.size() > 1)
            if (intersects(i, figures)) {
                mScore = std::max(score, mScore);
                lose = true;
            }
    }
    nextFigure(nextColor);
}

void Tile::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (ghostFiguresDraw)
        for (int i = 0; i < ghostFigures.size(); ++i)
            target.draw(ghostFigures[i], states);
    for (int i = 0; i < figures.size(); ++i)
        target.draw(figures[i], states);
}

void Tile::onEvent(sf::Event& event)
{
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        && reloadL.getElapsedTime().asSeconds() > 0.1) {
        direction = Left;
        reloadL.restart();
    }
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    && reloadR.getElapsedTime().asSeconds() > 0.1) {
        direction = Right;
        reloadR.restart();
    }
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Q) || (event.type == sf::Event::MouseWheelMoved)
    && event.mouseWheel.delta > 0) && reloadRL.getElapsedTime().asSeconds() > 0.2) {
        if (figureType != OShape)
            rotateDirection = Left;
        reloadRL.restart();
    }
    if ((sf::Keyboard::isKeyPressed(sf::Keyboard::E) || (event.type == sf::Event::MouseWheelMoved)
    && event.mouseWheel.delta < 0) && reloadRR.getElapsedTime().asSeconds() > 0.2) {
        if (figureType != OShape)
            rotateDirection = Right;
        reloadRR.restart();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
    sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
        skip = true;
}

void Tile::onUpdate()
{
    checkLines();

    ghostFiguresAnimate();

    if (direction == Left)
        velocity.x = -CELL_SIZE * SCREEN_RESIZE;
    if (direction == Right)
        velocity.x = CELL_SIZE * SCREEN_RESIZE;

    Tile miniTile = *this;
    for (int i = 0; i < figures.size(); ++i) {
        miniTile.getFigures()[i].move(velocity.x, 0);
        if (miniTile.intersects(i, miniTile.getFigures()))
            velocity.x = 0;
    }

    float fallingReload = 0.5;
    static int musicType = -1;
    if (score < 75) {
        if (musicType != 0) {
            difficultyBuffer.loadFromFile("./resources/music/difficulty0.wav");
            difficultySound.setVolume(5);
            difficultySound.play();
            musicType = 0;
        }
    }
    else if (score >= 225) {
        fallingReload = 0.1;
        if (musicType != 3) {
            difficultyBuffer.loadFromFile("./resources/music/difficulty3.wav");
            difficultySound.setVolume(10);
            difficultySound.play();
            musicType = 3;
        }
    }
    else if (score >= 150) {
        fallingReload = 0.15;
        if (musicType != 2) {
            difficultyBuffer.loadFromFile("./resources/music/difficulty2.wav");
            difficultySound.setVolume(3);
            difficultySound.play();
            musicType = 2;
        }
    }
    else if (score >= 75) {
        fallingReload = 0.3;
        if (musicType != 1) {
            difficultyBuffer.loadFromFile("./resources/music/difficulty1.wav");
            difficultySound.setVolume(10);
            difficultySound.play();
            musicType = 1;
        }
    }

    static bool lastSkip = skip;
    if (skip) {
        if (lastSkip != skip)
            fallSound.play();
        lastSkip = skip;
        skip = false;
        velocity.y = CELL_SIZE * SCREEN_RESIZE;
    }
    else if (movementReload.getElapsedTime().asSeconds() > fallingReload) {
        velocity.y = CELL_SIZE * SCREEN_RESIZE;
        movementReload.restart();
        lastSkip = skip;
    }

    for (int i = 0; i < figures.size(); ++i) {
        if (intersects(i, figures) && !lose) {
            ++score;
            rotateDirection = None;
            velocity.y = 0;
            megaTile.push_back(new Tile);
            ghostFiguresDraw = false;
            return;
        }
    }

    miniTile = *this;
    for (int i = 0; i < figures.size(); ++i) {
        if (rotateDirection == Left) {
            float x2 = figures[1].getPosition().x + figures[1].getPosition().y - figures[i].getPosition().y;
            float y2 = figures[i].getPosition().x + figures[1].getPosition().y - figures[1].getPosition().x;
            miniTile.getFigures()[i].setPosition(x2, y2);
        }
        else if (rotateDirection == Right) {
            float x2 = figures[1].getPosition().x + figures[1].getPosition().y - figures[i].getPosition().y;
            float y2 = figures[i].getPosition().x + figures[1].getPosition().y - figures[1].getPosition().x;
            miniTile.getFigures()[i].setPosition(x2, y2);
        }

        if (miniTile.intersects(i, miniTile.getFigures()))
            rotateDirection = None;
    }

    if (rotateDirection == Left)
        for (int i = 0; i < figures.size(); ++i) {
            float x2 = figures[1].getPosition().x + figures[1].getPosition().y - figures[i].getPosition().y;
            float y2 = figures[i].getPosition().x + figures[1].getPosition().y - figures[1].getPosition().x;
            figures[i].setPosition(x2, y2);
            float x2G =
                    ghostFigures[1].getPosition().x + ghostFigures[1].getPosition().y - ghostFigures[i].getPosition
                                                                                                               ().y;
            float y2G =
                    ghostFigures[i].getPosition().x + ghostFigures[1].getPosition().y - ghostFigures[1].getPosition
                                                                                                               ().x;
            ghostFigures[i].setPosition(x2G, y2G);
        }

    if (rotateDirection == Right)
        for (int j = 0; j < 3; ++j)
            for (int i = 0; i < figures.size(); ++i) {
                float x2 = figures[1].getPosition().x + figures[1].getPosition().y - figures[i].getPosition().y;
                float y2 = figures[i].getPosition().x + figures[1].getPosition().y - figures[1].getPosition().x;
                figures[i].setPosition(x2, y2);
                float x2G = ghostFigures[1].getPosition().x + ghostFigures[1].getPosition().y - ghostFigures[i]
                        .getPosition().y;
                float y2G = ghostFigures[i].getPosition().x + ghostFigures[1].getPosition().y - ghostFigures[1]
                        .getPosition().x;
                ghostFigures[i].setPosition(x2G, y2G);
            }

    while (ghostIsOutOfScreen())
        for (int i = 0; i < ghostFigures.size(); ++i)
            ghostFigures[i].move(0, -CELL_SIZE * SCREEN_RESIZE);


    for (int i = 0; i < figures.size(); ++i) {
        figures[i].move(velocity.x, velocity.y);
        ghostFigures[i].move(velocity.x, 0);
    }

    if (isOutOfBorder()) {
        float maxTile = moveToBorder();
        if (maxTile > (CELL_SIZE * COLUMNS * SCREEN_RESIZE) / 2)
            for (int i = 0; i < figures.size(); ++i) {
                figures[i].move(CELL_SIZE * COLUMNS * SCREEN_RESIZE - maxTile - CELL_SIZE * SCREEN_RESIZE, 0);
                ghostFigures[i].move(CELL_SIZE * COLUMNS * SCREEN_RESIZE - maxTile - CELL_SIZE * SCREEN_RESIZE, 0);

            }
        if (maxTile < (CELL_SIZE * COLUMNS * SCREEN_RESIZE) / 2)
            for (int i = 0; i < figures.size(); ++i) {
                figures[i].move(-maxTile, 0);
                ghostFigures[i].move(-maxTile, 0);
            }
    }

    velocity.x = 0;
    velocity.y = 0;
    rotateDirection = None;
    direction = None;
}


float Tile::moveToBorder()
{
    float maxRight = CELL_SIZE * COLUMNS * SCREEN_RESIZE - CELL_SIZE * SCREEN_RESIZE;
    float minLeft = 0;
    bool rOut = false;
    bool lOut = false;
    for (int i = 0; i < figures.size(); ++i) {
        if (figures[i].getPosition().x >= CELL_SIZE * COLUMNS * SCREEN_RESIZE - SCREEN_RESIZE) {
            maxRight = std::max(figures[i].getPosition().x, maxRight);
            rOut = true;
        }
        if (figures[i].getPosition().x <= 0) {
            minLeft = std::min(figures[i].getPosition().x, minLeft);
            lOut = true;
        }
    }
    if (rOut)
        return maxRight;
    if (lOut)
        return minLeft;

    return (CELL_SIZE * COLUMNS * SCREEN_RESIZE) / 2;
}

int Tile::checkLines()
{
    for (int i = ROWS; i > 0; --i) {
        std::vector<std::pair<int, int>> figuresToDelete;
        int cells = 0;
        for (int k = 0; k < megaTile.size() - 1; ++k)
            for (int l = 0; l < megaTile[k]->getFigures().size(); ++l)
                if (megaTile[k]->getFigures()[l].getPosition().y == (i - 1) * CELL_SIZE * SCREEN_RESIZE) {
                    figuresToDelete.push_back(std::make_pair(k, l));
                    ++cells;
                }
        if (cells == COLUMNS) {
            clearLineAnimation(figuresToDelete);
            clearLine(i, figuresToDelete);
        }
    }
}


void Tile::clearLine(int row, std::vector<std::pair<int, int>>& toDelete)
{
    clearLineSound.play();
    for (int i = 0; i < COLUMNS; ++i)
        megaTile[toDelete[i].first]->getFigures()[toDelete[i].second].setFillColor(sf::Color::White);

    for (int k = 0; k < megaTile.size() - 1; ++k)
        for (int l = 0; l < megaTile[k]->getFigures().size(); ++l)
            if (megaTile[k]->getFigures()[l].getFillColor() == sf::Color::White) {
                megaTile[k]->getFigures().erase(megaTile[k]->getFigures().begin() + l);
                --l;
            }

    for (int k = 0; k < megaTile.size() - 1; ++k)
        for (int l = 0; l < megaTile[k]->getFigures().size(); ++l) {
            if (megaTile[k]->getFigures()[l].getPosition().y < (row - 1) * CELL_SIZE * SCREEN_RESIZE)
                megaTile[k]->getFigures()[l].move(0, CELL_SIZE * SCREEN_RESIZE);
        }
    score += 10;
}

void Tile::clearLineAnimation(std::vector<std::pair<int, int>>& toAnimate)
{
    std::sort(toAnimate.begin(), toAnimate.end(), [=](std::pair<int, int> left, std::pair<int, int> right) -> bool {
        return megaTile[left.first]->getFigures()[left.second].getPosition().x < megaTile[right.first]->getFigures()
        [right.second].getPosition().x;
    });
    auto mass = toAnimate;
    for (int i = 0; i < COLUMNS; ++i)
        mass[i] = toAnimate[COLUMNS / 2 + std::pow(-1, i + 2) * (i + 1) / 2];
    toAnimate = mass;
    for (int i = 0; i < COLUMNS; ++i) {
        sf::Sprite sprite;
        sprite.setTexture(Core::texture->getTexture());
        megaTile[toAnimate[i].first]->getFigures()[toAnimate[i].second].setFillColor(sf::Color(153, 230, 255));
        Core::texture->draw(megaTile[toAnimate[i].first]->getFigures()[toAnimate[i].second]);
        Core::texture->display();
        sprite.setTexture(Core::texture->getTexture());
        Core::window->draw(sprite);
        Core::window->display();
        sf::sleep(sf::seconds(0.015));
    }
    for (int i = 0; i < COLUMNS; ++i) {
        megaTile[toAnimate[i].first]->getFigures()[toAnimate[i].second].setFillColor(sf::Color::White);
        Core::texture->draw(megaTile[toAnimate[i].first]->getFigures()[toAnimate[i].second]);
    }
    Core::texture->display();
    sf::Sprite sprite;
    sprite.setTexture(Core::texture->getTexture());
    Core::window->draw(sprite);
    Core::window->display();
    sf::sleep(sf::seconds(0.1));
}

void Tile::loseAnimation()
{
    for (int k = 0; k < megaTile.size(); ++k)
        for (int l = 0; l < megaTile[k]->getFigures().size(); ++l)
            megaTile[k]->getFigures()[l].setFillColor(sf::Color(90, 90, 90));
    if (!tvoiPerviyRaz) {
        difficultySound.stop();
        loseSound.play();
        tvoiPerviyRaz = true;
    }
}

void Tile::newGame()
{
    loseSound.stop();
    lose = false;
    megaTile.clear();
    score = 0;
    tvoiPerviyRaz = false;
}

void Tile::ghostFiguresAnimate()
{
    for (int i = 0; i < ghostFigures.size(); ++i)
        ghostFigures[i].setPosition(figures[i].getPosition().x, figures[i].getPosition().y);
    moveDown(ghostFigures);

}

void Tile::moveDown(std::vector<sf::RectangleShape>& Figures)
{
    bool moveDown = true;
    while (moveDown) {
        for (int i = 0; i < Figures.size(); ++i) {
            if (intersects(i, Figures)) {
                moveDown = false;
                break;
            }
        }
        if (moveDown)
            for (int i = 0; i < Figures.size(); ++i)
                Figures[i].move(0, CELL_SIZE * SCREEN_RESIZE);
    }
}

void Tile::nextFigure(sf::Color color)
{
    nextFigures.clear();
    shape.setFillColor(color);
    for (int i = 0; i < 4; ++i)
        nextFigures.push_back(shape);
    nextFigureType = static_cast<FigureType>(rand() % 7);
    while (nextFigureType == figureType)
        nextFigureType = static_cast<FigureType>(rand() % 7);
    setPositions(nextFigureType);
    for (int i = 0; i < nextFigures.size(); ++i) {
        nextFigures[i].setPosition((positions[i].x + 19) * CELL_SIZE * SCREEN_RESIZE,
                                   (positions[i].y) * CELL_SIZE * SCREEN_RESIZE);
    }
}

void Tile::saveMaxScore()
{
    std::fstream maxScore;
    maxScore.open("./maxScore", std::ios::out | std::ios::trunc);
    maxScore << mScore;
    maxScore.close();
}

bool Tile::loadMaxScore()
{
    std::fstream maxScore;
    maxScore.open("./maxScore");
    if (!maxScore.is_open()) {
        std::ofstream f("./maxScore");
        f.close();
        maxScore.open("./maxScore");
    }

    if (maxScore.peek() == std::fstream::traits_type::eof()) {
        maxScore.close();
        return false;
    }
    maxScore >> mScore;
    maxScore.close();
    return true;
}

bool Tile::intersects(int index, std::vector<sf::RectangleShape>& Figures)
{
    if (Figures[index].getPosition().y + Figures[index].getSize().y >= CELL_SIZE * ROWS * SCREEN_RESIZE -
                                                                       SCREEN_RESIZE)
        return true;
    for (int j = 0; j < megaTile.size() - 1; ++j)
        for (int k = 0; k < megaTile[j]->getFigures().size(); ++k) {
            sf::FloatRect rect = Figures[index].getGlobalBounds();
            rect.width -= 10;
            rect.left += 5;
            if (megaTile[j]->getFigures()[k].getGlobalBounds().intersects(rect))
                return true;
        }
    return false;
}

bool Tile::isOutOfBorder()
{
    for (int i = 0; i < figures.size(); ++i) {
        if (figures[i].getPosition().x >= CELL_SIZE * COLUMNS * SCREEN_RESIZE - CELL_SIZE * SCREEN_RESIZE ||
            figures[i].getPosition().x <= 0)
            return true;
    }
    return false;
}

bool Tile::ghostIsOutOfScreen()
{
    for (int i = 0; i < ghostFigures.size(); ++i)
        if (ghostFigures[i].getPosition().y + ghostFigures[i].getSize().y > CELL_SIZE * ROWS * SCREEN_RESIZE -
                                                                            SCREEN_RESIZE)
            return true;
    return false;
}

Tile* Tile::getLastInstance()
{
    if (megaTile.empty())
        megaTile.push_back(new Tile);
    return megaTile.back();
}

std::vector<Tile*>& Tile::getMegaTile()
{
    return megaTile;
}

std::vector<sf::RectangleShape>& Tile::getFigures()
{
    return figures;
}

void Tile::setPositions(FigureType& type)
{
    switch (type) {
        case TShape: {

            positions[0].x = 2;
            positions[0].y = 3;

            positions[1].x = 3;
            positions[1].y = 3;

            positions[2].x = 4;
            positions[2].y = 3;

            positions[3].x = 3;
            positions[3].y = 2;

            break;
        }
        case JShape: {

            positions[0].x = 3;
            positions[0].y = 2;

            positions[1].x = 3;
            positions[1].y = 3;

            positions[2].x = 3;
            positions[2].y = 4;

            positions[3].x = 2;
            positions[3].y = 4;

            break;
        }
        case OShape: {

            positions[0].x = 2;
            positions[0].y = 3;

            positions[1].x = 3;
            positions[1].y = 3;

            positions[2].x = 2;
            positions[2].y = 4;

            positions[3].x = 3;
            positions[3].y = 4;

            break;
        }
        case IShape: {

            positions[0].x = 3;
            positions[0].y = 4;

            positions[1].x = 3;
            positions[1].y = 3;

            positions[2].x = 3;
            positions[2].y = 2;

            positions[3].x = 3;
            positions[3].y = 1;

            break;
        }
        case LShape: {

            positions[0].x = 3;
            positions[0].y = 2;

            positions[1].x = 3;
            positions[1].y = 3;

            positions[2].x = 3;
            positions[2].y = 4;

            positions[3].x = 4;
            positions[3].y = 4;

            break;
        }
        case ZShape: {

            positions[0].x = 2;
            positions[0].y = 3;

            positions[1].x = 3;
            positions[1].y = 3;

            positions[2].x = 3;
            positions[2].y = 4;

            positions[3].x = 4;
            positions[3].y = 4;

            break;
        }
        case SShape: {

            positions[0].x = 4;
            positions[0].y = 3;

            positions[1].x = 3;
            positions[1].y = 3;

            positions[2].x = 3;
            positions[2].y = 4;

            positions[3].x = 2;
            positions[3].y = 4;

            break;
        }
    }
}