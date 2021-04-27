//
// Created by benmo on 2/14/2020.
//

#ifndef SFML_TEMPLATE_SPRITE_H
#define SFML_TEMPLATE_SPRITE_H

#include <string>
#include <SFML/Graphics.hpp>
#include <cmath>

#ifndef INFINITE
#define INFINITE -1
#endif

#ifdef VISIBLE
#define VISIBLE -2
#endif

class GameSprite: public sf::Sprite{
protected:
    struct Physics {
        float velocity, direction, acceleration, maxVelocity;
        float rotVelocity, rotAcceleration, maxRotVelocity;
        float xPos, yPos;
    };

    Physics spritePhysics{};

    bool isAnimated = false;
    int currentFrame = 0, frameDelay = 0, currentRow = 0, currentCol = 0, rows, cols, xOffset, yOffset;
    sf::IntRect initRect;

    int lifetime = INFINITE;
    int framesAlive = 0;

    void nextTexture();
public:
    constexpr static const double PI = 3.1415926;

    static double distance(sf::Vector2f pos1, sf::Vector2f pos2) {
        return sqrt(pow(pos2.x - pos1.x, 2) + pow(pos2.y - pos1.y, 2));
    }

    /*
     * Contructors
     */

    GameSprite();
    explicit GameSprite(const sf::Texture &texture);
    GameSprite(const sf::Texture &texture, float scale);
    GameSprite(const sf::Texture &texture, const sf::IntRect &rectangle);
    GameSprite(const sf::Texture &texture, float scale, float xPos, float yPos, float velocity, float direction);
    GameSprite(const sf::Texture &texture, float scale, float xPos, float yPos, float velocity, float maxVelocity, float direction, float rotVelocity, float maxRotVelocity);
    GameSprite(const sf::Texture &texture, float scale, float xPos, float yPos, float velocity, float acceleration, float maxVelocity, float direction, float rotVelocity, float rotAcceleration, float maxRotVelocity);
    GameSprite(const sf::Texture &texture, float scale, float xPos, float yPos, float velocity, float maxVelocity, float direction);
    GameSprite(const sf::Texture& texture, const sf::IntRect &rect, int _rows, int _cols, int _xOffset, int _yOffset, int frameDelay);

    void init();

    /**
     * Updates sprite's position, direction, velocities, etc. every tick based on its Physics struct
     */
    void update();
    void updateAnimation(bool override = false);

    /*
     * Helper functions for update()
     */
    void calculateNewDirection();
    void calculateNewPosition();

    /**
     * Accelerates ship by acceleration stat, can be overridden
     * @param override - Used to override acceleration stat default 0
     * @param ignoreMax - When false, maximum velocity will be honored when calculating new velocity default false
     */
    void accelerate(float override = 0, bool ignoreMax = false);

    /**
     * Accelerates ship rotationally by rotational acceleration stat, can be overridden
     * @param override - Used to override acceleration stat default 0
     * @param ignoreMax - When false, maximum rotational velocity will be honored when calculating new velocity default false
     */
    void rotAccel(float override = 0, bool ignoreMax = false);
    void turn(float degrees);

    float getXPos() const;
    float getYPos() const;

    void setPosition(float xPos, float yPos);
    void setPosition(const sf::Vector2f &vec);

    float getDirection() const;
    void setDirection(float angle);

    void setVelocity(float velo);
    float getVelocity() const;

    static double getAimAngle(const Sprite& b, const Sprite& a);
    static double getAimAngle(sf::Vector2f b, sf::Vector2f a);

    int getFramesAlive() const;
    bool isPastLifetime() const;
    int getLifetime() const;
};


#endif //SFML_TEMPLATE_SPRITE_H
