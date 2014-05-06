#ifdef Qt5
  #include <QtConcurrent/QtConcurrentRun>
#else
  #include <QtConcurrentRun>
#endif

#include <QDebug>

#include "OggFile.h"

//=================================================================================================
#define BUFFER_SIZE 32768
//=================================================================================================
OggFile::OggFile( const QString &fileName, QObject *parent ):
  QObject( parent ),
  fileName( fileName )
{
  qDebug() << "Load file " << fileName << "...\n";
  LoadOGG( fileName, bufferData, format, freq );
}
//=================================================================================================
OggFile::~OggFile()
{
}
//=================================================================================================
void OggFile::LoadOGG( const QString &fileName, std::vector<char> &buffer, ALenum &format, ALsizei &freq )
{
  // Create sound buffer and source
  alGenBuffers( 1, &bufferID );
  alGenSources( 1, &sourceID );

  // Set the source and listener to the same location
  alListener3f( AL_POSITION, 0.0f, 0.0f, 0.0f );
  alSource3f( sourceID, AL_POSITION, 0.0f, 0.0f, 0.0f );

  int endian = 0;                         // 0 for Little-Endian, 1 for Big-Endian
  int bitStream;
  long bytes;
  char array[ BUFFER_SIZE ];              // Local fixed size array
  FILE *f;

  // Open for binary reading
  f = fopen( fileName.toLatin1().data(), "rb" );

  if (f == NULL)
  {
    qDebug() << "Cannot open " << fileName << " for reading...\n";
    return;
  }

  vorbis_info *pInfo;
  oggFile = new OggVorbis_File();

  // Try opening the given file
  if (ov_open( f, oggFile, NULL, 0 ) != 0)
  {
    qDebug() << "Error opening " << fileName << " for decoding...\n";
    return;
  }

  // Get some information about the OGG file
  pInfo = ov_info( oggFile, -1 );

  // Check the number of channels... always use 16-bit samples
  if (pInfo->channels == 1)
    format = AL_FORMAT_MONO16;
  else
    format = AL_FORMAT_STEREO16;

  // The frequency of the sampling rate
  freq = pInfo->rate;

  bytespersec = pInfo->channels * pInfo->rate * 16 / 8;

  // Keep reading until all is read
  do
  {
    // Read up to a buffer's worth of decoded sound data
    bytes = ov_read( oggFile, array, BUFFER_SIZE, endian, 2, 1, &bitStream );

    if (bytes < 0)
    {
      ov_clear( oggFile );
      qDebug() << "Error decoding " << fileName << "...\n";
      return;
    }

    // Append to end of buffer
    buffer.insert( buffer.end(), array, array + bytes );
  }
  while (bytes > 0);
}
//=================================================================================================
void OggFile::playSound()
{
  // Upload sound data to buffer
  alBufferData( bufferID, format, &bufferData[0], static_cast<ALsizei>( bufferData.size() ), freq );

  // Attach sound buffer to source
  alSourcei( sourceID, AL_BUFFER, bufferID );

  qDebug() << "Play file " << fileName;

  // Finally, play the sound!!!
  alSourcePlay( sourceID );

  // This is a busy wait loop but should be good enough for example purpose
  do
  {
    ALfloat butes;

    // Query the state of the souce
    alGetSourcei( sourceID, AL_SOURCE_STATE, &state );
    alGetSourcef( sourceID, AL_BYTE_OFFSET, &butes );

    emit position( butes, bytespersec );
  }
  while (state != AL_STOPPED);
}
//=================================================================================================
void OggFile::play()
{
  QtConcurrent::run( this, &OggFile::playSound );
}
//=================================================================================================
void OggFile::stop()
{
  qDebug() << "Stop file " << fileName;
  alSourceStop( sourceID );
}
//=================================================================================================
void OggFile::free()
{
  qDebug() << "Clear file " << fileName;

  // Clean up!
  ov_clear( oggFile );

  // Clean up sound buffer and source
  alDeleteBuffers( 1, &bufferID );
  alDeleteSources( 1, &sourceID );
}
//=================================================================================================
