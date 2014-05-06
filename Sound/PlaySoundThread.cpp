#include "OggFile.h"
#include "PlaySoundThread.h"

PlaySoundThread::PlaySoundThread( OggFile *ogg, QObject *parent ):
  QThread( parent ),
  oggFile( ogg )
{
}

void PlaySoundThread::stopPlay()
{
  oggFile->stop();
}

void PlaySoundThread::run()
{
  oggFile->play();
}
