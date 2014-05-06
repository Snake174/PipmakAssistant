#ifndef OGGFILE_H
#define OGGFILE_H

//=================================================================================================
#include <QObject>

#include <AL/al.h>
#include <AL/alc.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisenc.h>
#include <vorbis/vorbisfile.h>
#include <iostream>
#include <vector>
//=================================================================================================
class OggFile : public QObject
{
  Q_OBJECT

  QString fileName;
  ALint state;                   // The state of the sound source
  ALuint bufferID;               // The OpenAL sound buffer ID
  ALuint sourceID;               // The OpenAL sound source
  ALenum format;                 // The sound data format
  ALsizei freq;                  // The frequency of the sound data
  std::vector<char> bufferData;  // The sound buffer data from file
  int bytespersec;
  OggVorbis_File *oggFile;

  public:
    explicit OggFile( const QString &fileName, QObject *parent = 0 );
    ~OggFile();

    void play();
    void stop();
    void free();

  private:
    void LoadOGG( const QString &fileName, std::vector<char> &buffer, ALenum &format, ALsizei &freq );
    void playSound();

  signals:
    void position( int, int );
};
//=================================================================================================

#endif // OGGFILE_H
