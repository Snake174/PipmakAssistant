#ifndef HOTSPOTVIEWER_H
#define HOTSPOTVIEWER_H

//=================================================================================================
#include <QGraphicsView>
//=================================================================================================
QT_BEGIN_NAMESPACE
  class Node;
  class HotSpotViewer;
QT_END_NAMESPACE
//=================================================================================================
class HotSpotScene : public QGraphicsScene
{
  Q_OBJECT

  HotSpotViewer *view;
  QImage *image;
  QImage *hotspotMap;
  int zoom;
  bool showHotspotMap;

  public:
    HotSpotScene( QWidget *parent = 0 );
    ~HotSpotScene();

  protected:
    void drawBackground( QPainter *painter, const QRectF &rect );
    void drawForeground( QPainter *painter, const QRectF &rect );
    void mousePressEvent( QGraphicsSceneMouseEvent *e );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *e );
    void mouseMoveEvent( QGraphicsSceneMouseEvent *e );
    void wheelEvent( QGraphicsSceneWheelEvent *e );
    void contextMenuEvent( QGraphicsSceneContextMenuEvent *e );

  public slots:
    void setEquirect();

  private slots:
    void setHotspotMapVisible();
};
//=================================================================================================
class HotSpotViewer : public QGraphicsView
{
  Q_OBJECT

  Node *node;
  HotSpotScene *scene;

  public:
    explicit HotSpotViewer( Node *node, QWidget *parent = 0 );
    ~HotSpotViewer();

    HotSpotScene *getScene();
    Node *getNode();
};
//=================================================================================================

#endif // HOTSPOTVIEWER_H
