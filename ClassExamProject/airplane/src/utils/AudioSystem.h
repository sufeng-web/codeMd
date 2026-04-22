#ifndef __AUDIOSYSTEM_H_
#define __AUDIOSYSTEM_H_

#include <QString>
#include <QObject>
#include <QtMultimedia/QMediaPlayer>
#include <QUrl>

class AudioSystem : public QObject 
{
    Q_OBJECT
public:
    enum SoundType 
    {
        APPLE_BG,
        APPLE_IN
    };

    static AudioSystem& instance();

    void setVolume(int volume);
    void playSound(SoundType type);
    void stopMusic();

private:
    AudioSystem();
    ~AudioSystem() override;

    QMediaPlayer* bgPlayer;
    QMediaPlayer* fxPlayer;
};

#endif // __AUDIOSYSTEM_H_
