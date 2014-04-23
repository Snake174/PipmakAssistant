#include <QGLWidget>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QScrollBar>
#include <QDebug>

#include "Node.h"
#include "../PathUtils.h"
#include "../LuaParser/LuaParser.h"
#include "PatchViewer.h"

//=================================================================================================
class FaceItem : public QGraphicsItem
{
  PatchScene *scene;
  QImage image;

  public:
    FaceItem( PatchScene *sc, const QString &nodeName, const QString &imageName, QGraphicsItem *parent = 0 ):
      QGraphicsItem( parent ),
      scene( sc ),
      face( -1 ),
      visible( true )
    {
      if (!image.load( PathUtils::nativePath( nodeName + "/" + imageName ) ))
        qDebug() << "Can't load image: " << nodeName + "/" + imageName;
      else
      {
        qDebug() << "Load image: " << nodeName + "/" + imageName;
        width = image.width();
        height = image.height();
      }

      setZValue(0);
      scene->addItem( this );
    }

    int width;
    int height;
    int face;
    bool visible;

    enum { Type = ItemType::Background };
    int type() const { return Type; }
    QRectF boundingRect() const { return QRectF( 0, 0, width, height ); }

  protected:
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *, QWidget * )
    {
      if (!isVisible())
        return;

      painter->drawImage( 0, 0, image );
    }
};
//=================================================================================================
class PatchItem : public QGraphicsItem
{
  PatchScene *scene;
  QImage image;

  public:
    PatchItem( PatchScene *sc, const QString &nodeName, const QString &imageName, QGraphicsItem *parent = 0 ):
      QGraphicsItem( parent ),
      scene( sc ),
      x(0),
      y(0),
      width(0),
      height(0),
      face( -1 ),
      visible( true )
    {
      if (!image.load( PathUtils::nativePath( nodeName + "/" + imageName ) ))
        qDebug() << "Can't load image: " << nodeName + "/" + imageName;
      else
      {
        qDebug() << "Load image: " << nodeName + "/" + imageName;
        width = image.width();
        height = image.height();
      }

      setZValue( 20 );
      scene->addItem( this );
    }

    int x;
    int y;
    int width;
    int height;
    int face;
    bool visible;

    enum { Type = ItemType::Patch };
    int type() const { return Type; }
    QRectF boundingRect() const { return QRectF( 0, 0, width, height ); }

    QRectF resizeHandle() const
    {
      QPointF br = boundingRect().bottomRight();
      return QRectF( br - QPointF( 6, 6 ), br ) ;
    }

  protected:
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *, QWidget * )
    {
      if (!visible)
        return;

      painter->drawImage( QRectF( x, y, width, height ), image, QRectF( 0, 0, image.width(), image.height() ) );
      painter->drawRect( x, y, width, height );
      painter->setBrush( Qt::white );
      painter->drawRect( resizeHandle().adjusted( 0, 0, 3, 3 ) );
    }
};
//=================================================================================================



