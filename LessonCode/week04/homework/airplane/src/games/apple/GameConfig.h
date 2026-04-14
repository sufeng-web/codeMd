#pragma once

#include <QString>
#include <map>

// GameConfig.h - 游戏配置（单例模式）
class GameConfig {
public:
    static GameConfig& getInstance();
    
    // 配置项
    int getInitialLives() const { return m_lives; }
    void setInitialLives(int v) { m_lives = v; }
    
    int getMaxApples() const { return m_maxApples; }
    void setMaxApples(int v) { m_maxApples = v; }
    
    int getBaseSpeed() const { return m_baseSpeed; }
    void setBaseSpeed(int v) { m_baseSpeed = v; }
    
    int getFruitSpawnInterval() const { return 1000; }  // ms
    int getGameSpeed() const { return 60; }  // FPS
    
    // 国际化
    QString getText(const QString& key);
    void setLanguage(const QString& lang);
    
private:
    GameConfig();
    std::map<QString, QString> configs;
    
    int m_lives = 3;
    int m_maxApples = 5;
    int m_baseSpeed = 40; // 影响下坠难度基数
};