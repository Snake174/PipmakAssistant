#ifndef LEGENDSCENE_H
#define LEGENDSCENE_H

//=================================================================================================
#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsView>
//=================================================================================================
QT_BEGIN_NAMESPACE
  class LuaParser;
  class NodeItem;
QT_END_NAMESPACE
//=================================================================================================
class LegendScene : public QGraphicsScene
{
  Q_OBJECT

  QString projectName;
  QGraphicsView *view;
  bool mousePressed;
  float zoom;
  QList<NodeItem *> nodes;
  int startNodeID;
  QPointF point;
  QGraphicsItem *currentItem;

  public:
    LegendScene( const QString &name, QWidget *parent = 0 );
    ~LegendScene();

    void addNodes( const QStringList &nodeNames );
    void setStartNode( int id );

  protected:
    void drawBackground( QPainter *painter, const QRectF &rect );
    void drawForeground( QPainter *painter, const QRectF &rect );
    void mousePressEvent( QGraphicsSceneMouseEvent *e );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *e );
    void mouseMoveEvent( QGraphicsSceneMouseEvent *e );
    void mouseDoubleClickEvent( QGraphicsSceneMouseEvent *e );
    void wheelEvent( QGraphicsSceneWheelEvent *e );
    void contextMenuEvent( QGraphicsSceneContextMenuEvent *e );

  private:
    NodeItem *getNode( int id );
    float getVectorLength( float dx, float dy );
    float getDistanceBetweenPoints( float x1, float y1, float x2, float y2 );

  private slots:
    void saveImage();
    void printImage();

  signals:
    void editNode( const QString & );
};
//=================================================================================================
class LegendViewer : public QGraphicsView
{
  Q_OBJECT

  public:
    LegendViewer( QWidget *parent = 0 );
    ~LegendViewer();

  protected:
    void resizeEvent( QResizeEvent *e );
};
//=================================================================================================
class NodeItem : public QGraphicsItem
{
  LegendScene *scene;
  QString nodeName;
  int id;
  bool isStartNode;
  bool isEndNode;
  QSize fontSize;
  QImage image;
  LuaParser *lua;
  QPen normalPen;
  QPen startPen;
  QPen finishPen;
  QPen dashPen;
  QList<int> nodeReferences;
  bool referencedToOther;

  public:
    enum
    {
      Type = UserType + 1
    };

    int type() const;

    NodeItem( LegendScene *sc, const QString &name, QGraphicsItem *parent = 0 );

    QRectF boundingRect() const;
    int getID();
    QList<int> getNodeReferences();
    QString getNodeName() const;
    void setStartNode( int id );
    bool isReferencesToOther();

  protected:
    void paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget );

  private:
    void getReferencedNodes( const QString &src );
};
//=================================================================================================
class RArrow
{
  public:
    RArrow();

    void raDrawArrow( QPainter *p );

    void raSetArrowHead();
    void raSetArrowLine();

    void raSetArrowHead2();
    void raSetArrowLine2();

  public:
    QPointF raArrowPoints[ 10 ];

    QPointF raStartPoint;
    QPointF raEndPoint;

    double raLineWidth;

    double raHeadWidth;
    double raHeadHeight;

    QColor raBorderColor;
    QColor raFillColor;
    double raBorderThickness;

    bool twoSided;
};
//=================================================================================================

#endif // LEGENDSCENE_H
