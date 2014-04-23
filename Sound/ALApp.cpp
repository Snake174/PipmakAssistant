#include <stdio.h>
#include <vorbis/vorbisfile.h>

#include "ALApp.h"

//=================================================================================================
class SoundFileOgg : public SoundFile
{
  public:
    SoundFileOgg( const QString &name );
    virtual ~SoundFileOgg();

    virtual int size();
    virtual int read( char *buf, int size = -1 );
    virtual void seek( double time );

  protected:
    FILE *file;
    OggVorbis_File vf;
    vorbis_info *vi;
};
//=================================================================================================
SoundFileOgg::SoundFileOgg( const QString &name )
{
  file = fopen( name.toLatin1().constData(), "rb" );

  if (!file)
  {
    fprintf( stderr, "SoundFileOgg::SoundFileOgg(): error open \"%s\" file\n", name.toLatin1().constData() );
    return;
  }

  if (ov_open( file, &vf, NULL, 0 ) < 0)
  {
    fprintf( stderr, "SoundFileOgg::SoundFileOgg(): \"%s\" is not ogg bitstream\n", name.toLatin1().constData() );
    fclose( file );
    file = NULL;
    return;
  }

  vi = ov_info( &vf, -1 );
  channels = vi->channels;
  freq = vi->rate;
}
//=================================================================================================
SoundFileOgg::~SoundFileOgg()
{
  if (file)
  {
    ov_clear( &vf );
    fclose( file );
  }
}
//=================================================================================================
int SoundFileOgg::size()
{
  if (!file)
    return 0;

  return (int)(ov_time_total( &vf, -1 ) + 0.5) * channels * freq * 2;
}
//=================================================================================================
int SoundFileOgg::read( char *buffer, int size )
{
  if (!file)
    return 0;

  int current_section;

  if (size < 0)
    size = this->size();

  int read = 0;

  while (read < size)
  {
    int ret = ov_read( &vf, buffer + read, size - read, 0, 2, 1, &current_section );

    if (ret <= 0)
      break;

    read += ret;
  }

  return read;
}
//=================================================================================================
void SoundFileOgg::seek( double time )
{
  if (!file)
    return;

  ov_time_seek( &vf, time );
}
//=================================================================================================




//=================================================================================================
SoundFile *SoundFile::load( const QString &name )
{
  if (name.endsWith( ".ogg", Qt::CaseInsensitive ))
    return new SoundFileOgg( name );

  fprintf( stderr, "\"%s\" is not supported\n", name.toLatin1().constData() );

  return NULL;
}
//=================================================================================================



