#ifndef LOADINGEFFECT_H
#define LOADINGEFFECT_H

//=================================================================================================
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QToolButton>
#include <QStackedWidget>
//=================================================================================================
class ProgressIndicator : public QWidget
{
  Q_OBJECT

  Q_PROPERTY( int delay READ animationDelay WRITE setAnimationDelay )
  Q_PROPERTY( bool displayedWhenStopped READ isDisplayedWhenStopped WRITE setDisplayedWhenStopped )
  Q_PROPERTY( QColor color READ color WRITE setColor )

  public:
    ProgressIndicator( QWidget *parent = 0 );

    int animationDelay() const { return m_delay; }
    bool isAnimated () const;
    bool isDisplayedWhenStopped() const;
    const QColor &color() const { return m_color; }

    virtual QSize sizeHint() const;
    int heightForWidth( int w ) const;

    int angle;
    int procent;

  public slots:
    void startAnimation();
    void stopAnimation();
    void setAnimationDelay( int delay );
    void setDisplayedWhenStopped( bool state );
    void setColor( const QColor &color );

  protected:
    virtual void timerEvent( QTimerEvent *event );
    virtual void paintEvent( QPaintEvent *event );

  private:
    int m_angle;
    int m_timerId;
    int m_delay;
    bool m_displayedWhenStopped;
    QColor m_color;
};
//=================================================================================================



//=================================================================================================
class LoadingEffect : public QStackedWidget
{
  Q_OBJECT

  ProgressIndicator *pProgress;
  QLabel *info;

  public:
    LoadingEffect( QWidget *parent = 0 );
    virtual ~LoadingEffect();

    void setWidget( QWidget *wgt ) { insertWidget( 1, wgt ); setCurrentIndex(1); }

  public slots:
    void install();
    void remove();
    void setProgress( int cur, int cnt );
    void setInfo( const QString &txt ) { info->setText( txt ); }
};
//=================================================================================================

#endif // LOADINGEFFECT_H
