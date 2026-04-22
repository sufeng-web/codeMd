#include "GameData.h"

GameData::GameData() : m_score(0), m_lives(3), m_level(1), m_missed(0), m_maxMisses(3), m_basketApples(0), m_isPaused(false), m_isGameOver(false) {}

int GameData::getScore() const 
{ 
    return m_score;
}
void GameData::addScore(int value)
{ 
    m_score += value;
    notifyObservers();
}
void GameData::resetScore() 
{ 
    m_score = 0; 
    m_level = 1; 
    m_missed = 0;
    m_basketApples = 0;
    notifyObservers();
}

int GameData::getLevel() const 
{ 
    return m_level;
}
void GameData::setLevel(int level) { 
    if(m_level != level)
    { 
        m_level = level; 
        notifyObservers();
    }
}

int GameData::getBasketApples() const 
{ 
    return m_basketApples;
}
void GameData::addBasketApple() 
{ 
    m_basketApples++;
    notifyObservers();
}
void GameData::clearBasketApples() 
{ 
    m_basketApples = 0;
    notifyObservers(); 
}

int GameData::getMissed() const 
{ 
    return m_missed; 
}
void GameData::addMissed()
{ 
    m_missed++;
    notifyObservers();
}
void GameData::resetMissed() 
{ 
    m_missed = 0;
    notifyObservers();
}

int GameData::getMaxMisses() const 
{ 
    return m_maxMisses;
}
void GameData::setMaxMisses(int maxMisses) 
{ 
    m_maxMisses = maxMisses; 
    notifyObservers(); 
}

int GameData::getLives() const
{ 
    return m_lives;
}
void GameData::setLives(int lives)
{ 
    m_lives = lives;
    notifyObservers();
}
void GameData::loseLife() 
{
    m_lives--;
    notifyObservers();
}
void GameData::addLife() 
{ 
    m_lives++; 
    notifyObservers();
}

bool GameData::isGameOver() const
{
    return m_isGameOver; 
}
void GameData::setGameOver(bool gameOver)
{ 
    m_isGameOver = gameOver;
    notifyObservers(); 
}

bool GameData::isPaused() const 
{ 
    return m_isPaused;
}
void GameData::setPaused(bool paused) 
{ 
    m_isPaused = paused; 
    notifyObservers(); 
}

void GameData::attach(IObserver* observer) 
{
    m_observers.push_back(observer);
}
void GameData::detach(IObserver* observer) 
{
    auto it = std::find(m_observers.begin(), m_observers.end(), observer);
    if (it != m_observers.end()) m_observers.erase(it);
}

void GameData::notifyObservers()
{
    // 遍历存储在 m_observers 列表中的每一个观察者// 告诉观察者：数据更新了，所有的观察这都要更新
    for (auto observer : m_observers) observer->onUpdate();
}