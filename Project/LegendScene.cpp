#include <QApplication>
#include <QFontMetrics>
#include <QFileDialog>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QImageWriter>
#include <QResizeEvent>
#include <QMenu>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QDebug>

#ifdef Qt5
  #include <QtPrintSupport/QPrinter>
  #include <QtPrintSupport/QPrintDialog>
#else
  #include <QPrinter>
  #include <QPrintDialog>
#endif

#include <qmath.h>

#include "../PathUtils.h"
#include "../LuaParser/LuaParser.h"
#include "LegendScene.h"

//=================================================================================================
LegendScene::LegendScene( const QString &name, QWidget *parent ):
  QGraphicsScene( parent ),
  projectName( name ),
  view( (QGraphicsView *)parent ),
  mousePressed( false ),
  zoom( 1.0f ),
  currentItem(0)
{
  setItemIndexMethod( QGraphicsScene::NoIndex );
}
//=================================================================================================
LegendScene::~LegendScene()
{
}
//=================================================================================================
void LegendScene::addNodes( const QStringList &nodeNames )
{
  int cnt = (int)qSqrt( nodeNames.count() );

  for (int i = 0; i < nodeNames.count(); ++i)
  {
    NodeItem *node = new NodeItem( this, nodeNames.at(i) );

    /*if (!node->isReferencesToOther())
    {
      delete node;
      node = 0;

      continue;
    }*/

    node->setFlag( QGraphicsItem::ItemIsMovable );

    if (nodes.count() > 0)
    {
      NodeItem *item = nodes.at( nodes.count() - 1 );

      if (i % cnt == 0)
        node->setPos( 20, item->pos().y() + 50 + item->boundingRect().height() );
      else
        node->setPos( item->pos().x() + 50 + item->boundingRect().width(), item->pos().y() );
    }
    else
      node->setPos( 20, 50 );

    node->setStartNode( startNodeID );

    nodes.push_back( node );
  }

  setSceneRect( 0, 0, itemsBoundingRect().width() + 20, itemsBoundingRect().height() + 20 );
  view->verticalScrollBar()->triggerAction( QScrollBar::SliderToMinimum );
  view->horizontalScrollBar()->triggerAction( QScrollBar::SliderToMinimum );
}
//=================================================================================================
void LegendScene::setStartNode( int id )
{
  startNodeID = id;
}
//=================================================================================================
void LegendScene::drawBackground( QPainter *painter, const QRectF &/*rect*/ )
{
  painter->fillRect( sceneRect(), Qt::white );
}
//=================================================================================================
// Отрисовка связей между нодами
void LegendScene::drawForeground( QPainter *painter, const QRectF &/*rect*/ )
{
  painter->beginNativePainting();

  for (int i = 0; i < nodes.count(); ++i)
  {
    NodeItem *node = nodes.at(i);
    QList<int> nodeReferences = node->getNodeReferences();

    for (int j = 0; j < nodeReferences.count(); ++j)
    {
      NodeItem *refNode = getNode( nodeReferences.at(j) );

      if (refNode)
      {
        /*float mouse_radius = getDistanceBetweenPoints( node->pos().x() + node->boundingRect().width() / 2,
                                                       node->pos().y() + node->boundingRect().height() / 2,
                                                       refNode->pos().x() + refNode->boundingRect().width() / 2,
                                                       refNode->pos().y() + refNode->boundingRect().height() / 2 );

        float dx = (refNode->pos().x() + refNode->boundingRect().width() / 2) - (node->pos().x() + node->boundingRect().width() / 2);
        float dy = (refNode->pos().y() + refNode->boundingRect().height() / 2) - (node->pos().y() + node->boundingRect().height() / 2);

        float dxn = dx * cos( i * M_PI / 180.0f ) - dy * sin( i * M_PI / 180.0f );
        float dyn = dx * sin( i * M_PI / 180.0f ) + dy * cos( i * M_PI / 180.0f );

        float newX = (node->pos().x() + node->boundingRect().width() / 2) + dxn;
        float newY = (node->pos().y() + node->boundingRect().height() / 2) + dyn;

        float pX = (node->pos().x() + node->boundingRect().width() / 2) + 20 * (newX - (node->pos().x() + node->boundingRect().width() / 2)) / mouse_radius;
        float pY = (node->pos().y() + node->boundingRect().height() / 2) + 20 * (newY - (node->pos().y() + node->boundingRect().height() / 2)) / mouse_radius;

        float dx2 = (node->pos().x() + node->boundingRect().width() / 2) - (refNode->pos().x() + refNode->boundingRect().width() / 2);
        float dy2 = (node->pos().y() + node->boundingRect().height() / 2) - (refNode->pos().y() + refNode->boundingRect().height() / 2);

        float dxn2 = dx2 * cos( i * M_PI / 180.0f ) - dy2 * sin( i * M_PI / 180.0f );
        float dyn2 = dx2 * sin( i * M_PI / 180.0f ) + dy2 * cos( i * M_PI / 180.0f );

        float newX2 = (refNode->pos().x() + refNode->boundingRect().width() / 2) + dxn2;
        float newY2 = (refNode->pos().y() + refNode->boundingRect().height() / 2) + dyn2;

        float pX2 = (refNode->pos().x() + refNode->boundingRect().width() / 2) + 20 * (newX2 - (refNode->pos().x() + refNode->boundingRect().width() / 2)) / mouse_radius;
        float pY2 = (refNode->pos().y() + refNode->boundingRect().height() / 2) + 20 * (newY2 - (refNode->pos().y() + refNode->boundingRect().height() / 2)) / mouse_radius;*/


        RArrow arrow;
        arrow.raStartPoint = QPointF( node->pos().x() + node->boundingRect().width() / 2/*pX*/, node->pos().y() + node->boundingRect().height() / 2/*pY*/ );
        arrow.raEndPoint = QPointF( refNode->pos().x() + refNode->boundingRect().width() / 2/*pX2*/, refNode->pos().y() + refNode->boundingRect().height() / 2/*pY2*/ );
        arrow.raHeadWidth = 5.0;
        arrow.raHeadHeight = 15.0;
        arrow.raBorderThickness = 2.0;
        arrow.raLineWidth = 2.0;
        arrow.raBorderColor = Qt::black;

        if (refNode->getNodeReferences().contains( node->getID() ))
        {
          arrow.raFillColor = Qt::gray;
          arrow.twoSided = true;
        }
        else
          arrow.raFillColor = Qt::white;

        arrow.raDrawArrow( painter );
      }
    }
  }

  painter->drawText( 0, 0, itemsBoundingRect().width(), 40, Qt::AlignCenter, PathUtils::getName( projectName ) );

  painter->endNativePainting();
}
//=================================================================================================
void LegendScene::mousePressEvent( QGraphicsSceneMouseEvent *e )
{
  if (e->button() == Qt::LeftButton)
  {
    if (!currentItem)
    {
      QGraphicsItem *it = itemAt( e->scenePos(), view->transform() );

      if (it)
      {
        currentItem = it;
        currentItem->setZValue( 20 );
        mousePressed = true;
        point = QPointF( e->scenePos().x() - it->pos().x(), e->scenePos().y() - it->pos().y() );
      }
    }
  }

  QGraphicsScene::mousePressEvent(e);
}
//=================================================================================================
void LegendScene::mouseReleaseEvent( QGraphicsSceneMouseEvent *e )
{
  if (e->button() == Qt::LeftButton)
  {
    mousePressed = false;

    if (currentItem)
    {
      currentItem->setZValue(0);
      currentItem = 0;
      setSceneRect( 0, 0, itemsBoundingRect().width() + 40, itemsBoundingRect().height() + 40 );
    }
  }

  QGraphicsScene::mouseReleaseEvent(e);
}
//=================================================================================================
void LegendScene::mouseMoveEvent( QGraphicsSceneMouseEvent *e )
{
  if (mousePressed)
  {
    if (currentItem)
      currentItem->setPos( e->scenePos() - point );
  }

  QGraphicsScene::mouseMoveEvent(e);
}
//=================================================================================================
void LegendScene::mouseDoubleClickEvent( QGraphicsSceneMouseEvent *e )
{
  NodeItem *node = (NodeItem *)itemAt( e->scenePos(), view->transform() );

  if (node)
    emit editNode( node->getNodeName() );

  QGraphicsScene::mouseDoubleClickEvent(e);
}
//=================================================================================================
void LegendScene::wheelEvent( QGraphicsSceneWheelEvent *e )
{
  /*view->setTransformationAnchor( QGraphicsView::AnchorUnderMouse );

  float scaleFactor = 1.15f;

  if (e->delta() > 0)
    view->scale( scaleFactor, scaleFactor );
  else
    view->scale( 1.0f / scaleFactor, 1.0f / scaleFactor );*/

  QGraphicsScene::wheelEvent(e);
}
//=================================================================================================
void LegendScene::contextMenuEvent( QGraphicsSceneContextMenuEvent *e )
{
  QMenu menu;
  menu.addAction( QIcon(":res/image.png"), tr("Save Image"), this, SLOT(saveImage()) );
  menu.addAction( QIcon(":res/print.png"), tr("Print"), this, SLOT(printImage()) );
  menu.exec( e->screenPos() );

  QGraphicsScene::contextMenuEvent(e);
}
//=================================================================================================
NodeItem *LegendScene::getNode( int id )
{
  for (int i = 0; i < nodes.count(); ++i)
  {
    if (nodes.at(i)->getID() == id)
      return nodes.at(i);
  }

  return 0;
}
//=================================================================================================
float LegendScene::getVectorLength( float dx, float dy )
{
  return qSqrt( dx * dx + dy * dy );
}
//=================================================================================================
float LegendScene::getDistanceBetweenPoints( float x1, float y1, float x2, float y2 )
{
  return getVectorLength( x2 - x1, y2 - y1 );
}
//=================================================================================================
void LegendScene::saveImage()
{
  QImageWriter writer;
  QList<QByteArray> formats = writer.supportedImageFormats();
  QStringList imgFormats;

  for (int i = 0; i < formats.count(); ++i)
    imgFormats << QString("*.%1").arg( formats.at(i).data() );

  QString selectedFilter;
  QString fileName = QFileDialog::getSaveFileName( 0, tr("Save Image"), qApp->applicationDirPath(),
                                                   imgFormats.join(";;"), &selectedFilter );

  if (!fileName.isEmpty())
  {
    clearSelection();
    setSceneRect( itemsBoundingRect() );

    QImage image( sceneRect().size().toSize(), QImage::Format_ARGB32 );
    image.fill( Qt::transparent );

    QPainter painter( &image );
    render( &painter );
    painter.end();

    writer.setFileName( fileName.append( selectedFilter.mid( 1, selectedFilter.length() - 1 ) ) );
    writer.write( image );
  }
}
//=================================================================================================
void LegendScene::printImage()
{
  QPrinter printer;

  if (QPrintDialog( &printer ).exec() == QDialog::Accepted)
  {
    QPainter painter( &printer );
    painter.setRenderHint( QPainter::Antialiasing );
    render( &painter );
  }
}
//=================================================================================================



