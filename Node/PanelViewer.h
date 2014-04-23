#ifndef PANELVIEWER_H
#define PANELVIEWER_H

//=================================================================================================
#include <QGraphicsView>
//=================================================================================================
QT_BEGIN_NAMESPACE
  class Node;
  class PanelViewer;
QT_END_NAMESPACE
//=================================================================================================
class PanelScene : public QGraphicsScene
{
  Q_OBJECT

  PanelViewer *view;

  public:
    PanelScene( QWidget *parent = 0 );
    ~PanelScene();

  protected:
    void drawBackground( QPainter *painter, const QRectF &rect );
    void drawForeground( QPainter *painter, const QRectF &rect );
    void mousePressEvent( QGraphicsSceneMouseEvent *e );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *e );
    void mouseMoveEvent( QGraphicsSceneMouseEvent *e );
};
//=================================================================================================
class PanelViewer : public QGraphicsView
{
  Q_OBJECT

  Node *node;
  PanelScene *scene;

  public:
    explicit PanelViewer( Node *node, QWidget *parent = 0 );
    ~PanelViewer();

    PanelScene *getScene();
    Node *getNode();
};
//=================================================================================================

#endif // PANELVIEWER_H
