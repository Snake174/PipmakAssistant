#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

//=================================================================================================
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QMap>

#include "ALApp.h"
//=================================================================================================
QT_BEGIN_NAMESPACE
  class QTreeWidget;
QT_END_NAMESPACE
//=================================================================================================
class AudioPlayer : public QMdiSubWindow, ALApp
{
  Q_OBJECT

  QMdiArea *mdi;
  QMap<QString, Sounds *> sounds;
  QTreeWidget *soundTree;

  public:
    explicit AudioPlayer( QWidget *parent = 0 );
    ~AudioPlayer();

    void addSound( const QString &name );

  protected:
    void closeEvent( QCloseEvent *e );

  private slots:
    void playSound();
    void stopSound();
};
//=================================================================================================

#endif // AUDIOPLAYER_H