//=================================================================================================
LegendViewer::LegendViewer( QWidget *parent ):
  QGraphicsView( parent )
{
}
//=================================================================================================
LegendViewer::~LegendViewer()
{
}
//=================================================================================================
void LegendViewer::resizeEvent( QResizeEvent *e )
{
  QGraphicsView::resizeEvent(e);
}
//=================================================================================================



//=================================================================================================
NodeItem::NodeItem( LegendScene *sc, const QString &name, QGraphicsItem *parent ):
  QGraphicsItem( parent ),
  scene( sc ),
  nodeName( name ),
  id( PathUtils::getName( name ).toInt() ),
  isStartNode( false ),
  isEndNode( false ),
  normalPen( Qt::black, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ),
  startPen( Qt::green, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ),
  finishPen( Qt::red, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ),
  dashPen( Qt::lightGray, 1, Qt::DashDotLine, Qt::RoundCap, Qt::RoundJoin ),
  referencedToOther( true )
{
  QFontMetrics fm( qApp->font() );
  fontSize.setHeight( fm.height() + 4 );

  lua = new LuaParser( nodeName + "/node.lua" );

  if (!lua->getCubeMap()->fields["front"].toString().isEmpty())
    image.load( PathUtils::nativePath( nodeName + "/" + lua->getCubeMap()->fields["front"].toString() ) );
  else if (!lua->getSlide()->fields["filename"].toString().isEmpty())
    image.load( PathUtils::nativePath( nodeName + "/" + lua->getSlide()->fields["filename"].toString() ) );
  else if (!lua->getPanel()->fields["filename"].toString().isEmpty())
    image.load( PathUtils::nativePath( nodeName + "/" + lua->getPanel()->fields["filename"].toString() ) );

  QVector<HotSpot *> hotspots = lua->getHotSpots();

  for (int i = 0; i < hotspots.count(); ++i)
  {
    HotSpot *hs = hotspots.at(i);

    if (hs->fields["target"].toInt() != -1)
      nodeReferences.push_back( hs->fields["target"].toInt() );

    if (hs->fields["onmousedown"].toString().contains("pipmak.gotonode"))
      getReferencedNodes( hs->fields["onmousedown"].toString() );

    if (hs->fields["onmouseup"].toString().contains("pipmak.gotonode"))
      getReferencedNodes( hs->fields["onmouseup"].toString() );

    if (hs->fields["onenddrag"].toString().contains("pipmak.gotonode"))
      getReferencedNodes( hs->fields["onenddrag"].toString() );

    if (hs->fields["onhilite"].toString().contains("pipmak.gotonode"))
      getReferencedNodes( hs->fields["onhilite"].toString() );

    if (hs->fields["onmouseenter"].toString().contains("pipmak.gotonode"))
      getReferencedNodes( hs->fields["onmouseenter"].toString() );

    if (hs->fields["onmouseleave"].toString().contains("pipmak.gotonode"))
      getReferencedNodes( hs->fields["onmouseleave"].toString() );

    if (hs->fields["onmousestilldown"].toString().contains("pipmak.gotonode"))
      getReferencedNodes( hs->fields["onmousestilldown"].toString() );

    if (hs->fields["onmousewithin"].toString().contains("pipmak.gotonode"))
      getReferencedNodes( hs->fields["onmousewithin"].toString() );
  }

  isEndNode = lua->isEndNode();

  delete lua;
  lua = 0;

  if (nodeReferences.isEmpty())
    referencedToOther = false;

  setFlag( QGraphicsItem::ItemIsMovable );
  setZValue(0);

  scene->addItem( this );
}
//=================================================================================================
int NodeItem::type() const
{
  return Type;
}
//=================================================================================================
QRectF NodeItem::boundingRect() const
{
  return QRectF( 0, 0, 100, 80 + fontSize.height() );
}
//=================================================================================================
void NodeItem::paint( QPainter *painter, const QStyleOptionGraphicsItem */*option*/, QWidget */*widget*/ )
{
  if (isStartNode)
    painter->setPen( startPen );
  else if (isEndNode)
    painter->setPen( finishPen );
  else
    painter->setPen( normalPen );

  painter->fillRect( QRectF( 0, 0, 100, 80 ), Qt::white );
  painter->drawImage( QRectF( 0, 0, 100, 80 ), image, QRectF( 0, 0, image.width(), image.height() ) );
  painter->drawLine( 0, 80, 100, 80 );

  painter->fillRect( QRectF( 0, 80, 100, fontSize.height() ), Qt::lightGray );

  painter->save();
  painter->setPen( normalPen );
  painter->drawText( QRectF( 0, 80, 100, fontSize.height() ), Qt::AlignCenter, QString::number( id ) );
  painter->restore();

  painter->drawRect( 0, 0, 100, 80 + fontSize.height() );
}
//=================================================================================================
void NodeItem::getReferencedNodes( const QString &src )
{
  int begin = src.indexOf("pipmak.gotonode");

  while (begin != -1)
  {
    int beginSkob = src.indexOf( "(", begin );
    ++beginSkob;
    QString sID;

    for (;;)
    {
      if (beginSkob >= src.length())
        break;

      if (src.at( beginSkob ) == ' ' || src.at( beginSkob ) == ',' || src.at( beginSkob ) == ')')
        break;

      sID += src.at( beginSkob );
      ++beginSkob;
    }

    nodeReferences.push_back( sID.trimmed().toInt() );
    begin = src.indexOf( "pipmak.gotonode", beginSkob );
  }
}
//=================================================================================================
int NodeItem::getID()
{
  return id;
}
//=================================================================================================
QList<int> NodeItem::getNodeReferences()
{
  return nodeReferences;
}
//=================================================================================================
QString NodeItem::getNodeName() const
{
  return nodeName;
}
//=================================================================================================
void NodeItem::setStartNode( int id )
{
  if (this->id == id)
    isStartNode = true;
}
//=================================================================================================
bool NodeItem::isReferencesToOther()
{
  return referencedToOther;
}
//=================================================================================================



