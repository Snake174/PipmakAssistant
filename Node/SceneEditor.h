#ifndef SCENEEDITOR_H
#define SCENEEDITOR_H

//=================================================================================================
#include <QWidget>
//=================================================================================================
QT_BEGIN_NAMESPACE
  class Node;
  class QStackedWidget;
  class PropertyEditor;
QT_END_NAMESPACE
//=================================================================================================
class SceneEditor : public QWidget
{
  Q_OBJECT

  Node *node;
  QStackedWidget *stack;
  PropertyEditor *propertyEditor;

  public:
    explicit SceneEditor( Node *node, QWidget *parent = 0 );
    ~SceneEditor();

  private slots:
    void sceneChanged( int index );
};
//=================================================================================================

#endif // SCENEEDITOR_H
