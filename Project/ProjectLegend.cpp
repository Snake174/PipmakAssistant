#include <QGLWidget>
#include <QDir>
#include <QFileInfoList>
#include <QScrollBar>
#include <QCloseEvent>
#include <QDebug>

#include "../PathUtils.h"
#include "LegendScene.h"
#include "ProjectLegend.h"

//=================================================================================================
ProjectLegend::ProjectLegend( const QString &projectName, QWidget *parent ):
  QMdiSubWindow( parent ),
  mdi( (QMdiArea *)parent ),
  projectName( projectName ),
  scene(0)
{
  LegendViewer *viewer = new LegendViewer( this );
  viewer->setViewport( new QGLWidget( QGLFormat( QGL::SampleBuffers | QGL::DoubleBuffer | QGL::AlphaChannel | QGL::Rgba ) ) );
  viewer->setViewportUpdateMode( QGraphicsView::BoundingRectViewportUpdate );
  viewer->setAutoFillBackground( false );

  QPalette p;
  p.setColor( QPalette::Base, Qt::white );

  viewer->setPalette(p);
  viewer->setAttribute( Qt::WA_TranslucentBackground, false );

  scene = new LegendScene( projectName, viewer );
  connect( scene, SIGNAL(editNode(QString)), this, SIGNAL(editNode(QString)) );

  QStringList nodeNames;
  QDir subdir( projectName );

  QFileInfoList infoEntries = subdir.entryInfoList( QStringList(), QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot );

  for (int i = 0; i < infoEntries.size(); ++i)
  {
    QFileInfo &item = infoEntries[i];

    if (QFile( PathUtils::nativePath( item.absoluteFilePath() + "/node.lua" ) ).exists())
      nodeNames.push_back( item.absoluteFilePath() );
  }

  QFile mainLua( PathUtils::nativePath( projectName + "/main.lua" ) );

  if (mainLua.open( QIODevice::ReadOnly ))
  {
    QByteArray ba = mainLua.readAll();

    if (ba.contains("startnode"))
    {
      int begin = ba.indexOf("startnode");
      int beginSkob = ba.indexOf( "(", begin );
      int end = ba.indexOf( ")", beginSkob + 1 );
      scene->setStartNode( ba.mid( beginSkob + 1, end - beginSkob - 1 ).toInt() );
    }

    mainLua.close();
  }
  else
    qDebug() << mainLua.errorString();

  scene->addNodes( nodeNames );

  viewer->setScene( scene );

  QScrollBar *vScrollBar = viewer->verticalScrollBar();
  vScrollBar->triggerAction( QScrollBar::SliderToMinimum );

  QScrollBar *hScrollBar = viewer->horizontalScrollBar();
  hScrollBar->triggerAction( QScrollBar::SliderToMinimum );

  setWidget( viewer );
  setMouseTracking( true );
  setWindowFlags( Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowTitleHint );
  setWindowIcon( QIcon(":res/legend.png") );
  setWindowTitle( tr("Project Inspector") + " : " + PathUtils::getName( projectName ) );
}
//=================================================================================================
ProjectLegend::~ProjectLegend()
{
}
//=================================================================================================
QString ProjectLegend::getProjectName() const
{
  return projectName;
}
//=================================================================================================
void ProjectLegend::closeEvent( QCloseEvent *e )
{
  mdi->removeSubWindow( this );
  e->accept();
}
//=================================================================================================
