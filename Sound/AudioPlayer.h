#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

//=================================================================================================
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QMap>

#include "ALApp.h"
//=================================================================================================
QT_BEGIN_NAMESPACE
  class QListWidget;
  class QSlider;
  class OggFile;
QT_END_NAMESPACE
//=================================================================================================
class AudioPlayer : public QMdiSubWindow, ALApp
{
  Q_OBJECT

  QMdiArea *mdi;
  QMap<QString, OggFile *> sounds;
  QListWidget *soundList;
  QSlider *timeScale;
  OggFile *currentSound;

  public:
    explicit AudioPlayer( QWidget *parent = 0 );
    ~AudioPlayer();

    void addSound( const QString &name );

  protected:
    void closeEvent( QCloseEvent *e );

  private slots:
    void playSound();
    void stopSound();
    void timePosition( int cur, int all );
};
//=================================================================================================

#endif // AUDIOPLAYER_H
