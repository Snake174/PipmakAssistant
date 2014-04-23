#include <QApplication>
#include <QDir>
#include <QFontMetrics>
#include <QImageReader>
#include <QStack>
#include <QStackedWidget>
#include <QStandardItemModel>
#include <QTreeView>
#include <QDebug>

#include "../Singleton.h"
#include "../Structures.h"
#include "../PathUtils.h"
#include "../LuaParser/LuaParser.h"
#include "ProjectTree.h"

//=================================================================================================
ProjectTree::ProjectTree( QWidget *parent ):
  QDockWidget( parent ),
  treeStack(0)
{
  checkImageSizesThread = new QThread( this );
  //moveToThread( checkImageSizesThread );
  connect( checkImageSizesThread, SIGNAL(started()), this, SLOT(startCheckSizes()) );

  QImageReader reader;

  for (int i = 0; i < reader.supportedImageFormats().count(); ++i)
    imageFormats << QString("%1").arg( reader.supportedImageFormats().at(i).data() );

  soundFormats << "ogg"; // << "wav" << "mp3";

  QFontMetrics fm( qApp->font() );
  itemSize.setHeight( fm.height() + 4 );

  treeStack = new QStackedWidget( this );

  setWidget( treeStack );
  setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
  setWindowTitle( tr("Project Structure") );
}
//=================================================================================================
ProjectTree::~ProjectTree()
{
}
//=================================================================================================
void ProjectTree::setCurrentProject( int index )
{
  treeStack->setCurrentIndex( index );
}
//=================================================================================================
void ProjectTree::loadProject( const QString &path )
{
  QStringList nodes;

  QTreeView *tree = new QTreeView( treeStack );
  tree->setHeaderHidden( true );
  tree->setEditTriggers( QAbstractItemView::NoEditTriggers );
  tree->setSortingEnabled( true );

  QStandardItemModel *model = new QStandardItemModel( tree );

  QStandardItem *rootNode = model->invisibleRootItem();
  rootNode->setData( path, Roles::FileName );
  rootNode->setData( ObjectType::Project, Roles::ObjectType );

  QStack<QStandardItem *> stack;
  stack.push( rootNode );

  while (!stack.isEmpty())
  {
    QStandardItem *rootItem = stack.pop();
    QString sSubdir = rootItem->data( Roles::FileName ).toString();
    QDir subdir( sSubdir );

    QStringList entries = subdir.entryList( QStringList() << "*.*", QDir::Files );

    for (int i = 0; i < entries.size(); ++i)
    {
      QString fileName = QString("%1/%2").arg( rootItem->data( Roles::FileName ).toString() ).arg( entries.at(i) );
      QString ext = PathUtils::getExt( fileName );
      QStandardItem *child = 0;

      if (imageFormats.contains( ext, Qt::CaseInsensitive ))
      {
        child = new QStandardItem();
        child->setData( displayData( fileName ), Qt::DisplayRole );
        child->setData( QIcon(":res/image.png"), Qt::DecorationRole );
        child->setData( fileName, Roles::FileName );
        child->setData( ObjectType::Image, Roles::ObjectType );
        child->setData( itemSize, Qt::SizeHintRole );
      }
      else if (soundFormats.contains( ext, Qt::CaseInsensitive ))
      {
        child = new QStandardItem();
        child->setData( displayData( fileName ), Qt::DisplayRole );
        child->setData( QIcon(":res/sound.png"), Qt::DecorationRole );
        child->setData( fileName, Roles::FileName );
        child->setData( ObjectType::Sound, Roles::ObjectType );
        child->setData( itemSize, Qt::SizeHintRole );
      }
      else if (!ext.compare( "lua", Qt::CaseInsensitive ))
      {
        child = new QStandardItem();
        child->setData( displayData( fileName ), Qt::DisplayRole );
        child->setData( QIcon(":res/script.png"), Qt::DecorationRole );
        child->setData( fileName, Roles::FileName );
        child->setData( ObjectType::Script, Roles::ObjectType );
        child->setData( itemSize, Qt::SizeHintRole );
      }

      if (child)
        rootItem->appendRow( child );
    }

    QFileInfoList infoEntries = subdir.entryInfoList( QStringList(), QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot );

    for (int i = 0; i < infoEntries.size(); ++i)
    {
      QFileInfo &item = infoEntries[i];
      QStandardItem *it = 0;

      nodes << PathUtils::nativePath( item.absoluteFilePath() );

      if (QFile( PathUtils::nativePath( item.absoluteFilePath() + "/node.lua" ) ).exists())
      {
        it = new QStandardItem();
        it->setData( displayData( item.absoluteFilePath() ), Qt::DisplayRole );
        it->setData( QIcon(":res/location.png"), Qt::DecorationRole );
        it->setData( item.absoluteFilePath(), Roles::FileName );
        it->setData( ObjectType::Location, Roles::ObjectType );
        it->setData( itemSize, Qt::SizeHintRole );
      }
      else
      {
        it = new QStandardItem();
        it->setData( displayData( item.absoluteFilePath() ), Qt::DisplayRole );
        it->setData( QIcon(":res/folder.png"), Qt::DecorationRole );
        it->setData( item.absoluteFilePath(), Roles::FileName );
        it->setData( ObjectType::Folder, Roles::ObjectType );
        it->setData( itemSize, Qt::SizeHintRole );
      }

      if (it)
      {
        rootItem->appendRow( it );
        stack.push( it );
      }
    }
  }

  tree->setModel( model );
  tree->sortByColumn( 0, Qt::AscendingOrder );
  tree->scrollToTop();
  connect( tree, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(nodeDoubleClicked(QModelIndex)) );

  treeStack->addWidget( tree );

  checkImageSizesThread->setObjectName( nodes.join(";") );
  checkImageSizesThread->start();
}
//=================================================================================================
void ProjectTree::removeProject( int index )
{
  disconnect( treeStack->widget( index ), 0, 0, 0 );
  treeStack->removeWidget( treeStack->widget( index ) );
}
//=================================================================================================
void ProjectTree::nodeDoubleClicked( const QModelIndex &index )
{
  QTreeView *tree = dynamic_cast<QTreeView *>( sender() );

  if (tree)
  {
    QString nodeName = PathUtils::nativePath( tree->model()->data( index, Roles::FileName ).toString() );
    int objectType = tree->model()->data( index, Roles::ObjectType ).toInt();

    if (objectType == ObjectType::Location)
      emit editNode( nodeName );
    else if (objectType == ObjectType::Sound)
      emit playSound( nodeName );
  }
}
// Определяем все ли изображения имеют одинаковый размер
//=================================================================================================
void ProjectTree::startCheckSizes()
{
  QStringList nodes = checkImageSizesThread->objectName().split(";");

  for (int i = 0; i < nodes.count(); ++i)
  {
    if (!QFile( PathUtils::nativePath( nodes.at(i) + "/node.lua" ) ).exists())
      continue;

    LuaParser *lua = new LuaParser( PathUtils::nativePath( nodes.at(i) + "/node.lua" ), this );

    if (!lua->getCubeMap()->fields["front"].toString().isEmpty())
    {
      QStringList images;
      images << PathUtils::nativePath( nodes.at(i) + "/" + lua->getCubeMap()->fields["front"].toString() )
             << PathUtils::nativePath( nodes.at(i) + "/" + lua->getCubeMap()->fields["right"].toString() )
             << PathUtils::nativePath( nodes.at(i) + "/" + lua->getCubeMap()->fields["back"].toString() )
             << PathUtils::nativePath( nodes.at(i) + "/" + lua->getCubeMap()->fields["left"].toString() )
             << PathUtils::nativePath( nodes.at(i) + "/" + lua->getCubeMap()->fields["top"].toString() )
             << PathUtils::nativePath( nodes.at(i) + "/" + lua->getCubeMap()->fields["bottom"].toString() );

      QImage front( images.at(0) );
      QImage right( images.at(1) );
      QImage back( images.at(2) );
      QImage left( images.at(3) );
      QImage top( images.at(4) );
      QImage bottom( images.at(5) );

      QList<QSize> sizes;
      sizes << QSize( front.width(), front.height() )
            << QSize( right.width(), right.height() )
            << QSize( back.width(), back.height() )
            << QSize( left.width(), left.height() )
            << QSize( top.width(), top.height() )
            << QSize( bottom.width(), bottom.height() );

      bool isEqual = true;
      QSize maxSize( 0, 0 );

      for (int i = 0; i < sizes.count(); ++i)
      {
        for (int j = 0; j < sizes.count(); ++j)
        {
          if (i == j)
            continue;

          if (sizes.at(i) != sizes.at(j))
          {
            isEqual = false;
            QSize size;

            if (sizes.at(i).width() > sizes.at(j).width())
              size = sizes.at(i);
            else
              size = sizes.at(j);

            if (size.width() > maxSize.width())
              maxSize = size;
          }
        }
      }

      if (!isEqual)
        emit notify( PathUtils::getName( nodes.at(i) ), images, maxSize );

      delete lua;
      lua = 0;
    }
  }

  checkImageSizesThread->exit(0);
}
//=================================================================================================
QVariant ProjectTree::displayData( const QString &name )
{
  bool ok = false;

  QString sId = PathUtils::getName( name );
  int id = sId.toInt( &ok );

  if (ok)
    return id;

  return sId;
}
//=================================================================================================
