#include <QCloseEvent>
#include <QToolButton>
#include <QFrame>
#include <QHBoxLayout>
#include <QListWidget>
#include <QSlider>
#include <QDebug>

#include "../PathUtils.h"
#include "OggFile.h"
#include "AudioPlayer.h"

//=================================================================================================
AudioPlayer::AudioPlayer( QWidget *parent ):
  QMdiSubWindow( parent ),
  mdi( (QMdiArea *)parent ),
  currentSound(0)
{
  ALApp::error();

  QFrame *fr = new QFrame( this );
  QVBoxLayout *vl = new QVBoxLayout( fr );

  soundList = new QListWidget( fr );

  QToolButton *tbFirst = new QToolButton( fr );
  tbFirst->setIconSize( QSize( 16, 16 ) );
  tbFirst->setIcon( QIcon(":res/audio_first.png") );
  connect( tbFirst, SIGNAL(clicked()), this, SLOT(playSound()) );

  QToolButton *tbPrev = new QToolButton( fr );
  tbPrev->setIconSize( QSize( 16, 16 ) );
  tbPrev->setIcon( QIcon(":res/audio_prev.png") );
  connect( tbPrev, SIGNAL(clicked()), this, SLOT(playSound()) );

  QToolButton *tbPlay = new QToolButton( fr );
  tbPlay->setIconSize( QSize( 16, 16 ) );
  tbPlay->setIcon( QIcon(":res/audio_play.png") );
  connect( tbPlay, SIGNAL(clicked()), this, SLOT(playSound()) );

  QToolButton *tbStop = new QToolButton( fr );
  tbStop->setIconSize( QSize( 16, 16 ) );
  tbStop->setIcon( QIcon(":res/audio_stop.png") );
  connect( tbStop, SIGNAL(clicked()), this, SLOT(stopSound()) );

  QToolButton *tbNext = new QToolButton( fr );
  tbNext->setIconSize( QSize( 16, 16 ) );
  tbNext->setIcon( QIcon(":res/audio_next.png") );
  connect( tbNext, SIGNAL(clicked()), this, SLOT(playSound()) );

  QToolButton *tbLast = new QToolButton( fr );
  tbLast->setIconSize( QSize( 16, 16 ) );
  tbLast->setIcon( QIcon(":res/audio_last.png") );
  connect( tbLast, SIGNAL(clicked()), this, SLOT(playSound()) );

  QHBoxLayout *hl = new QHBoxLayout();
  hl->addWidget( tbFirst );
  hl->addWidget( tbPrev );
  hl->addSpacing(5);
  hl->addWidget( tbPlay );
  hl->addWidget( tbStop );
  hl->addSpacing(5);
  hl->addWidget( tbNext );
  hl->addWidget( tbLast );
  hl->setMargin(3);
  hl->setSpacing(0);
  hl->setAlignment( Qt::AlignHCenter );

  timeScale = new QSlider( fr );
  timeScale->setOrientation( Qt::Horizontal );

  vl->addWidget( soundList );
  vl->addWidget( timeScale, Qt::AlignHCenter );
  vl->addLayout( hl );

  setWidget( fr );
  setWindowFlags( Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowTitleHint );
  setWindowIcon( QIcon(":res/sound.png") );
  setWindowTitle( tr("Audio Player") );
}
//=================================================================================================
AudioPlayer::~AudioPlayer()
{
  for (QMap<QString, OggFile *>::iterator it = sounds.begin(); it != sounds.end(); ++it)
  {
    it.value()->stop();
    it.value()->free();

    delete *it;
    *it = 0;
  }
}
//=================================================================================================
void AudioPlayer::addSound( const QString &name )
{
  if (!sounds.contains( name ))
  {
    bool finded = false;

    for (int i = 0; i < soundList->count(); ++i)
    {
      QListWidgetItem *item = soundList->item(i);

      if (!item->whatsThis().compare( name ))
      {
        finded = true;
        sounds[ name ]->stop();

        currentSound = sounds[ name ];
        playSound();

        break;
      }
    }

    if (!finded)
    {
      QListWidgetItem *sound = new QListWidgetItem( soundList );
      sound->setText( PathUtils::getName( name ) );
      sound->setWhatsThis( name );
      sound->setData( Qt::DecorationRole, QIcon(":res/sound.png") );

      sounds.insert( name, new OggFile( name, this ) );

      if (currentSound)
        currentSound->stop();

      currentSound = sounds[ name ];
      playSound();
    }
  }
}
//=================================================================================================
void AudioPlayer::closeEvent( QCloseEvent *e )
{
  mdi->removeSubWindow( this );
  e->accept();
}
//=================================================================================================
void AudioPlayer::playSound()
{
  if (currentSound)
  {
    disconnect( currentSound, 0, 0, 0 );
    connect( currentSound, SIGNAL(position(int,int)), this, SLOT(timePosition(int,int)) );
    currentSound->play();
  }
}
//=================================================================================================
void AudioPlayer::stopSound()
{
  if (currentSound)
    currentSound->stop();
}
//=================================================================================================
void AudioPlayer::timePosition( int cur, int all )
{
  timeScale->setRange( 0, all );

  /*float p = (float)cur / (float)all * 100.0f;
  int pos = p * all / 100;
  qDebug() << pos;*/

  timeScale->setValue( cur );
}
//=================================================================================================