//=================================================================================================
Sounds::Sounds( const QString &name, int flag ):
  flag( flag )
{
  file = SoundFile::load( name );

  if (!file)
    return;

  format = file->channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;

  if (file->size() < BUFFER_SIZE * 2)
    this->flag &= ~STREAM;

  if (this->flag & STREAM)
  {
    buffer = new char[ BUFFER_SIZE * 2 ];
    alGenBuffers( 2, buffers );
    alGenSources( 1, &source );
    current_buffer = 0;
  }
  else
  {
    buffer = new char[ file->size() ];
    int size = file->read( buffer );
    alGenBuffers( 1, buffers );
    alBufferData( buffers[0], format, buffer, size, file->freq );
    alGenSources( 1, &source );
    alSourcei( source, AL_BUFFER, buffers[0] );
    alSourcei( source, AL_LOOPING, flag & LOOP ? AL_TRUE : AL_FALSE );

    delete[] buffer;
    buffer = 0;
  }
}
//=================================================================================================
Sounds::~Sounds()
{
  if (!file)
    return;

  delete file;
  file = 0;

  if (flag & STREAM)
  {
    delete[] buffer;
    buffer = 0;
  }

  alDeleteSources( 1, &source );

  if (flag & STREAM)
    alDeleteBuffers( 2, buffers );
  else
    alDeleteBuffers( 1, buffers );
}
//=================================================================================================
void Sounds::play()
{
  if (!file)
    return;

  ALint state;
  alGetSourcei( source, AL_SOURCE_STATE, &state );

  if (state == AL_PLAYING)
    return;

  if (state != AL_PAUSED && flag & STREAM)
  {
    file->read( buffer, BUFFER_SIZE );
    alBufferData( buffers[0], format, buffer, BUFFER_SIZE, file->freq );
    file->read( buffer, BUFFER_SIZE );
    alBufferData( buffers[1], format, buffer, BUFFER_SIZE, file->freq );
    alSourceQueueBuffers( source, 2, buffers );
    ALApp::addStream( this );
  }

  alSourcePlay( source );
}
//=================================================================================================
void Sounds::pause()
{
  if (!file)
    return;

  alSourcePause( source );
}
//=================================================================================================
void Sounds::stop()
{
  if (!file)
    return;

  alSourceStop( source );
  file->seek( 0.0 );

  if (flag & STREAM)
  {
    ALint queued;
    alGetSourcei( source, AL_BUFFERS_QUEUED, &queued );

    if (queued > 0)
      alSourceUnqueueBuffers( source, 2, buffers );

    ALApp::removeStream( this );
    current_buffer = 0;
  }
}
//=================================================================================================
void Sounds::update()
{
  if (!file)
    return;

  if (flag & STREAM)
  {
    ALint processed;
    alGetSourcei( source, AL_BUFFERS_PROCESSED, &processed );

    if (processed == 1)
    {
      alSourceUnqueueBuffers( source, 1, &buffers[ current_buffer ] );
      int size = file->read( buffer, BUFFER_SIZE );

      if (size > 0 || (size == 0 && flag & LOOP))
      {
        alBufferData( buffers[ current_buffer ], format, buffer, size, file->freq );
        alSourceQueueBuffers( source, 1, &buffers[ current_buffer ] );

        if (size != BUFFER_SIZE && flag & LOOP)
          file->seek( 0.0 );
      }
      else
      {
        int queued;
        alGetSourcei( source, AL_BUFFERS_QUEUED, &queued );

        if (queued == 0)
          file->seek( 0.0 );
      }

      current_buffer = 1 - current_buffer;
    }
    else if (processed == 2)
    {
      alSourceUnqueueBuffers( source, 2, buffers );
      current_buffer = 0;
      play();
    }
  }
}
//=================================================================================================
QVector<Sounds *> ALApp::streams;
//=================================================================================================
ALApp::ALApp()
{
  device = alcOpenDevice( NULL );

  if (!device)
  {
    fprintf( stderr, "ALApp::ALApp(): invalid device\n" );
    return;
  }

  context = alcCreateContext( device, NULL );

  if (!context)
  {
    fprintf( stderr, "ALApp::ALApp(): invalid context\n" );
    return;
  }

  alcMakeContextCurrent( context );
}
//=================================================================================================
ALApp::~ALApp()
{
  alcDestroyContext( context );
  alcCloseDevice( device );
}
//=================================================================================================
void ALApp::error()
{
  ALenum error;

  while ((error = alGetError()) != AL_NO_ERROR)
    fprintf( stderr, "ALApp::error(): 0x%04X\n", error );
}
//=================================================================================================
void ALApp::update()
{
  for (int i = 0; i < streams.count(); i++)
    streams.at(i)->update();
}
//=================================================================================================
void ALApp::addStream( Sounds *s )
{
  int i = 0;

  for (i = 0; i < streams.count(); i++)
  {
    if (streams.at(i) == s)
      break;
  }

  if (i == streams.count())
    streams.push_back(s);
}
//=================================================================================================
void ALApp::removeStream( Sounds *s )
{
  if (streams.count() == 0)
    return;

  int i = 0;

  for (i = 0; i < streams.count(); i++)
  {
    if (streams.at(i) == s)
      break;
  }

  if (i == streams.count())
    streams.remove(i);
}
//=================================================================================================
