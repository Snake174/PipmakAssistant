#include <QApplication>
#include <QPainter>
#include <QEvent>
#include <QFrame>
#include <QVBoxLayout>
#include <QStyle>

#include "LoadingEffect.h"

//=================================================================================================
ProgressIndicator::ProgressIndicator( QWidget *parent ):
  QWidget( parent ),
  procent(0),
  m_angle(0),
  m_timerId( -1 ),
  m_delay( 40 ),
  m_displayedWhenStopped( false ),
  m_color( Qt::black )
{
  setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed );
  setFocusPolicy( Qt::NoFocus );
}
//=================================================================================================
bool ProgressIndicator::isAnimated() const
{
  return (m_timerId != -1);
}
//=================================================================================================
void ProgressIndicator::setDisplayedWhenStopped( bool state )
{
  m_displayedWhenStopped = state;
  update();
}
//=================================================================================================
bool ProgressIndicator::isDisplayedWhenStopped() const
{
  return m_displayedWhenStopped;
}
//=================================================================================================
void ProgressIndicator::startAnimation()
{
  m_angle = 0;

  if (m_timerId == -1)
    m_timerId = startTimer( m_delay );
}
//=================================================================================================
void ProgressIndicator::stopAnimation()
{
  if (m_timerId != -1)
    killTimer( m_timerId );

  m_timerId = -1;
  update();
}
//=================================================================================================
void ProgressIndicator::setAnimationDelay( int delay )
{
  if (m_timerId != -1)
    killTimer( m_timerId );

  m_delay = delay;

  if (m_timerId != -1)
    m_timerId = startTimer( m_delay );
}
//=================================================================================================
void ProgressIndicator::setColor( const QColor &color )
{
  m_color = color;
  update();
}
//=================================================================================================
QSize ProgressIndicator::sizeHint() const
{
  return QSize( 20, 20 );
}
//=================================================================================================
int ProgressIndicator::heightForWidth( int w ) const
{
  return w;
}
//=================================================================================================
void ProgressIndicator::timerEvent( QTimerEvent */*event*/ )
{
  m_angle = (m_angle + 30) % 360;
  update();
}
//=================================================================================================
void ProgressIndicator::paintEvent( QPaintEvent */*event*/ )
{
  if (!m_displayedWhenStopped && !isAnimated())
    return;

  int width = qMin( this->width(), this->height() );

  QPainter p( this );
  p.setRenderHint( QPainter::Antialiasing );

  int outerRadius = int( (width - 1) * 0.5 );
  int innerRadius = int( (width - 1) * 0.25 );

  int capsuleHeight = outerRadius - innerRadius;
  int capsuleWidth  = (width > 32 ) ? int( capsuleHeight * 0.23 ) : int( capsuleHeight * 0.35 );
  int capsuleRadius = capsuleWidth / 2;

  for (int i = 0; i < 12; ++i)
  {
    QColor color = m_color;
    color.setAlphaF( 1.0f - (i / 12.0f) );
    p.setPen( Qt::NoPen );
    p.setBrush( color );
    p.save();
    p.translate( rect().center() );
    p.rotate( m_angle - i * 30.0f );
    p.drawRoundedRect( int( -capsuleWidth * 0.5 ), -(innerRadius + capsuleHeight),
      capsuleWidth, capsuleHeight, capsuleRadius, capsuleRadius );
    p.restore();
  }

  if (procent > 0)
  {
    p.save();
    p.setPen( QPen( QColor( 0, 0, 0, 255 ), 3 ) );
    p.drawArc( rect().center().x() - 14, rect().center().y() - 14, 28, 28, 1440, angle );
    p.restore();

    if (angle == 5760)
      angle = 0;
    else
    {
      p.save();
      QFont f( QApplication::font() );
      f.setPixelSize( 10 );
      p.setFont(f);
      p.setPen( QPen( QColor( 0, 0, 255, 255 ), 2 ) );
      p.drawText( QRect( rect().center().x() - 12, rect().center().y() - 12, 24, 24 ), Qt::AlignCenter, QString::number( procent ) );
      p.restore();
    }
  }
}
//=================================================================================================



//=================================================================================================
LoadingEffect::LoadingEffect( QWidget *parent ):
  QStackedWidget( parent ),
  pProgress(0),
  info(0)
{
  QFrame *fr = new QFrame( this );
  QVBoxLayout *vlFr = new QVBoxLayout( fr );

  pProgress = new ProgressIndicator( fr );
  info = new QLabel( fr );

  vlFr->addWidget( pProgress, 0, Qt::AlignHCenter );
  vlFr->addWidget( info, 0, Qt::AlignHCenter );
  vlFr->setAlignment( Qt::AlignHCenter | Qt::AlignVCenter );

  insertWidget( 0, fr );
}
//=================================================================================================
LoadingEffect::~LoadingEffect()
{
}
//=================================================================================================
void LoadingEffect::setProgress( int cur, int cnt )
{
  float p = (float)cur / (float)cnt * 100.0f;
  int ang = p * 360 / 100;
  pProgress->angle = ang << 4;
  pProgress->procent = (int)p;
}
//=================================================================================================
void LoadingEffect::install()
{
  setCurrentIndex(0);
  widget(1)->setUpdatesEnabled( false );
  widget(1)->blockSignals( true );

  pProgress->setAnimationDelay( 80 );
  pProgress->setFixedSize( 64, 64 );
  pProgress->startAnimation();
}
//=================================================================================================
void LoadingEffect::remove()
{
  setCurrentIndex(1);
  widget(1)->setUpdatesEnabled( true );
  widget(1)->blockSignals( false );
  pProgress->stopAnimation();
}
//=================================================================================================
