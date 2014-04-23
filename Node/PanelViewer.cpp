#include <QGLWidget>

#include "../PathUtils.h"
#include "Node.h"
#include "../LuaParser/LuaParser.h"
#include "PanelViewer.h"

//=================================================================================================
PanelScene::PanelScene( QWidget *parent ):
  QGraphicsScene( parent ),
  view( (PanelViewer *)parent )
{
  if (!view->getNode()->getLua()->getPanel()->fields["filename"].toString().isEmpty())
  {
    QImage image( PathUtils::nativePath( view->getNode()->getNodeName() + "/"
                                         + view->getNode()->getLua()->getPanel()->fields["filename"].toString() ) );

    view->setBackgroundBrush( QPixmap( PathUtils::nativePath( view->getNode()->getNodeName() + "/"
                                                              + view->getNode()->getLua()->getPanel()->fields["filename"].toString() ) ) );

    setSceneRect( 0, 0, image.width(), image.height() );
    update();
  }

  setItemIndexMethod( QGraphicsScene::NoIndex );
}
//=================================================================================================
PanelScene::~PanelScene()
{
}
//=================================================================================================
void PanelScene::drawBackground( QPainter *painter, const QRectF &/*rect*/ )
{
  painter->fillRect( sceneRect(), Qt::lightGray );
}
//=================================================================================================
void PanelScene::drawForeground( QPainter *painter, const QRectF &rect )
{
}
//=================================================================================================
void PanelScene::mousePressEvent( QGraphicsSceneMouseEvent *e )
{
}
//=================================================================================================
void PanelScene::mouseReleaseEvent( QGraphicsSceneMouseEvent *e )
{
}
//=================================================================================================
void PanelScene::mouseMoveEvent( QGraphicsSceneMouseEvent *e )
{
}
//=================================================================================================



//=================================================================================================
PanelViewer::PanelViewer( Node *node, QWidget *parent):
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

  scene = new PanelScene( this );
  setScene( scene );
}
//=================================================================================================
PanelViewer::~PanelViewer()
{
}
//=================================================================================================
PanelScene *PanelViewer::getScene()
{
  return scene;
}
//=================================================================================================
Node *PanelViewer::getNode()
{
  return node;
}
//=================================================================================================

