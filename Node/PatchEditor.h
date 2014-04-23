#ifndef PATCHEDITOR_H
#define PATCHEDITOR_H

//=================================================================================================
#include <QWidget>
#include <QVariant>
#include <QListWidgetItem>
//=================================================================================================
QT_BEGIN_NAMESPACE
  class QComboBox;
  class QTreeWidget;
  class QTreeWidgetItem;
  class Node;
  class PropertyEditor;
  class PatchViewer;
QT_END_NAMESPACE
//=================================================================================================
class PatchEditor : public QWidget
{
  Q_OBJECT

  PatchViewer *viewer;
  Node *node;
  QStringList imageFormats;

  public:
    explicit PatchEditor( Node *node, QWidget *parent = 0 );
    ~PatchEditor();

    QComboBox *faces;

    Node *getNode();

  private slots:
    void faceChanged( int index );
    void selectImage( QWidget *lineEdit );
    void patchChanged( int index );
    void patchVisibleChanged( QListWidgetItem *it );
};
//=================================================================================================

#endif // PATCHEDITOR_H
