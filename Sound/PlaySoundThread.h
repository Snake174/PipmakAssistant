#ifndef PLAYSOUNDTHREAD_H
#define PLAYSOUNDTHREAD_H

#include <QThread>

QT_BEGIN_NAMESPACE
  class OggFile;
QT_END_NAMESPACE

class PlaySoundThread : public QThread
{
  Q_OBJECT

  OggFile *oggFile;

  public:
    explicit PlaySoundThread( OggFile *ogg, QObject *parent = 0 );

    void stopPlay();

  protected:
    void run();
};

#endif // PLAYSOUNDTHREAD_H
