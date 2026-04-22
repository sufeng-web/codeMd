#include "Fruit.h"

Fruit::Fruit(FruitType type, QPoint position, QString letter, int speed) 
    : m_type(type), m_state(FruitState::Falling), m_position(position), m_exactY(position.y()), m_speed(speed), m_scoreValue(10), m_letter(letter), m_brokenTimer(0) {
}

void Fruit::update(float deltaTime) 
{
    if (m_state == FruitState::Falling) 
    {
        this->m_exactY += (this->m_speed * deltaTime);
        this->m_position.setY(static_cast<int>(m_exactY));
    } else if (m_state == FruitState::Broken) 
    {
        m_brokenTimer -= deltaTime;
    }
}

QRect Fruit::getBounds() const 
{
    return QRect(m_position.x(), m_position.y(), 50, 50);
}

int Fruit::getScoreValue() const 
{
    return m_scoreValue;
}

QString Fruit::getLetter() const 
{ 
    return m_letter;
}
QPoint Fruit::getPosition() const 
{ 
    return m_position; 
}
FruitState Fruit::getState() const 
{ 
    return m_state;
}

void Fruit::setBroken() 
{
    m_state = FruitState::Broken;
    m_brokenTimer = 0.5f; // ÀÈµÙ∫Û≤•∑≈∂Øª≠ªÚÕ£¡Ù0.5√Î
}

bool Fruit::isExpired() const 
{
    return m_state == FruitState::Broken && m_brokenTimer <= 0;
}
