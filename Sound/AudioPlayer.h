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
  class QListWidgetItem;
  class QSlider;
  class OggFile;
  class QToolButton;
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
  QToolButton *tbLoop;

  public:
    explicit AudioPlayer( QWidget *parent = 0 );
    ~AudioPlayer();

    void addSound( const QString &name );

  protected:
    void closeEvent( QCloseEvent *e );

  private slots:
    void playSound();
    void stopSound();
    void nextSound();
    void prevSound();
    void lastSound();
    void firstSound();
    void timePosition( int cur, int all );
    void soundsListDoubleClicked( QListWidgetItem *item );
    void setLooping( bool state );
};
//=================================================================================================

#endif // AUDIOPLAYER_H
