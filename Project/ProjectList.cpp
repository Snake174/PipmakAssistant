#include <QApplication>
#include <QDir>
#include <QFileDialog>
#include <QFontMetrics>
#include <QFutureWatcher>
#include <QListView>
#include <QMenu>
#include <QStack>
#include <QStandardItemModel>

#ifdef Qt5
  #include <QtConcurrent/QtConcurrentRun>
#else
  #include <QtConcurrentRun>
#endif

#include "../Structures.h"
#include "../PathUtils.h"
#include "../LoadingEffect.h"
#include "ProjectList.h"

//=================================================================================================
ProjectList::ProjectList( QWidget *parent ):
  QDockWidget( parent ),
  progress(0),
  projectList(0),
  model(0)
{
  progress = new LoadingEffect( this );

  projectList = new QListView( progress );
  projectList->setContextMenuPolicy( Qt::CustomContextMenu );
  projectList->setEditTriggers( QAbstractItemView::NoEditTriggers );
  connect( projectList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)) );
  connect( projectList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(listDoubleClicked(QModelIndex)) );

  progress->setWidget( projectList );

  QFontMetrics fm( qApp->font() );
  itemSize.setHeight( fm.height() + 4 );

  setWidget( progress );
  setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  setWindowTitle( tr("Projects") );
}
//=================================================================================================
ProjectList::~ProjectList()
{
}
//=================================================================================================
QStringList ProjectList::scanDirs( const QString &path )
{
  QStringList res;

  QStack<QString> stack;
  stack.push( path );

  while (!stack.isEmpty())
  {
    QString dirName = stack.pop();
    QDir subDir( dirName );

    QStringList files = subDir.entryList( QStringList() << "*.*", QDir::Files );

    if (dirName.endsWith( ".pipmak", Qt::CaseInsensitive ) && files.contains( "main.lua", Qt::CaseInsensitive ))
      res.push_back( dirName );

    QFileInfoList dirs = subDir.entryInfoList( QStringList(), QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot );

    for (int i = 0; i < dirs.size(); ++i)
      stack.push( dirs.at(i).absoluteFilePath() );
  }

  res.removeDuplicates();

  return res;
}
//=================================================================================================
void ProjectList::showContextMenu( const QPoint &pos )
{
  QMenu menu( this );
  menu.addAction( QIcon(":res/add.png"), tr("Add"), this, SLOT(addProjects()) );
  menu.exec( projectList->mapToGlobal( pos ) );
}
//=================================================================================================
void ProjectList::addProjects()
{
  QFileDialog fd( this );
  fd.setFileMode( QFileDialog::DirectoryOnly );

  QString projectsLocation = fd.getExistingDirectory( this, tr("Select Projects Location"), qApp->applicationDirPath() );

  if (!projectsLocation.isEmpty())
  {
    progress->setInfo( tr("Loading Projects") );
    progress->install();

    res = QtConcurrent::run( this, &ProjectList::scanDirs, projectsLocation );
    QFutureWatcher<QStringList> *watcher = new QFutureWatcher<QStringList>;
    watcher->setFuture( res );
    connect( watcher, SIGNAL(finished()), this, SLOT(setModel()) );
  }
}
//=================================================================================================
void ProjectList::setModel()
{
  projectList->setUpdatesEnabled( false );
  projectList->blockSignals( true );

  QFutureWatcher<QStringList> *watcher = dynamic_cast<QFutureWatcher<QStringList> *>( sender() );

  if (watcher)
  {
    QStringList files = watcher->result();
    QStringList storedFiles;

    if (!model)
      model = new QStandardItemModel( projectList );

    for (int i = 0; i < model->rowCount(); ++i)
      storedFiles.push_back( model->data( model->index( i, 0 ), Roles::FileName ).toString() );

    for (int i = 0; i < files.count(); ++i)
    {
      if (storedFiles.contains( files.at(i) ))
        continue;

      QStandardItem *item = new QStandardItem();
      item->setData( PathUtils::getName( files.at(i) ), Qt::DisplayRole );
      item->setData( QIcon(":res/pipmak.png"), Qt::DecorationRole );
      item->setData( itemSize, Qt::SizeHintRole );
      item->setData( files.at(i), Qt::ToolTipRole );
      item->setData( files.at(i), Roles::FileName );

      model->invisibleRootItem()->appendRow( item );
    }

    projectList->setModel( model );

    watcher->deleteLater();
  }

  projectList->blockSignals( false );
  projectList->setUpdatesEnabled( true );

  progress->remove();
}
//=================================================================================================
void ProjectList::listDoubleClicked( const QModelIndex &index )
{
  if (index.isValid() && model)
    emit loadProject( model->data( index, Roles::FileName ).toString() );
}
//=================================================================================================
void ProjectList::setCurrentProject( const QString &path )
{
  projectList->setUpdatesEnabled( false );
  projectList->blockSignals( true );

  for (int i = 0; i < model->rowCount(); ++i)
  {
    if (!path.compare( model->data( model->index( i, 0 ), Roles::FileName ).toString() ) )
    {
      projectList->setCurrentIndex( model->index( i, 0 ) );
      projectList->selectionModel()->select( model->index( i, 0 ), QItemSelectionModel::ClearAndSelect );
      break;
    }
  }

  projectList->blockSignals( false );
  projectList->setUpdatesEnabled( true );
}
//=================================================================================================
