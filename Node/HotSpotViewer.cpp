#include <QGLWidget>
#include <QGraphicsSceneWheelEvent>
#include <QMenu>
#include <QDebug>

#include "../PathUtils.h"
#include "Node.h"
#include "../LuaParser/LuaParser.h"
#include "HotSpotViewer.h"

//=================================================================================================
HotSpotScene::HotSpotScene( QWidget *parent ):
  QGraphicsScene( parent ),
  view( (HotSpotViewer *)parent ),
  image(0),
  hotspotMap(0),
  zoom(1),
  showHotspotMap( false )
{
  setEquirect();
}
//=================================================================================================
HotSpotScene::~HotSpotScene()
{
  if (image)
  {
    delete image;
    image = 0;
  }

  if (hotspotMap)
  {
    delete hotspotMap;
    hotspotMap = 0;
  }
}
//=================================================================================================
void HotSpotScene::setEquirect()
{
  if (!view->getNode()->getLua()->getCubeMap()->fields["front"].toString().isEmpty())
    image = view->getNode()->getEquirectImage();
  else if (!view->getNode()->getLua()->getSlide()->fields["filename"].toString().isEmpty())
    image = new QImage( QImage( PathUtils::nativePath( view->getNode()->getNodeName() + "/" + view->getNode()->getLua()->getSlide()->fields["filename"].toString() ) ) );
  else if (!view->getNode()->getLua()->getPanel()->fields["filename"].toString().isEmpty())
    image = new QImage( QImage( PathUtils::nativePath( view->getNode()->getNodeName() + "/" + view->getNode()->getLua()->getPanel()->fields["filename"].toString() ) ) );

  if (!view->getNode()->getLua()->getHotSpotMap().fileName.isEmpty())
    hotspotMap = view->getNode()->getHotspotMapImage();

  imageSize.setWidth( image->width() );
  imageSize.setHeight( image->height() );

  setSceneRect( 0, 0, image->width(), image->height() );
  update();
}
//=================================================================================================
void HotSpotScene::setHotspotMapVisible()
{
  showHotspotMap = !showHotspotMap;
}
//=================================================================================================
void HotSpotScene::drawBackground( QPainter *painter, const QRectF &/*rect*/ )
{
  painter->fillRect( sceneRect(), Qt::lightGray );
}
//=================================================================================================
void HotSpotScene::drawForeground( QPainter *painter, const QRectF &/*rect*/ )
{
  if (!image)
    return;

  painter->drawImage( sceneRect(), *image );

  if (showHotspotMap)
    painter->drawImage( sceneRect(), *hotspotMap );

  int x = mousePos.x();
  int y = mousePos.y();
  int w = imageSize.width() * zoom;
  int h = imageSize.height() * zoom;

  if (x > 0 && x < w && y > 0 && y < h)
  {
    painter->save();
    painter->setPen( QPen( QBrush( Qt::green ), 3 ) );
    painter->drawLine( 0, y, 5, y );
    painter->drawLine( w - 5, y, w, y );
    painter->drawLine( x, 0, x, 5 );
    painter->drawLine( x, h - 5, x, h );
    painter->restore();
  }
}
//=================================================================================================
void HotSpotScene::mousePressEvent( QGraphicsSceneMouseEvent *e )
{
  QGraphicsScene::mousePressEvent(e);

  if (e->isAccepted())
    return;
}
//=================================================================================================
void HotSpotScene::mouseReleaseEvent( QGraphicsSceneMouseEvent *e )
{
}
//=================================================================================================
void HotSpotScene::mouseMoveEvent( QGraphicsSceneMouseEvent *e )
{
  QGraphicsScene::mouseMoveEvent(e);

  if (e->isAccepted())
    return;

  mousePos = e->scenePos();
  update();
}
//=================================================================================================
void HotSpotScene::wheelEvent( QGraphicsSceneWheelEvent *e )
{
  QGraphicsScene::wheelEvent(e);

  if (e->isAccepted())
    return;

  if (e->delta() < 0)
  {
    if (zoom < 4)
      ++zoom;
  }
  else
  {
    if (zoom > 1)
      --zoom;
  }

  setSceneRect( 0, 0, image->width() * zoom, image->height() * zoom );
  update();
}
//=================================================================================================
void HotSpotScene::contextMenuEvent( QGraphicsSceneContextMenuEvent *e )
{
  QMenu menu;
  menu.addAction( QIcon(":res/image.png"), showHotspotMap == false ? tr("Show Hotspot Map") : tr("Hide Hotspot Map"), this, SLOT(setHotspotMapVisible()) );
  menu.exec( e->screenPos() );

  QGraphicsScene::contextMenuEvent(e);
}
//=================================================================================================



//=================================================================================================
HotSpotViewer::HotSpotViewer( Node *node, QWidget *parent ):
  QGraphicsView( parent ),
  node( node )
{
  setViewport( new QGLWidget( QGLFormat( QGL::SampleBuffers | QGL::DoubleBuffer | QGL::AlphaChannel | QGL::Rgba ) ) );
  setViewportUpdateMode( QGraphicsView::FullViewportUpdate );
  setAutoFillBackground( false );
  setCacheMode( QGraphicsView::CacheBackground );
  setAlignment( Qt::AlignLeft | Qt::AlignTop );
  setMouseTracking( true );

  QPalette p;
  p.setColor( QPalette::Base, Qt::white );

  setPalette(p);
  setAttribute( Qt::WA_TranslucentBackground, false );

  scene = new HotSpotScene( this );
  setScene( scene );
}
//=================================================================================================
HotSpotViewer::~HotSpotViewer()
{
}
//=================================================================================================
HotSpotScene *HotSpotViewer::getScene()
{
  return scene;
}
//=================================================================================================
Node *HotSpotViewer::getNode()
{
  return node;
}
//=================================================================================================
