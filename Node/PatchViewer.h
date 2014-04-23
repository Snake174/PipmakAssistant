#ifndef PATCHVIEWER_H
#define PATCHVIEWER_H

//=================================================================================================
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
//=================================================================================================
QT_BEGIN_NAMESPACE
  class FaceItem;
  class PatchItem;
  class Node;
  class PatchViewer;
  class Patch;
QT_END_NAMESPACE
//=================================================================================================
namespace ItemType
{
  enum
  {
    Background = QGraphicsItem::UserType + 1,
    Patch = QGraphicsItem::UserType + 2
  };
}
//=================================================================================================
class PatchScene : public QGraphicsScene
{
  Q_OBJECT

  PatchViewer *view;
  QList<FaceItem *> faceItems;
  QList<PatchItem *> patchItems;
  QList<QImage> faces;
  int currentFace;

  public:
    PatchScene( QWidget *parent = 0 );
    ~PatchScene();

    void addFaceItem( const QString &nodeName, const QString &image, Patch *patch );
    void addPatchItem( const QString &nodeName, const QString &image, Patch *patch );
    void setPatchVisible( int index, bool on );
    void setCurrentFace( int face );

  protected:
    void drawBackground( QPainter *painter, const QRectF &rect );
    void drawForeground( QPainter *painter, const QRectF &rect );
    void mousePressEvent( QGraphicsSceneMouseEvent *e );
    void mouseReleaseEvent( QGraphicsSceneMouseEvent *e );
    void mouseMoveEvent( QGraphicsSceneMouseEvent *e );
};
//=================================================================================================
class PatchViewer : public QGraphicsView
{
  Q_OBJECT

  Node *node;
  PatchScene *scene;

  public:
    explicit PatchViewer( Node *node, QWidget *parent = 0 );
    ~PatchViewer();

    PatchScene *getScene();
    Node *getNode();

  public slots:
    void setCurrentFace( int face );
};
//=================================================================================================

#endif // PATCHVIEWER_H
