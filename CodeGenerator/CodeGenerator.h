#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <QMdiSubWindow>

QT_BEGIN_NAMESPACE
  class QTreeWidget;
  class QTreeWidgetItem;
  class ScriptViewer;
QT_END_NAMESPACE

class CodeGenerator : public QMdiSubWindow
{
  Q_OBJECT

  QMdiArea *mdi;
  QTreeWidget *codeTree;
  ScriptViewer *codeEditor;
  QTreeWidgetItem *currentItem;
  QString codeText;

  public:
    explicit CodeGenerator( QWidget *parent = 0 );

  protected:
    void closeEvent( QCloseEvent *e );

  private:
    void parseTree( QTreeWidgetItem *item, int level );
    void addSpaces( int cnt );

  private slots:
    void showTreeMenu( const QPoint &pos );
    void generateCode();
    void saveCode();
    void removeItem();
    void addItem();
};

#endif // CODEGENERATOR_H
