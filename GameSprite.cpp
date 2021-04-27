#include <iostream>
#include "GameSprite.h"

void GameSprite::init() {
    spritePhysics.velocity = 0;
    spritePhysics.xPos = 0;
    spritePhysics.yPos = 0;
    spritePhysics.direction = 0;

    setOrigin(getGlobalBounds().width/2,getGlobalBounds().height/2);
}

GameSprite::GameSprite() : sf::Sprite() {
    init();
}

GameSprite::GameSprite(const sf::Texture &texture) : sf::Sprite(texture) {
    init();
}

GameSprite::GameSprite(const sf::Texture &texture, float scale) : sf::Sprite(texture) {
    init();

    setScale(scale/100, scale/100);
}

GameSprite::GameSprite(const sf::Texture &texture, const sf::IntRect &rectangle) : sf::Sprite(texture, rectangle) {
    init();
}

GameSprite::GameSprite(const sf::Texture &texture, float scale, float xPos, float yPos, float velocity, float direction) : sf::Sprite(texture) {
    spritePhysics.velocity = velocity;
    spritePhysics.xPos = xPos;
    spritePhysics.yPos = yPos;
    spritePhysics.direction = direction;

    setOrigin(getGlobalBounds().width/2,getGlobalBounds().height/2);
    setScale(scale/100, scale/100);
    setPosition(spritePhysics.xPos,spritePhysics.yPos);
    setRotation(direction);
}

GameSprite::GameSprite(const sf::Texture &texture, float scale, float xPos, float yPos, float velocity, float maxVelocity, float direction) : GameSprite(texture, scale, xPos, yPos, velocity, direction) {
    spritePhysics.maxVelocity = maxVelocity;
}

GameSprite::GameSprite(const sf::Texture &texture, float scale, float xPos, float yPos, float velocity, float maxVelocity, float direction, float rotVelocity, float maxRotVelocity) : GameSprite(texture, scale, xPos, yPos, velocity, maxVelocity, direction) {
    spritePhysics.rotVelocity = rotVelocity;
    spritePhysics.maxRotVelocity = maxRotVelocity;
}

GameSprite::GameSprite(const sf::Texture &texture, float scale, float xPos, float yPos, float velocity, float acceleration, float maxVelocity, float direction, float rotVelocity, float rotAcceleration, float maxRotVelocity) : GameSprite(texture, scale, xPos, yPos, velocity, maxVelocity, direction, rotVelocity, maxRotVelocity) {
    spritePhysics.acceleration = acceleration;
    spritePhysics.rotAcceleration = rotAcceleration;
}

GameSprite::GameSprite(const sf::Texture &texture, const sf::IntRect &rect, int _rows, int _cols, int _xOffset, int _yOffset, int _frameDelay) : GameSprite(texture, rect) {
    rows = _rows;
    cols = _cols;
    xOffset = _xOffset;
    yOffset =  _yOffset;
    frameDelay = _frameDelay;
    initRect = rect;

    isAnimated = true;
}

void GameSprite::update() {
    framesAlive++;

    if (isAnimated) updateAnimation();

    calculateNewPosition();

    calculateNewDirection();

    setPosition(spritePhysics.xPos, spritePhysics.yPos);
    setRotation(-spritePhysics.direction);
}

void GameSprite::nextTexture() {
    currentCol++;
    if (currentCol == cols) {
        currentCol = 0;
        currentRow++;

        if (currentRow == rows) {
            currentRow = 0;
        }
    }

    sf::IntRect newRect(initRect.left + xOffset * currentCol, initRect.top + yOffset * currentRow, initRect.width, initRect.height);
    setTextureRect(newRect);
}

void GameSprite::updateAnimation(bool override) {
    currentFrame++;
    if (override) {
        nextTexture();
        currentFrame = 0;
    } else {
        if (currentFrame == frameDelay) {
            nextTexture();
            currentFrame = 0;
        }
    }
}


void GameSprite::calculateNewDirection() {
    spritePhysics.direction -= spritePhysics.rotVelocity;
    spritePhysics.direction = fmod(spritePhysics.direction, 360);
    if (spritePhysics.direction < 0)
        spritePhysics.direction += 360;
}

void GameSprite::calculateNewPosition() {
    spritePhysics.xPos += cos(spritePhysics.direction * (PI / 180)) * spritePhysics.velocity;
    spritePhysics.yPos += -(sin(spritePhysics.direction * (PI / 180)) * spritePhysics.velocity);
}

void GameSprite::accelerate(float override, bool ignoreMax) {
    if (override != 0) spritePhysics.velocity += override;
    else spritePhysics.velocity += spritePhysics.acceleration;

    if (!ignoreMax && std::abs(spritePhysics.velocity) > spritePhysics.maxVelocity) spritePhysics.velocity = spritePhysics.velocity > 0 ? spritePhysics.maxVelocity : -spritePhysics.maxVelocity;
}

void GameSprite::rotAccel(float override, bool ignoreMax) {
    if (override != 0) spritePhysics.rotVelocity += override;
    else spritePhysics.rotVelocity += spritePhysics.rotAcceleration;

    if (!ignoreMax && spritePhysics.rotVelocity > spritePhysics.maxRotVelocity) spritePhysics.rotVelocity = spritePhysics.maxRotVelocity;
    else if (!ignoreMax && -spritePhysics.rotVelocity > spritePhysics.maxRotVelocity) spritePhysics.rotVelocity = -spritePhysics.maxRotVelocity;
}

void GameSprite::turn(float degrees) {
    spritePhysics.direction -= degrees;

    setRotation(-spritePhysics.direction);
}

float GameSprite::getXPos() const {
    return spritePhysics.xPos;
}

float GameSprite::getYPos() const {
    return spritePhysics.yPos;
}

void GameSprite::setPosition(float xPos, float yPos) {
    spritePhysics.xPos = xPos;
    spritePhysics.yPos = yPos;

    sf::Sprite::setPosition(sf::Vector2f(xPos, yPos));
}

void GameSprite::setPosition(const sf::Vector2f &vec) {
    spritePhysics.xPos = vec.x;
    spritePhysics.yPos = vec.y;

    sf::Sprite::setPosition(vec);
}

float GameSprite::getDirection() const {
    return spritePhysics.direction;
}

void GameSprite::setDirection(float angle) {
    spritePhysics.direction = angle;

    setRotation(-angle);
}

void GameSprite::setVelocity(float velo) {
    spritePhysics.velocity = velo;
}

float GameSprite::getVelocity() const {
    return spritePhysics.velocity;
}

double GameSprite::getAimAngle(const Sprite& b, const Sprite& a) {
    return getAimAngle(b.getPosition(), a.getPosition());
}

double GameSprite::getAimAngle(sf::Vector2f b, sf::Vector2f a) {
    double dx = b.x - a.x;
    double dy = b.y - a.y;

    double targetAngle = -((atan2(dy, dx)) * 180 / GameSprite::PI);
    targetAngle = fmod(targetAngle, 360);

    if (targetAngle < 0)
        targetAngle += 360;
    return -targetAngle;
}

int GameSprite::getFramesAlive() const {
    return framesAlive;
}

bool GameSprite::isPastLifetime() const {
    return lifetime != INFINITE && framesAlive >= lifetime;
}

int GameSprite::getLifetime() const {
    return lifetime;
}
