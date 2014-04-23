#ifndef ALAPP_H
#define ALAPP_H

//=================================================================================================
#include <QObject>
#include <QVector>

#include <AL/al.h>
#include <AL/alc.h>
//=================================================================================================
class SoundFile
{
  public:
    SoundFile():
      channels(0),
      freq(0)
    {}

    virtual ~SoundFile() {}

    static SoundFile *load( const QString &name );

    virtual int size() = 0;
    virtual int read( char *buffer, int size = -1 ) = 0;
    virtual void seek( double time ) = 0;

    int channels;
    int freq;
};
//=================================================================================================
class Sounds
{
  public:
    Sounds( const QString &name, int flag = 0 );
    ~Sounds();

    enum
    {
      LOOP = 1 << 0,
      STREAM = 1 << 1,
      BUFFER_SIZE = 65536
    };

    void play();
    void pause();
    void stop();
    void update();

    int flag;
    SoundFile *file;
    ALuint format;
    char *buffer;
    int current_buffer;
    ALuint buffers[2];
    ALuint source;
};
//=================================================================================================
class ALApp
{
  public:
    explicit ALApp();
    ~ALApp();

    void error();
    void update();

  protected:
    ALCdevice *device;
    ALCcontext *context;

    friend class Sounds;
    static void addStream( Sounds *s );
    static void removeStream( Sounds *s );
    static QVector<Sounds *> streams;
};
//=================================================================================================

#endif // ALAPP_H
