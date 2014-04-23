#include <QApplication>
#include <QFrame>
#include <QFileDialog>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QGroupBox>
#include <QSignalMapper>
#include <QSplitter>
#include <QTreeWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QImageReader>
#include <QListWidget>
#include <QScrollArea>
#include <QToolBox>
#include <QDebug>

#include "../Singleton.h"
#include "../PathUtils.h"
#include "../LuaParser/LuaParser.h"
#include "../PropertyEditor/Property.h"
#include "Node.h"
#include "PatchViewer.h"
#include "PatchEditor.h"

//=================================================================================================
PatchEditor::PatchEditor( Node *node, QWidget *parent ):
  QWidget( parent ),
  node( node )
{
  QImageReader reader;

  for (int i = 0; i < reader.supportedImageFormats().count(); ++i)
    imageFormats << QString("*.%1").arg( reader.supportedImageFormats().at(i).data() );

  QFrame *fr = new QFrame( this );
  QHBoxLayout *hlFr = new QHBoxLayout( fr );

  faces = new QComboBox( fr );
  faces->addItem( "FRONT", 0 );
  faces->addItem( "RIGHT", 1 );
  faces->addItem( "BACK", 2 );
  faces->addItem( "LEFT", 3 );
  faces->addItem( "TOP", 4 );
  faces->addItem( "BOTTOM", 5 );
  faces->setEditable( false );

  hlFr->addWidget( new QLabel( tr("Face"), fr ) );
  hlFr->addSpacing( 10 );
  hlFr->addWidget( faces );
  hlFr->setSpacing(0);
  hlFr->setMargin(0);
  hlFr->setAlignment( Qt::AlignLeft | Qt::AlignTop );

  QSplitter *splitter = new QSplitter( this );

  viewer = new PatchViewer( node, splitter );

  connect( faces, SIGNAL(currentIndexChanged(int)), viewer, SLOT(setCurrentFace(int)) );

  QScrollArea *sa = new QScrollArea( splitter );
  sa->setWidgetResizable( true );

  QToolBox *toolBox = new QToolBox( sa );

  QVector<Patch *> p = node->getLua()->getPatches();

  for (int i = 0; i < p.count(); ++i)
  {
    PropertyEditor *propertyEditor = new PropertyEditor( toolBox );
    p.at(i)->setEditor( propertyEditor );
    p.at(i)->setCurrent();
    toolBox->addItem( propertyEditor, QIcon(":res/patch.png"), tr("Patch") );

    viewer->getScene()->addPatchItem( node->getNodeName(), p.at(i)->getProp("image").toString().replace( "\"", "" ), p.at(i) );
  }

  sa->setWidget( toolBox );

  splitter->addWidget( sa );
  splitter->addWidget( viewer );

  QVBoxLayout *mainLayout = new QVBoxLayout( this );
  mainLayout->addWidget( fr );
  mainLayout->addWidget( splitter, 1 );

  splitter->setSizes( QList<int>() << 120 << 200 );
}
//=================================================================================================
PatchEditor::~PatchEditor()
{
}
//=================================================================================================
Node *PatchEditor::getNode()
{
  return node;
}
//=================================================================================================
void PatchEditor::faceChanged( int index )
{
  viewer->getScene()->setCurrentFace( index );
}
//=================================================================================================
void PatchEditor::selectImage( QWidget *lineEdit )
{
  QString fileName = QFileDialog::getOpenFileName( this, tr("Select Image"), qApp->applicationDirPath(),
                                                   QString( tr("Image Files (%1)") ).arg( imageFormats.join(" ") ) );

  if (!fileName.isEmpty())
    ((QLineEdit *)lineEdit)->setText( PathUtils::getName( fileName ) );
}
//=================================================================================================
void PatchEditor::patchChanged( int index )
{
  node->getLua()->getPatches().at( index )->setCurrent();
}
//=================================================================================================
void PatchEditor::patchVisibleChanged( QListWidgetItem *it )
{
  QListWidget *list = dynamic_cast<QListWidget *>( sender() );

  if (list)
  {
    list->setCurrentItem( it, QItemSelectionModel::ClearAndSelect );

    if (it->checkState() == Qt::Checked)
      viewer->getScene()->setPatchVisible( list->currentIndex().row(), true );
    else
      viewer->getScene()->setPatchVisible( list->currentIndex().row(), false );
  }
}
//=================================================================================================
