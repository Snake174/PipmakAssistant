#ifndef PROJECTLIST_H
#define PROJECTLIST_H

//=================================================================================================
#include <QDockWidget>
#include <QFuture>
#include <QModelIndex>
//=================================================================================================
QT_BEGIN_NAMESPACE
  class QListView;
  class QStandardItemModel;
  class LoadingEffect;
QT_END_NAMESPACE
//=================================================================================================
class ProjectList : public QDockWidget
{
  Q_OBJECT

  LoadingEffect *progress;
  QListView *projectList;
  QStandardItemModel *model;
  QSize itemSize;
  QFuture<QStringList> res;

  public:
    explicit ProjectList( QWidget *parent = 0 );
    ~ProjectList();

  private:
    QStringList scanDirs( const QString &path );

  private slots:
    void showContextMenu( const QPoint &pos );
    void setModel();
    void listDoubleClicked( const QModelIndex &index );

  public slots:
    void setCurrentProject( const QString &path );
    void addProjects();

  signals:
    void loadProject( const QString & );
};
//=================================================================================================

#endif // PROJECTLIST_H
