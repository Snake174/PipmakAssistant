#ifndef PROJECTTREE_H
#define PROJECTTREE_H

//=================================================================================================
#include <QDockWidget>
#include <QModelIndex>
//=================================================================================================
QT_BEGIN_NAMESPACE
  class QStackedWidget;
  class QThread;
QT_END_NAMESPACE
//=================================================================================================
class ProjectTree : public QDockWidget
{
  Q_OBJECT

  QStackedWidget *treeStack;
  QStringList imageFormats;
  QStringList soundFormats;
  QSize itemSize;
  QThread *checkImageSizesThread;

  public:
    explicit ProjectTree( QWidget *parent = 0 );
    ~ProjectTree();

  public slots:
    void setCurrentProject( int index );
    void loadProject( const QString &path );
    void removeProject( int index );
    void nodeDoubleClicked( const QModelIndex &index );

  private slots:
    void startCheckSizes();

  private:
    QVariant displayData( const QString &name );

  signals:
    void editNode( const QString & );
    void notify( const QString &, const QStringList &, QSize );
    void playSound( const QString & );
};
//=================================================================================================

#endif // PROJECTTREE_H
