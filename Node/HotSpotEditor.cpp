#include <QApplication>
#include <QScrollArea>
#include <QSplitter>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QToolBox>
#include <QDebug>

#include <qmath.h>

#include "../PathUtils.h"
#include "../LuaParser/LuaParser.h"
#include "Node.h"
#include "HotSpotViewer.h"
#include "HotSpotEditor.h"

//=================================================================================================
HotSpotEditor::HotSpotEditor( Node *node, QWidget *parent ):
  QWidget( parent ),
  node( node )
{
  QVBoxLayout *mainLayout = new QVBoxLayout( this );

  QSplitter *splitter = new QSplitter( this );

  viewer = new HotSpotViewer( node, splitter );

  QScrollArea *sa = new QScrollArea( splitter );
  sa->setWidgetResizable( true );

  QToolBox *toolBox = new QToolBox( sa );

  QVector<HotSpot *> p = node->getLua()->getHotSpots();

  for (int i = 0; i < p.count(); ++i)
  {
    PropertyEditor *propertyEditor = new PropertyEditor( toolBox );
    p.at(i)->setEditor( propertyEditor );
    p.at(i)->setCurrent();
    toolBox->addItem( propertyEditor, QIcon(":res/hotspots.png"), tr("Hotspot") );
  }

  sa->setWidget( toolBox );

  splitter->addWidget( sa );
  splitter->addWidget( viewer );

  mainLayout->addWidget( splitter );

  splitter->setSizes( QList<int>() << 120 << 200 );
}
//=================================================================================================
HotSpotEditor::~HotSpotEditor()
{
}
//=================================================================================================
