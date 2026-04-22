#ifndef __FRUIT_H_
#define __FRUIT_H_

#include <QPoint>
#include <QRect>
#include <QString>

enum class FruitType 
{ 
    Apple,
    GoldenApple,
    PoisonApple 
};
enum class FruitState 
{ 
    Falling,
    Broken
};

class Fruit {
public:
    Fruit(FruitType type, QPoint position, QString letter, int speed);
    
    void update(float deltaTime);  // 更新位置
    QRect getBounds() const;       // 碰撞检测用
    int getScoreValue() const;
    QString getLetter() const;
    QPoint getPosition() const;
    
    FruitState getState() const;
    void setBroken();
    bool isExpired() const;
    
private:
    FruitType m_type;
    FruitState m_state;
    QPoint m_position;
    float m_exactY; // 用浮点数保存精确的Y坐标防止低速掉落精度丢失
    int m_speed;
    int m_scoreValue;
    QString m_letter;
    float m_brokenTimer;
};
#endif