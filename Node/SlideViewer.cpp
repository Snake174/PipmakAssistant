#include <QGLWidget>

#include "../PathUtils.h"
#include "Node.h"
#include "../LuaParser/LuaParser.h"
#include "SlideViewer.h"

//=================================================================================================
SlideScene::SlideScene( QWidget *parent ):
  QGraphicsScene( parent ),
  view( (SlideViewer *)parent ),
  image(0)
{
  if (!view->getNode()->getLua()->getSlide()->fields["filename"].toString().isEmpty())
  {
    image = new QImage( PathUtils::nativePath( view->getNode()->getNodeName() + "/"
                                               + view->getNode()->getLua()->getSlide()->fields["filename"].toString() ) );

    //view->setBackgroundBrush( QPixmap( PathUtils::nativePath( view->getNode()->getNodeName() + "/"
    //                                                          + view->getNode()->getLua()->getSlide()->fields["filename"].toString() ) ) );

    setSceneRect( 0, 0, image->width(), image->height() );
    update();
  }

  setItemIndexMethod( QGraphicsScene::NoIndex );
}
//=================================================================================================
SlideScene::~SlideScene()
{
  delete image;
  image = 0;
}
//=================================================================================================
void SlideScene::drawBackground( QPainter *painter, const QRectF &/*rect*/ )
{
  painter->fillRect( sceneRect(), Qt::lightGray );
}
//=================================================================================================
void SlideScene::drawForeground( QPainter *painter, const QRectF &/*rect*/ )
{
  if (!image)
    return;

  painter->drawImage( sceneRect(), *image );
}
//=================================================================================================
void SlideScene::mousePressEvent( QGraphicsSceneMouseEvent *e )
{
}
//=================================================================================================
void SlideScene::mouseReleaseEvent( QGraphicsSceneMouseEvent *e )
{
}
//=================================================================================================
void SlideScene::mouseMoveEvent( QGraphicsSceneMouseEvent *e )
{
}
//=================================================================================================



//=================================================================================================
SlideViewer::SlideViewer( Node *node, QWidget *parent):
  QGraphicsView( parent ),
  node( node )
{
  setViewport( new QGLWidget( QGLFormat( QGL::SampleBuffers | QGL::DoubleBuffer | QGL::AlphaChannel | QGL::Rgba ) ) );
  setViewportUpdateMode( QGraphicsView::FullViewportUpdate );
  setAutoFillBackground( false );
  setCacheMode( QGraphicsView::CacheBackground );
  setAlignment( Qt::AlignLeft | Qt::AlignTop );

  QPalette p;
  p.setColor( QPalette::Base, Qt::white );

  setPalette(p);
  setAttribute( Qt::WA_TranslucentBackground, false );

  scene = new SlideScene( this );
  setScene( scene );
}
//=================================================================================================
SlideViewer::~SlideViewer()
{
}
//=================================================================================================
SlideScene *SlideViewer::getScene()
{
  return scene;
}
//=================================================================================================
Node *SlideViewer::getNode()
{
  return node;
}
//=================================================================================================
