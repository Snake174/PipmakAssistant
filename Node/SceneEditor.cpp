#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolBox>
#include <QScrollArea>
#include <QFrame>
#include <QToolButton>
#include <QStackedWidget>
#include <QSignalMapper>
#include <QDebug>

#include "../PropertyEditor/Property.h"
#include "../LuaParser/LuaParser.h"
#include "Node.h"
#include "CubeMapViewer.h"
#include "SlideViewer.h"
#include "PanelViewer.h"
#include "SceneEditor.h"

//=================================================================================================
SceneEditor::SceneEditor( Node *node, QWidget *parent ):
  QWidget( parent ),
  node( node )
{
  QVBoxLayout *vl = new QVBoxLayout( this );

  QSplitter *splitter = new QSplitter( Qt::Horizontal, this );

  stack = new QStackedWidget( splitter );

  stack->addWidget( new CubeMapViewer( node, stack ) );
  stack->addWidget( new SlideViewer( node, stack ) );
  stack->addWidget( new PanelViewer( node, stack ) );
  stack->setCurrentIndex(0);

  QFrame *fr = new QFrame( this );
  QHBoxLayout *hl = new QHBoxLayout( fr );

  QSignalMapper *sm = new QSignalMapper( this );

  QToolButton *tbCubeMap = new QToolButton( fr );
  tbCubeMap->setIconSize( QSize( 24, 24 ) );
  tbCubeMap->setIcon( QIcon(":res/cubemap.png") );
  tbCubeMap->setAutoExclusive( true );
  tbCubeMap->setCheckable( true );
  tbCubeMap->setChecked( true );
  tbCubeMap->setToolTip( tr("CubeMap") );
  connect( tbCubeMap, SIGNAL(clicked()), sm, SLOT(map()) );

  QToolButton *tbSlide = new QToolButton( fr );
  tbSlide->setIconSize( QSize( 24, 24 ) );
  tbSlide->setIcon( QIcon(":res/slide.png") );
  tbSlide->setAutoExclusive( true );
  tbSlide->setCheckable( true );
  tbSlide->setChecked( false );
  tbSlide->setToolTip( tr("Slide") );
  connect( tbSlide, SIGNAL(clicked()), sm, SLOT(map()) );

  QToolButton *tbPanel = new QToolButton( fr );
  tbPanel->setIconSize( QSize( 24, 24 ) );
  tbPanel->setIcon( QIcon(":res/panel.png") );
  tbPanel->setAutoExclusive( true );
  tbPanel->setCheckable( true );
  tbPanel->setChecked( false );
  tbPanel->setToolTip( tr("Panel") );
  connect( tbPanel, SIGNAL(clicked()), sm, SLOT(map()) );

  sm->setMapping( tbCubeMap, 0 );
  sm->setMapping( tbSlide, 1 );
  sm->setMapping( tbPanel, 2 );
  connect( sm, SIGNAL(mapped(int)), this, SLOT(sceneChanged(int)) );

  hl->addWidget( tbCubeMap );
  hl->addWidget( tbSlide );
  hl->addWidget( tbPanel );
  hl->setSpacing(0);
  hl->setMargin(0);
  hl->setAlignment( Qt::AlignLeft | Qt::AlignTop );

  QScrollArea *sa = new QScrollArea( splitter );
  sa->setWidgetResizable( true );

  QToolBox *toolBox = new QToolBox( sa );

  propertyEditor = new PropertyEditor( toolBox );

  node->getLua()->getCubeMap()->setEditor( propertyEditor );
  node->getLua()->getSlide()->setEditor( propertyEditor );
  node->getLua()->getPanel()->setEditor( propertyEditor );

  if (!node->getLua()->getCubeMap()->fields["front"].toString().isEmpty())
  {
    node->getLua()->getCubeMap()->setCurrent();
    tbCubeMap->setChecked( true );
    stack->setCurrentIndex(0);
  }
  else if (!node->getLua()->getSlide()->fields["filename"].toString().isEmpty())
  {
    node->getLua()->getSlide()->setCurrent();
    tbSlide->setChecked( true );
    stack->setCurrentIndex(1);
  }
  else if (!node->getLua()->getPanel()->fields["filename"].toString().isEmpty())
  {
    node->getLua()->getPanel()->setCurrent();
    tbPanel->setChecked( true );
    stack->setCurrentIndex(2);
  }

  toolBox->addItem( propertyEditor, QIcon(":res/scene.png"), tr("Scene") );

  sa->setWidget( toolBox );

  splitter->addWidget( sa );
  splitter->addWidget( stack );

  vl->addWidget( fr );
  vl->addWidget( splitter, 1 );

  splitter->setSizes( QList<int>() << 120 << 200 );
}
//=================================================================================================
SceneEditor::~SceneEditor()
{
}
//=================================================================================================
void SceneEditor::sceneChanged( int index )
{
  stack->setCurrentIndex( index );

  switch (index)
  {
    case 0: node->getLua()->getCubeMap()->setCurrent(); break;
    case 1: node->getLua()->getSlide()->setCurrent(); break;
    case 2: node->getLua()->getPanel()->setCurrent(); break;
  }
}
//=================================================================================================
