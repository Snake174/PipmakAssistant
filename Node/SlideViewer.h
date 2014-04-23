#ifndef SLIDEVIEWER_H
#define SLIDEVIEWER_H

//=================================================================================================
#include <QGraphicsView>
//=================================================================================================
QT_BEGIN_NAMESPACE
  class Node;
  class SlideViewer;
QT_END_NAMESPACE
//=================================================================================================
class SlideScene : public QGraphicsScene
{
  Q_OBJECT

  SlideViewer *view;
  QImage *image;

  public:
    SlideScene( QWidget *parent = 0 );
    ~SlideScene();

  protected:
    void drawBackground( QPainter *painter, const QRectF &rect );
    void drawForeground( QPainter *painter, const QRectF &rect );
    void mousePressEvent( QGraphicsSceneMouseEvent *e );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *e );
    void mouseMoveEvent( QGraphicsSceneMouseEvent *e );
  };
//=================================================================================================
class SlideViewer : public QGraphicsView
{
  Q_OBJECT

  Node *node;
  SlideScene *scene;

  public:
    explicit SlideViewer( Node *node, QWidget *parent = 0 );
    ~SlideViewer();

    SlideScene *getScene();
    Node *getNode();
};
//=================================================================================================

#endif // SLIDEVIEWER_H
