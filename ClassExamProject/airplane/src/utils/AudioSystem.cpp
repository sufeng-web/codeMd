#include "AudioSystem.h"

AudioSystem::AudioSystem() 
{
    bgPlayer = new QMediaPlayer(this);
    bgPlayer->setMedia(QUrl("qrc:/res/Apple/Sounds/APPLE_BG.wav")); 

    fxPlayer = new QMediaPlayer(this);
    fxPlayer->setMedia(QUrl("qrc:/res/Apple/Sounds/APPLE_IN.wav"));
}

AudioSystem::~AudioSystem() 
{
    if (bgPlayer) 
    {
        bgPlayer->stop();
    }
    if (fxPlayer) 
    {
        fxPlayer->stop();
    }
}

AudioSystem& AudioSystem::instance() 
{
    static AudioSystem instance;
    return instance;
}

void AudioSystem::setVolume(int volume) 
{
    if (bgPlayer) bgPlayer->setVolume(volume);
    if (fxPlayer) fxPlayer->setVolume(volume);
}

void AudioSystem::playSound(SoundType type) 
{
    if (type == APPLE_BG) 
    {
        bgPlayer->setPosition(0);
        bgPlayer->play();
    } else if (type == APPLE_IN) 
    {
        fxPlayer->setPosition(0);
        fxPlayer->play();
    }
}

void AudioSystem::stopMusic() 
{
    if (bgPlayer) {
        bgPlayer->stop();
    }
}
