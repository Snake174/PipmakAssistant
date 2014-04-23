#include <QCloseEvent>
#include <QToolButton>
#include <QFrame>
#include <QHBoxLayout>
#include <QTreeWidget>

#include "../PathUtils.h"
#include "AudioPlayer.h"

//=================================================================================================
AudioPlayer::AudioPlayer( QWidget *parent ):
  QMdiSubWindow( parent ),
  mdi( (QMdiArea *)parent )
{
  ALApp::error();

  soundTree = new QTreeWidget( this );
  soundTree->setColumnCount(2);
  soundTree->setHeaderLabels( QStringList() << tr("Name") << tr("Operations") );
  soundTree->setHeaderHidden( false );

  setWidget( soundTree );
  setWindowFlags( Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowTitleHint );
  setWindowIcon( QIcon(":res/sound.png") );
  setWindowTitle( tr("Audio Player") );
}
//=================================================================================================
AudioPlayer::~AudioPlayer()
{
  for (QMap<QString, Sounds *>::iterator it = sounds.begin(); it != sounds.end(); ++it)
  {
    it.value()->stop();

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

    for (int i = 0; i < soundTree->topLevelItemCount(); ++i)
    {
      if (!soundTree->topLevelItem(i)->whatsThis(0).compare( name ))
      {
        finded = true;
        sounds[ name ]->stop();
        sounds[ name ]->play();
        break;
      }
    }

    if (!finded)
    {
      QTreeWidgetItem *sound = new QTreeWidgetItem( soundTree );
      sound->setText( 0, PathUtils::getName( name ) );
      sound->setWhatsThis( 0, name );

      QFrame *fr = new QFrame( soundTree );
      QHBoxLayout *hl = new QHBoxLayout( fr );

      QToolButton *tbPlay = new QToolButton( fr );
      tbPlay->setIconSize( QSize( 16, 16 ) );
      tbPlay->setIcon( QIcon(":res/play.png") );
      tbPlay->setWhatsThis( name );
      connect( tbPlay, SIGNAL(clicked()), this, SLOT(playSound()) );

      QToolButton *tbStop = new QToolButton( fr );
      tbStop->setIconSize( QSize( 16, 16 ) );
      tbStop->setIcon( QIcon(":res/stop.png") );
      tbStop->setWhatsThis( name );
      connect( tbPlay, SIGNAL(clicked()), this, SLOT(stopSound()) );

      hl->addWidget( tbPlay );
      hl->addWidget( tbStop );
      hl->setMargin(0);
      hl->setSpacing(0);
      hl->setAlignment( Qt::AlignLeft );

      soundTree->setItemWidget( sound, 1, fr );

      sounds.insert( name, new Sounds( name ) );
      sounds[ name ]->play();
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
  QToolButton *tb = dynamic_cast<QToolButton *>( sender() );

  if (tb)
  {
    QString name = tb->whatsThis();
    sounds[ name ]->stop();
    sounds[ name ]->play();
  }
}
//=================================================================================================
void AudioPlayer::stopSound()
{
  QToolButton *tb = dynamic_cast<QToolButton *>( sender() );

  if (tb)
  {
    QString name = tb->whatsThis();
    sounds[ name ]->stop();
  }
}
//=================================================================================================
