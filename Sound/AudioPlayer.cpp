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
  connect( soundList, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(soundsListDoubleClicked(QListWidgetItem*)) );

  QToolButton *tbFirst = new QToolButton( fr );
  tbFirst->setIconSize( QSize( 16, 16 ) );
  tbFirst->setIcon( QIcon(":res/audio_first.png") );
  tbFirst->setToolTip( tr("First Track") );
  connect( tbFirst, SIGNAL(clicked()), this, SLOT(firstSound()) );

  QToolButton *tbPrev = new QToolButton( fr );
  tbPrev->setIconSize( QSize( 16, 16 ) );
  tbPrev->setIcon( QIcon(":res/audio_prev.png") );
  tbPrev->setToolTip( tr("Previous Track") );
  connect( tbPrev, SIGNAL(clicked()), this, SLOT(prevSound()) );

  QToolButton *tbPlay = new QToolButton( fr );
  tbPlay->setIconSize( QSize( 16, 16 ) );
  tbPlay->setIcon( QIcon(":res/audio_play.png") );
  tbPlay->setToolTip( tr("Play") );
  connect( tbPlay, SIGNAL(clicked()), this, SLOT(playSound()) );

  QToolButton *tbStop = new QToolButton( fr );
  tbStop->setIconSize( QSize( 16, 16 ) );
  tbStop->setIcon( QIcon(":res/audio_stop.png") );
  tbStop->setToolTip( tr("Stop") );
  connect( tbStop, SIGNAL(clicked()), this, SLOT(stopSound()) );

  QToolButton *tbNext = new QToolButton( fr );
  tbNext->setIconSize( QSize( 16, 16 ) );
  tbNext->setIcon( QIcon(":res/audio_next.png") );
  tbNext->setToolTip( tr("Next Track") );
  connect( tbNext, SIGNAL(clicked()), this, SLOT(nextSound()) );

  QToolButton *tbLast = new QToolButton( fr );
  tbLast->setIconSize( QSize( 16, 16 ) );
  tbLast->setIcon( QIcon(":res/audio_last.png") );
  tbLast->setToolTip( tr("Last Track") );
  connect( tbLast, SIGNAL(clicked()), this, SLOT(lastSound()) );

  tbLoop = new QToolButton( fr );
  tbLoop->setIconSize( QSize( 16, 16 ) );
  tbLoop->setIcon( QIcon(":res/audio_loop.png") );
  tbLoop->setToolTip( tr("Loop") );
  tbLoop->setCheckable( true );
  tbLoop->setChecked( false );
  connect( tbLoop, SIGNAL(toggled(bool)), this, SLOT(setLooping(bool)) );

  QHBoxLayout *hl = new QHBoxLayout();
  hl->addWidget( tbFirst );
  hl->addWidget( tbPrev );
  hl->addSpacing(5);
  hl->addWidget( tbPlay );
  hl->addWidget( tbStop );
  hl->addSpacing(5);
  hl->addWidget( tbNext );
  hl->addWidget( tbLast );
  hl->addSpacing(5);
  hl->addWidget( tbLoop );
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
        tbLoop->setChecked( currentSound->isLooped() );

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

    setWindowTitle( tr("Audio Player") + " : " + PathUtils::getName( name ) );
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
void AudioPlayer::nextSound()
{
  if (soundList->currentRow() < soundList->count() - 1)
    soundList->setCurrentRow( soundList->currentRow() + 1, QItemSelectionModel::ClearAndSelect );
  else
    soundList->setCurrentRow( 0, QItemSelectionModel::ClearAndSelect );

  if (currentSound)
    currentSound->stop();

  currentSound = sounds[ soundList->item( soundList->currentRow() )->whatsThis() ];
  playSound();
  setWindowTitle( tr("Audio Player") + " : " + PathUtils::getName( soundList->item( soundList->currentRow() )->whatsThis() ) );
}
//=================================================================================================
void AudioPlayer::prevSound()
{
  if (soundList->currentRow() > 0)
    soundList->setCurrentRow( soundList->currentRow() - 1, QItemSelectionModel::ClearAndSelect );
  else
    soundList->setCurrentRow( soundList->count() - 1, QItemSelectionModel::ClearAndSelect );

  if (currentSound)
    currentSound->stop();

  currentSound = sounds[ soundList->item( soundList->currentRow() )->whatsThis() ];
  playSound();
  setWindowTitle( tr("Audio Player") + " : " + PathUtils::getName( soundList->item( soundList->currentRow() )->whatsThis() ) );
}
//=================================================================================================
void AudioPlayer::lastSound()
{
  if (soundList->currentRow() < soundList->count() - 1)
    soundList->setCurrentRow( soundList->count() - 1, QItemSelectionModel::ClearAndSelect );

  if (currentSound)
    currentSound->stop();

  currentSound = sounds[ soundList->item( soundList->currentRow() )->whatsThis() ];
  playSound();
  setWindowTitle( tr("Audio Player") + " : " + PathUtils::getName( soundList->item( soundList->currentRow() )->whatsThis() ) );
}
//=================================================================================================
void AudioPlayer::firstSound()
{
  if (soundList->currentRow() > 0)
    soundList->setCurrentRow( 0, QItemSelectionModel::ClearAndSelect );

  if (currentSound)
    currentSound->stop();

  currentSound = sounds[ soundList->item( soundList->currentRow() )->whatsThis() ];
  playSound();
  setWindowTitle( tr("Audio Player") + " : " + PathUtils::getName( soundList->item( soundList->currentRow() )->whatsThis() ) );
}
//=================================================================================================
void AudioPlayer::timePosition( int cur, int all )
{
  timeScale->setRange( 0, all );
  timeScale->setValue( cur );
}
//=================================================================================================
void AudioPlayer::soundsListDoubleClicked( QListWidgetItem *item )
{
  if (currentSound)
    currentSound->stop();

  currentSound = sounds[ item->whatsThis() ];
  currentSound->play();
  tbLoop->setChecked( currentSound->isLooped() );

  setWindowTitle( tr("Audio Player") + " : " + PathUtils::getName( item->whatsThis() ) );
}
//=================================================================================================
void AudioPlayer::setLooping( bool state )
{
  if (currentSound)
    currentSound->setLoop( state );
}
//=================================================================================================
