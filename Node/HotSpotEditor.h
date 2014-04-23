#ifndef HOTSPOTEDITOR_H
#define HOTSPOTEDITOR_H

//=================================================================================================
#include <QMainWindow>
//=================================================================================================
QT_BEGIN_NAMESPACE
  class Node;
  class HotSpotViewer;
QT_END_NAMESPACE
//=================================================================================================
class HotSpotEditor : public QWidget
{
  Q_OBJECT

  Node *node;
  HotSpotViewer *viewer;

  public:
    explicit HotSpotEditor( Node *node, QWidget *parent = 0 );
    ~HotSpotEditor();
};
//=================================================================================================

#endif // HOTSPOTEDITOR_H