//=================================================================================================
RArrow::RArrow()
{
  raStartPoint = QPoint( -1, -1 );
  raEndPoint = QPoint( -1, -1 );
  raLineWidth = 0;
  raHeadHeight = 0;
  raHeadWidth = 0;
  raBorderThickness = 0;
  twoSided = false;

  for (int i = 0; i < 10; ++i)
    raArrowPoints[i] = QPointF( -1, -1 );
}
//=================================================================================================
void RArrow::raSetArrowHead()
{
  double x1 = raStartPoint.x();
  double y1 = raStartPoint.y();
  double x2 = raEndPoint.x();
  double y2 = raEndPoint.y();

  double distance = sqrt( pow( x2 - x1, 2 ) + pow( y2 - y1, 2 ) );

  double dx = x2 + (x1 - x2) * raHeadHeight / distance;
  double dy = y2 + (y1 - y2) * raHeadHeight / distance;

  double k = raHeadWidth / raHeadHeight;

  double x2o = x2 - dx;
  double y2o = dy - y2;

  double x3 = y2o * k + dx;
  double y3 = x2o * k + dy;

  double x4 = dx - y2o * k;
  double y4 = dy - x2o * k;

  raArrowPoints[0] = QPointF( x4, y4 );
  raArrowPoints[1] = QPointF( x2, y2 );
  raArrowPoints[2] = QPointF( x3, y3 );
}
//=================================================================================================
void RArrow::raSetArrowLine()
{
  double x1 = raStartPoint.x();
  double y1 = raStartPoint.y();
  double x2 = (raArrowPoints[0].x() + raArrowPoints[2].x()) / 2.0;
  double y2 = (raArrowPoints[0].y() + raArrowPoints[2].y()) / 2.0;

  double k = raLineWidth / sqrt( pow( x2 - x1, 2 ) + pow( y2 - y1, 2 ) );

  double x2o = x2 - x1;
  double y2o = y1 - y2;

  double x3 = y2o * k + x1;
  double y3 = x2o * k + y1;

  double x4 = x1 - y2o * k;
  double y4 = y1 - x2o * k;

  double x1o = x1 - x2;
  double y1o = y2 - y1;

  double x5 = y1o * k + x2;
  double y5 = x1o * k + y2;

  double x6 = x2 - y1o * k;
  double y6 = y2 - x1o * k;

  raArrowPoints[4] = QPointF( x3, y3 );
  raArrowPoints[5] = QPointF( x4, y4 );
  raArrowPoints[6] = QPointF( x5, y5 );
  raArrowPoints[3] = QPointF( x6, y6 );
}
//=================================================================================================
void RArrow::raSetArrowHead2()
{
  double x1 = raEndPoint.x();
  double y1 = raEndPoint.y();
  double x2 = raStartPoint.x();
  double y2 = raStartPoint.y();

  double distance = sqrt( pow( x2 - x1, 2 ) + pow( y2 - y1, 2 ) );

  double dx = x2 + (x1 - x2) * raHeadHeight / distance;
  double dy = y2 + (y1 - y2) * raHeadHeight / distance;

  double k = raHeadWidth / raHeadHeight;

  double x2o = x2 - dx;
  double y2o = dy - y2;

  double x3 = y2o * k + dx;
  double y3 = x2o * k + dy;

  double x4 = dx - y2o * k;
  double y4 = dy - x2o * k;

  raArrowPoints[5] = QPointF( x4, y4 );
  raArrowPoints[6] = QPointF( x2, y2 );
  raArrowPoints[7] = QPointF( x3, y3 );
}
//=================================================================================================
void RArrow::raSetArrowLine2()
{
  double x1 = (raArrowPoints[0].x() + raArrowPoints[2].x()) / 2.0;
  double y1 = (raArrowPoints[0].y() + raArrowPoints[2].y()) / 2.0;
  double x2 = (raArrowPoints[5].x() + raArrowPoints[7].x()) / 2.0;
  double y2 = (raArrowPoints[5].y() + raArrowPoints[7].y()) / 2.0;

  double k = raLineWidth / sqrt( pow( x2 - x1, 2 ) + pow( y2 - y1, 2 ) );

  double x2o = x2 - x1;
  double y2o = y1 - y2;

  double x3 = y2o * k + x1;
  double y3 = x2o * k + y1;

  double x4 = x1 - y2o * k;
  double y4 = y1 - x2o * k;

  double x1o = x1 - x2;
  double y1o = y2 - y1;

  double x5 = y1o * k + x2;
  double y5 = x1o * k + y2;

  double x6 = x2 - y1o * k;
  double y6 = y2 - x1o * k;

  raArrowPoints[9] = QPointF( x3, y3 );
  raArrowPoints[3] = QPointF( x4, y4 );
  raArrowPoints[4] = QPointF( x5, y5 );
  raArrowPoints[8] = QPointF( x6, y6 );
}
//=================================================================================================
void RArrow::raDrawArrow( QPainter *p )
{
  p->setPen( QPen( QBrush( raBorderColor ), raBorderThickness, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin ) );
  p->setBrush( QBrush( raFillColor ) );

  if (!twoSided)
  {
    raSetArrowHead();
    raSetArrowLine();
  }
  else
  {
    raSetArrowHead();
    raSetArrowHead2();
    raSetArrowLine2();
  }

  p->setRenderHint( QPainter::Antialiasing, true );

  if (!twoSided)
    p->drawPolygon( raArrowPoints, 7 );
  else
    p->drawPolygon( raArrowPoints, 10 );
}
//=================================================================================================
