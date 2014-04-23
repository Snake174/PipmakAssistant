#ifndef NODE_H
#define NODE_H

//=================================================================================================
#include <QMdiSubWindow>
#include <QMdiArea>
//=================================================================================================
QT_BEGIN_NAMESPACE
  class QStackedWidget;
  class QVBoxLayout;
  class LuaParser;
QT_END_NAMESPACE
//=================================================================================================
class Node : public QMdiSubWindow
{
  Q_OBJECT

  QMdiArea *mdi;
  QString nodeName;
  QStackedWidget *stack;
  QVBoxLayout *buttonsLayout;
  LuaParser *lua;

  public:
    explicit Node( const QString &nodeName, QWidget *parent = 0 );
    ~Node();

    QString getNodeName() const;
    LuaParser *getLua();
    QImage *getEquirectImage();
    QImage *getHotspotMapImage();

  protected:
    void closeEvent( QCloseEvent *e );
};
//=================================================================================================

#endif // NODE_H
