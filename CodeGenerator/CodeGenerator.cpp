#include <QCloseEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QMdiArea>
#include <QMenu>
#include <QScrollBar>
#include <QSplitter>
#include <QToolButton>
#include <QTreeWidget>
#include <QVBoxLayout>

#include "../PathUtils.h"
#include "../ScriptEditor/ScriptViewer.h"
#include "CodeGenerator.h"

CodeGenerator::CodeGenerator( QWidget *parent ):
  QMdiSubWindow( parent ),
  mdi( (QMdiArea *)parent ),
  currentItem(0)
{
  QFrame *fr = new QFrame( this );
  QVBoxLayout *vl = new QVBoxLayout( fr );

  QSplitter *split = new QSplitter( fr );

  codeTree = new QTreeWidget( split );
  codeTree->setHeaderHidden( true );
  codeTree->setColumnCount(2);
  codeTree->setEditTriggers( QAbstractItemView::DoubleClicked );
  codeTree->setSortingEnabled( false );
  codeTree->setContextMenuPolicy( Qt::CustomContextMenu );
  connect( codeTree, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showTreeMenu(QPoint)) );

  codeEditor = new ScriptViewer( split );

  split->addWidget( codeTree );
  split->addWidget( codeEditor );

  split->setSizes( QList<int>() << 150 << 200 );

  QHBoxLayout *hl = new QHBoxLayout();

  QToolButton *tbGenerate = new QToolButton( fr );
  tbGenerate->setIconSize( QSize( 24, 24 ) );
  tbGenerate->setIcon( QIcon(":res/generate.png") );
  tbGenerate->setToolTip( tr("Generate Code") );
  connect( tbGenerate, SIGNAL(clicked()), this, SLOT(generateCode()) );

  QToolButton *tbSave = new QToolButton( fr );
  tbSave->setIconSize( QSize( 24, 24 ) );
  tbSave->setIcon( QIcon(":res/save.png") );
  tbSave->setToolTip( tr("Save Code") );
  connect( tbSave, SIGNAL(clicked()), this, SLOT(saveCode()) );

  hl->addWidget( tbGenerate );
  hl->addWidget( tbSave );
  hl->setMargin(0);
  hl->setSpacing(3);
  hl->setAlignment( Qt::AlignLeft );

  vl->addLayout( hl );
  vl->addWidget( split );

  setWidget( fr );
  setWindowFlags( Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowTitleHint );
  setMinimumSize( 640, 480 );
  setWindowIcon( QIcon(":res/code_generator.png") );
  setWindowTitle( tr("Visual Code Generator") );
}

void CodeGenerator::closeEvent( QCloseEvent *e )
{
  mdi->removeSubWindow( this );
  e->accept();
}

void CodeGenerator::parseTree( QTreeWidgetItem *item, int level )
{
  if (!item->whatsThis(0).compare("state"))
  {
    addSpaces( level );
    codeText += "  state." + ((QLineEdit *)codeTree->itemWidget( item, 1 ))->text() + "\r\n";
  }
  else if (!item->whatsThis(0).compare("pipmak function"))
  {
    addSpaces( level );
    codeText += ((QLineEdit *)codeTree->itemWidget( item, 1 ))->text() + "(\r\n";
    addSpaces( level );
    codeText += "  function()\r\n";

    for (int i = 0; i < item->childCount(); ++i)
      parseTree( item->child(i), level + 2 );

    addSpaces( level );
    codeText += "  end\r\n";
    addSpaces( level );
    codeText += ")\r\n";
  }
}

void CodeGenerator::addSpaces( int cnt )
{
  for (int i = 0; i < cnt; ++i)
    codeText += " ";
}

void CodeGenerator::showTreeMenu( const QPoint &pos )
{
  QMenu menu( this );
  QMenu addMenu;
  addMenu.setIcon( QIcon(":res/add.png") );
  addMenu.setTitle( tr("Add") );
  addMenu.addAction( QIcon(""), tr("state"), this, SLOT(addItem()) );
  addMenu.addAction( QIcon(""), tr("pipmak function"), this, SLOT(addItem()) );

  menu.addMenu( &addMenu );

  currentItem = codeTree->itemAt( pos );

  if (currentItem)
  {
    menu.addSeparator();
    menu.addAction( QIcon(":res/remove.png"), tr("Remove"), this, SLOT(removeItem()) );
  }
  else
    currentItem = 0;

  menu.exec( codeTree->mapToGlobal( pos ) );
}

void CodeGenerator::generateCode()
{
  codeText.clear();

  for (int i = 0; i < codeTree->topLevelItemCount(); ++i)
    parseTree( codeTree->topLevelItem(i), 0 );

  codeEditor->setText( codeText );
  codeEditor->verticalScrollBar()->triggerAction( QScrollBar::SliderToMinimum );
  codeEditor->ensureCursorVisible();
}

void CodeGenerator::saveCode()
{
}

void CodeGenerator::removeItem()
{
  if (currentItem)
  {
    delete currentItem;
    currentItem = 0;
  }
}

void CodeGenerator::addItem()
{
  QAction *a = dynamic_cast<QAction *>( sender() );

  if (a)
  {
    QTreeWidgetItem *item;

    if (currentItem)
      item = new QTreeWidgetItem( currentItem );
    else
      item = new QTreeWidgetItem( codeTree );

    codeTree->setItemWidget( item, 1, new QLineEdit( codeTree ) );

    if (!a->text().compare("state"))
    {
      item->setIcon( 0, QIcon("") );
      item->setToolTip( 0, "state" );
      item->setWhatsThis( 0, "state" );
      item->setText( 0, "state" );
    }
    else if (!a->text().compare("pipmak function"))
    {
      item->setIcon( 0, QIcon("") );
      item->setToolTip( 0, "pipmak function" );
      item->setWhatsThis( 0, "pipmak function" );
      item->setText( 0, "pipmak function" );
    }
  }
}
