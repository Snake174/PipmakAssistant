#ifndef SCRIPTEDITOR_H
#define SCRIPTEDITOR_H

//=================================================================================================
#include <QWidget>
//=================================================================================================
QT_BEGIN_NAMESPACE
  class ScriptViewer;
  class QPushButton;
  class QTreeWidget;
  class QTreeWidgetItem;
  class QPlainTextEdit;
  class Node;
QT_END_NAMESPACE
//=================================================================================================
class ScriptEditor : public QWidget
{
  Q_OBJECT

  Node *node;
  ScriptViewer *viewer;
  QString scriptName;
  QTreeWidget *funcTree;
  QPlainTextEdit *funcDescr;
  bool isChanged;

  public:
    explicit ScriptEditor( Node *node, QWidget *parent = 0 );
    ~ScriptEditor();

    void loadFile( const QString &fileName );

  protected:
    bool eventFilter( QObject *target, QEvent *e );

  private slots:
    void apply();
    void cancel();
    void textChanged();
    void showFuncDescr( QTreeWidgetItem *it, int col );
};
//=================================================================================================

#endif // SCRIPTEDITOR_H