//=================================================================================================
PatchScene::PatchScene( QWidget *parent ):
  QGraphicsScene( parent ),
  view( (PatchViewer *)parent ),
  currentFace(0)
{
  if (!view->getNode()->getLua()->getCubeMap()->fields["front"].toString().isEmpty())
  {
    faces << QImage( PathUtils::nativePath( view->getNode()->getNodeName() + "/" + view->getNode()->getLua()->getCubeMap()->fields["front"].toString() ) )
          << QImage( PathUtils::nativePath( view->getNode()->getNodeName() + "/" + view->getNode()->getLua()->getCubeMap()->fields["right"].toString() ) )
          << QImage( PathUtils::nativePath( view->getNode()->getNodeName() + "/" + view->getNode()->getLua()->getCubeMap()->fields["back"].toString() ) )
          << QImage( PathUtils::nativePath( view->getNode()->getNodeName() + "/" +  view->getNode()->getLua()->getCubeMap()->fields["left"].toString() ) )
          << QImage( PathUtils::nativePath( view->getNode()->getNodeName() + "/" + view->getNode()->getLua()->getCubeMap()->fields["top"].toString() ) )
          << QImage( PathUtils::nativePath( view->getNode()->getNodeName() + "/" + view->getNode()->getLua()->getCubeMap()->fields["bottom"].toString() ) );
  }
  else if (!view->getNode()->getLua()->getSlide()->fields["filename"].toString().isEmpty())
    faces << QImage( PathUtils::nativePath( view->getNode()->getNodeName() + "/" + view->getNode()->getLua()->getSlide()->fields["filename"].toString() ) );
  else if (!view->getNode()->getLua()->getPanel()->fields["filename"].toString().isEmpty())
    faces << QImage( PathUtils::nativePath( view->getNode()->getNodeName() + "/" + view->getNode()->getLua()->getPanel()->fields["filename"].toString() ) );

  setItemIndexMethod( QGraphicsScene::NoIndex );
  setSceneRect( 0, 0, faces.at(0).width(), faces.at(0).height() );

  update();
}
//=================================================================================================
PatchScene::~PatchScene()
{
}
//=================================================================================================
void PatchScene::addFaceItem( const QString &nodeName , const QString &image , Patch *patch )
{
  FaceItem *face = new FaceItem( this, nodeName, image );
  face->setPos( 0, 0 );
  face->face = patch->getProp("face").toInt();

  faceItems.push_back( face );
  update();
}
//=================================================================================================
void PatchScene::addPatchItem( const QString &nodeName, const QString &image , Patch *patch )
{
  PatchItem *p = new PatchItem( this, nodeName, image );
  p->setPos( patch->getProp("x").toInt(), patch->getProp("y").toInt() );
  p->face = patch->getProp("face").toInt();

  patchItems.push_back(p);
  update();
}
//=================================================================================================
void PatchScene::setPatchVisible( int index , bool on )
{
  patchItems.at( index )->visible = on;
  update();
}
//=================================================================================================
void PatchScene::setCurrentFace( int face )
{
  if (face >= faces.count())
    return;

  currentFace = face;
  setSceneRect( 0, 0, faces.at( currentFace ).width(), faces.at( currentFace ).height() );
  update();
}
//=================================================================================================
void PatchScene::drawBackground( QPainter *painter, const QRectF &/*rect*/ )
{
  painter->fillRect( sceneRect(), Qt::lightGray );
  painter->drawImage( sceneRect(), faces.at( currentFace ) );
}
//=================================================================================================
void PatchScene::drawForeground( QPainter */*painter*/, const QRectF &/*rect*/ )
{
}
//=================================================================================================
void PatchScene::mousePressEvent( QGraphicsSceneMouseEvent *e )
{
  QGraphicsScene::mousePressEvent(e);
}
//=================================================================================================
void PatchScene::mouseReleaseEvent( QGraphicsSceneMouseEvent *e )
{
  QGraphicsScene::mouseReleaseEvent(e);
}
//=================================================================================================
void PatchScene::mouseMoveEvent( QGraphicsSceneMouseEvent *e )
{
  QGraphicsScene::mouseMoveEvent(e);
}
//=================================================================================================



//=================================================================================================
PatchViewer::PatchViewer( Node *node, QWidget *parent ):
  QGraphicsView( parent ),
  node( node ),
  scene(0)
{
  setViewport( new QGLWidget( QGLFormat( QGL::SampleBuffers | QGL::DoubleBuffer | QGL::AlphaChannel | QGL::Rgba ) ) );
  setViewportUpdateMode( QGraphicsView::FullViewportUpdate );
  setAutoFillBackground( false );
  setAlignment( Qt::AlignLeft | Qt::AlignTop );

  QPalette p;
  p.setColor( QPalette::Base, Qt::white );

  setPalette(p);
  setAttribute( Qt::WA_TranslucentBackground, false );

  scene = new PatchScene( this );
  setScene( scene );
}
//=================================================================================================
PatchViewer::~PatchViewer()
{
}
//=================================================================================================
PatchScene *PatchViewer::getScene()
{
  return scene;
}
//=================================================================================================
Node *PatchViewer::getNode()
{
  return node;
}
//=================================================================================================
void PatchViewer::setCurrentFace( int face )
{
  scene->setCurrentFace( face );
}
//=================================================================================================
