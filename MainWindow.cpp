#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QFile>
#include <QMdiArea>
#include <QStyle>
#include <QShowEvent>
#include <QTabWidget>
#include <QTextCodec>
#include <QTextStream>
#include <QToolBar>
#include <QTreeView>
#include <QToolButton>
#include <QDir>
#include <QHBoxLayout>
#include <QPainter>
#include <QProcess>
#include <QTimer>
#include <QStyleFactory>
#include <QDebug>

#include "Singleton.h"
#include "PathUtils.h"
#include "Project/ProjectList.h"
#include "Project/ProjectTree.h"
#include "Project/ProjectLegend.h"
#include "Node/Node.h"
#include "SettingsDialog/SettingsDialog.h"
#include "PipmakResource/PipmakResource.h"
#include "wigglywidget.h"
#include "About.h"
#include "Sound/AudioPlayer.h"
#include "MainWindow.h"

//=================================================================================================
MainWindow::MainWindow( QWidget *parent ):
  QMainWindow( parent ),
  settings(0)
{
  createMenus();
  createDocks();

  projectTab = new QTabWidget( this );
  projectTab->setTabsClosable( true );
  connect( projectTab, SIGNAL(tabCloseRequested(int)), this, SLOT(closeProject(int)) );
  connect( projectTab, SIGNAL(currentChanged(int)), this, SLOT(setCurrentProject(int)) );

  connect( projectTree, SIGNAL(editNode(QString)), this, SLOT(editNode(QString)) );
  connect( projectTree, SIGNAL(notify(QString,QStringList,QSize)), this, SLOT(notify(QString,QStringList,QSize)) );
  connect( projectTree, SIGNAL(playSound(QString)), this, SLOT(playSound(QString)) );

  pipmakResource = new PipmakResource( this );

  if (!QDir( PathUtils::nativePath( qApp->applicationDirPath() + "/Data" ) ).exists())
    QDir().mkdir( PathUtils::nativePath( qApp->applicationDirPath() + "/Data" ) );

  if (!pipmakResource->decompress( PathUtils::nativePath( qApp->applicationDirPath() + "/Pipmak Resources" ),
                                   PathUtils::nativePath( qApp->applicationDirPath() + "/Data" ) ))
  {
    qDebug("Can't unpack Pipmak Resources file");
  }

  setCentralWidget( projectTab );
  setWindowTitle( tr("Pipmak Assistant") );
}
//=================================================================================================
MainWindow::~MainWindow()
{
  removeDir( PathUtils::nativePath( qApp->applicationDirPath() + "/Data" ) );
}
//=================================================================================================
void MainWindow::centerAndResize()
{
  QSize availableSize = qApp->desktop()->availableGeometry().size();
  int width  = availableSize.width();
  int height = availableSize.height();
  width *= 0.7;
  height *= 0.7;

  QSize newSize( width, height );

  setGeometry( QStyle::alignedRect( Qt::LeftToRight, Qt::AlignCenter, newSize, qApp->desktop()->availableGeometry() ) );

  projectTree->setMinimumHeight( height * 1.5f / 3 );
}
//=================================================================================================
void MainWindow::readSettings()
{
  // Стиль
  QString currentStyle = Singleton<Settings>::Ptr().getValue( "Style", "Current" );

  if (!currentStyle.isEmpty())
    qApp->setStyle( QStyleFactory::create( currentStyle ) );

  // Горячие клавиши
  QMap<QString, QString> shortcuts = Singleton<Settings>::Ptr().getKeyValueMap("Keyboard");

  for (int i = 0; i < actions.count(); ++i)
  {
    QAction *act = actions.at(i);
    QList<QKeySequence> keys;
    QStringList keyList = shortcuts.value( act->whatsThis() ).split(";");

    for (int j = 0; j < keyList.count(); ++j)
      keys << keyList.at(j);

    act->setShortcuts( keys );
  }
}
//=================================================================================================
void MainWindow::showEvent( QShowEvent *e )
{
  projectTree->setMinimumHeight( 150 );
  e->accept();
}
//=================================================================================================
void MainWindow::createMenus()
{
  QToolBar *tbProject = new QToolBar( tr("Project"), this );
  tbProject->setIconSize( QSize( 32, 32 ) );

  QAction *aNewProject = new QAction( QIcon(":res/new.png"), tr("New Project"), this );
  aNewProject->setWhatsThis("NewProject");
  connect( aNewProject, SIGNAL(triggered()), this, SLOT(createProject()) );

  QAction *aOpenProjects = new QAction( QIcon(":res/open.png"), tr("Open Projects"), this );
  aOpenProjects->setWhatsThis("OpenProjects");
  connect( aOpenProjects, SIGNAL(triggered()), this, SLOT(openProject()) );

  QAction *aRunProject = new QAction( QIcon(":res/start.png"), tr("Run Project"), this );
  aRunProject->setWhatsThis("RunProject");
  connect( aRunProject, SIGNAL(triggered()), this, SLOT(runProject()) );

  QAction *aProjectLegend = new QAction( QIcon(":res/legend.png"), tr("Project Legend"), this );
  aProjectLegend->setWhatsThis("ProjectLegend");
  connect( aProjectLegend, SIGNAL(triggered()), this, SLOT(showProjectLegend()) );

  QAction *aSettings = new QAction( QIcon(":res/settings.png"), tr("Settings"), this );
  aSettings->setWhatsThis("Settings");
  connect( aSettings, SIGNAL(triggered()), this, SLOT(showSettings()) );

  QAction *aUpdate = new QAction( QIcon(":res/update.png"), tr("Update"), this );
  aUpdate->setWhatsThis("Update");
  connect( aUpdate, SIGNAL(triggered()), this, SLOT(checkUpdates()) );

  tbProject->addAction( aNewProject );
  tbProject->addAction( aOpenProjects );
  tbProject->addSeparator();
  tbProject->addAction( aRunProject );
  tbProject->addSeparator();
  tbProject->addAction( aProjectLegend );
  tbProject->addSeparator();
  tbProject->addAction( aSettings );
  tbProject->addSeparator();
  tbProject->addAction( aUpdate );

  actions << aNewProject << aOpenProjects << aRunProject << aProjectLegend << aSettings << aUpdate;

  QToolButton *tb = new QToolButton( this );
  tb->setIcon( QIcon(":res/about.png") );
  tb->setIconSize( QSize( 32, 32 ) );
  tb->setToolTip( tr("About Program") );
  tb->setFocusPolicy( Qt::NoFocus );
  connect( tb, SIGNAL(clicked()), this, SLOT(showAbout()) );

  WigglyWidget *fr = new WigglyWidget( this );
  //fr->setText("GUI Editor for Pipmak Game Engine");
  fr->setFocusPolicy( Qt::NoFocus );
  QHBoxLayout *hl = new QHBoxLayout( fr );
  hl->addStretch(1);

  tbProject->addWidget( fr );
  tbProject->addWidget( tb );

  addToolBar( tbProject );
}
//=================================================================================================
void MainWindow::createDocks()
{
  projectList = new ProjectList( this );
  addDockWidget( Qt::LeftDockWidgetArea, projectList );

  projectTree = new ProjectTree( this );
  addDockWidget( Qt::LeftDockWidgetArea, projectTree );

  connect( projectList, SIGNAL(loadProject(QString)), this, SLOT(loadProject(QString)) );
}
//=================================================================================================
void MainWindow::removeDir( const QString &dirName )
{
  QDir dir( dirName );

  if (dir.exists( dirName ))
  {
    foreach (QFileInfo info, dir.entryInfoList( QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst ))
    {
      if (info.isDir())
        removeDir( PathUtils::nativePath( info.absoluteFilePath() ) );
      else
        QFile::remove( PathUtils::nativePath( info.absoluteFilePath() ) );
    }

    dir.rmdir( dirName );
  }
}
//=================================================================================================
// Функция загружает структуру проекта при щелчке по проекту в окне "Projects".
// Если проект был уже загружен, делаем его активным, если не был ещё загружен - создаём новую вкладку
// и загружаем структуру проекта в окно "Project Structure"
void MainWindow::loadProject( const QString &path )
{
  bool isOpen = false;

  for (int i = 0; i < projectTab->count(); ++i)
  {
    if (!projectTab->tabWhatsThis(i).compare( path ))
    {
      projectTab->setCurrentIndex(i);
      projectTree->setCurrentProject(i);
      isOpen = true;
      break;
    }
  }

  if (!isOpen)
  {
    projectTab->blockSignals( true );

    QMdiArea *mdi = new QMdiArea( projectTab );
    mdi->setHorizontalScrollBarPolicy( Qt::ScrollBarAsNeeded );
    mdi->setVerticalScrollBarPolicy( Qt::ScrollBarAsNeeded );

    projectTab->addTab( mdi, QIcon(":res/project.png"), PathUtils::getName( path ) );
    projectTab->setTabWhatsThis( projectTab->count() - 1, path );
    projectTab->setTabToolTip( projectTab->count() - 1, path );
    projectTab->setCurrentIndex( projectTab->count() - 1 );

    projectTab->blockSignals( false );

    projectTree->loadProject( path );
    projectTree->setCurrentProject( projectTab->count() - 1 );
  }

  setWindowTitle( tr("Pipmak Assistant") + " : " + PathUtils::getName( path ) );
}
//=================================================================================================
void MainWindow::createProject()
{
}
//=================================================================================================
// При закрытии вкладки убираем структуру проекта из окна "Project Structure"
void MainWindow::closeProject( int index )
{
  QWidget *wgt = projectTab->widget( index );

  if (wgt)
  {
    delete wgt;
    wgt = 0;
  }

  projectTree->removeProject( index );
}
//=================================================================================================
void MainWindow::setCurrentProject( int index )
{
  QString currentProject = projectTab->tabWhatsThis( index );

  if (!currentProject.isEmpty())
  {
    projectList->setCurrentProject( currentProject );
    projectTree->setCurrentProject( index );
    setWindowTitle( tr("Pipmak Assistant") + " : " + PathUtils::getName( currentProject ) );
  }
  else
    setWindowTitle( tr("Pipmak Assistant") );
}
//=================================================================================================
void MainWindow::openProject()
{
  projectList->addProjects();
}
//=================================================================================================
void MainWindow::runProject()
{
  if (!projectTab->count())
    return;

  QString project = projectTab->tabWhatsThis( projectTab->currentIndex() );

  // Убираем из файла project/main.lua комментарии, т.к. pipmak выдаёт ошибку при запуске
  // -- comment
  // --[[ multiline comment ]]
  QFile mainLua( project + "/main.lua" );

  if (!mainLua.open( QIODevice::ReadOnly ))
    return;

  QTextStream ts( &mainLua );
  //ts.setCodec( QTextCodec::codecForName("UTF-8") );
  ts.setAutoDetectUnicode( true );
  ts.setGenerateByteOrderMark( false );

  QStringList src;

  while (!ts.atEnd())
    src << ts.readLine().trimmed();

  src.removeAll("");

  mainLua.close();

  QString srcStr = src.join("\r\n");
  QStringList substr;

  // Убираем многострочные комментарии
  int begin = srcStr.indexOf("--[[");

  while (begin != -1)
  {
    int end = srcStr.indexOf( "]]", begin + 4, Qt::CaseSensitive );

    if (end != -1)
    {
      substr << srcStr.mid( begin, end - begin + 2 );
      begin = srcStr.indexOf("--[[", end + 2 );
    }
  }

  for (int i = 0; i < substr.count(); ++i)
    srcStr.replace( substr.at(i), "" );

  // Убираем однострочные комментарии
  substr.clear();

  begin = srcStr.indexOf("--");

  while (begin != -1)
  {
    int end = srcStr.indexOf( "\r\n", begin + 2, Qt::CaseSensitive );

    if (end != -1)
    {
      substr << srcStr.mid( begin, end - begin );
      begin = srcStr.indexOf("--", end );
    }
  }

  for (int i = 0; i < substr.count(); ++i)
    srcStr.replace( substr.at(i), "" );

  src.clear();
  src = srcStr.split("\r\n");
  src.removeAll("");

  // Создаём новый файл project/_main.lua
  QFile _mainLua( project + "/_main.lua" );

  if (!_mainLua.open( QIODevice::WriteOnly ))
    return;

  QTextStream tsNew( &_mainLua );
  //tsNew.setCodec( QTextCodec::codecForName("UTF-8") );
  tsNew.setAutoDetectUnicode( true );
  tsNew.setGenerateByteOrderMark( false );

  for (int i = 0; i < src.count(); ++i)
    tsNew << src.at(i) + "\r\n";

  _mainLua.close();

  // Запуск процесса pipmak.exe project/_main.lua
  QProcess *p = new QProcess( this );
  p->setWorkingDirectory( qApp->applicationDirPath() );
  p->setEnvironment( QProcess::systemEnvironment() );
  p->setProcessChannelMode( QProcess::MergedChannels );
  p->setObjectName( project + "/_main.lua" );
  p->start( PathUtils::nativePath( qApp->applicationDirPath() + "/pipmak" ),
            QStringList() << PathUtils::nativePath( project + "/_main.lua" ) );
  connect( p, SIGNAL(started()), this, SLOT(processStarted()) );
  connect( p, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)) );
  connect( p, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processError(QProcess::ProcessError)) );
}
//=================================================================================================
void MainWindow::processStarted()
{
  qDebug() << "Process started";
}
//=================================================================================================
void MainWindow::processFinished( int exitCode, QProcess::ExitStatus exitStatus )
{
  qDebug() << "Process finished" << exitCode << exitStatus;

  QProcess *p = dynamic_cast<QProcess *>( sender() );

  if (p)
  {
    QFile f( PathUtils::nativePath( p->objectName() ) );

    if (f.exists())
      f.remove();

    p->deleteLater();
  }
}
//=================================================================================================
void MainWindow::processError( QProcess::ProcessError error )
{
  qDebug() << "Process error" << error;
}
//=================================================================================================
void MainWindow::editNode( const QString &nodeName )
{
  QMdiArea *mdi = (QMdiArea *)projectTab->currentWidget();

  bool isOpen = false;

  for (int i = 0; i < mdi->subWindowList().count(); ++i)
  {
    Node *node = dynamic_cast<Node *>( mdi->subWindowList().at(i) );

    if (node && !nodeName.compare( node->getNodeName() ))
    {
      mdi->setActiveSubWindow( node );
      isOpen = true;
      break;
    }
  }

  if (!isOpen)
  {
    Node *node = new Node( nodeName, mdi );
    mdi->addSubWindow( node );
    node->show();
  }
}
//=================================================================================================
void MainWindow::showProjectLegend()
{
  if (!projectTab->count())
    return;

  QString projectName = projectTab->tabWhatsThis( projectTab->currentIndex() );
  QMdiArea *mdi = (QMdiArea *)projectTab->currentWidget();

  bool isOpen = false;

  for (int i = 0; i < mdi->subWindowList().count(); ++i)
  {
    ProjectLegend *legend = dynamic_cast<ProjectLegend *>( mdi->subWindowList().at(i) );

    if (legend && !projectName.compare( legend->getProjectName() ))
    {
      {
        mdi->setActiveSubWindow( legend );
        isOpen = true;
        break;
      }
    }
  }

  if (!isOpen)
  {
    ProjectLegend *legend = new ProjectLegend( projectName, mdi );
    mdi->addSubWindow( legend );
    legend->show();
    connect( legend, SIGNAL(editNode(QString)), this, SLOT(editNode(QString)) );
  }
}
//=================================================================================================
void MainWindow::showSettings()
{
  if (!settings)
    settings = new SettingsDialog( this );

  if (settings->exec() == QDialog::Accepted)
  {
    Singleton<Settings>::Ptr().setValue( "Scene", "Rotation", settings->isRotationEnabled() ? "1" : "0" );
    Singleton<Settings>::Ptr().setValue( "Scene", "Time", QString::number( settings->getTimeToRotate() ) );
    Singleton<Settings>::Ptr().setValue( "Scene", "Speed", QString::number( settings->getRotateSpeed() ) );

    Singleton<Settings>::Ptr().setValue( "Style", "Current", settings->getStyle() );

    Singleton<Settings>::Ptr().setValue( "Proxy", "Enable", settings->isProxyEnabled() ? "1" : "0" );
    Singleton<Settings>::Ptr().setValue( "Proxy", "Host", settings->getProxyHost() );
    Singleton<Settings>::Ptr().setValue( "Proxy", "Port", QString::number( settings->getProxyPort() ) );

    Singleton<Settings>::Ptr().setKeyValueMap( "Keyboard", settings->getShortcuts() );

    /*if (!pipmakResource->compress( PathUtils::nativePath( qApp->applicationDirPath() + "/Pipmak Resources"),
                                   PathUtils::nativePath( qApp->applicationDirPath() + "/Data" ) ))
    {
      qDebug("Can't create Pipmak Resources file");
    }*/
  }
}
//=================================================================================================
void MainWindow::showAbout()
{
  About( this ).exec();
}
//=================================================================================================
void MainWindow::notify( const QString &nodeName, const QStringList &images, QSize maxSize )
{
  qDebug() << "==========================";
  qDebug() << nodeName;
  qDebug() << images;
  qDebug() << maxSize;
  qDebug() << "==========================";
}
//=================================================================================================
void MainWindow::playSound( const QString &name )
{
  QMdiArea *mdi = (QMdiArea *)projectTab->currentWidget();

  bool isOpen = false;

  for (int i = 0; i < mdi->subWindowList().count(); ++i)
  {
    AudioPlayer *player = dynamic_cast<AudioPlayer *>( mdi->subWindowList().at(i) );

    if (player)
    {
      mdi->setActiveSubWindow( player );
      player->addSound( name );
      isOpen = true;
      break;
    }
  }

  if (!isOpen)
  {
    AudioPlayer *player = new AudioPlayer( mdi );
    player->addSound( name );
    mdi->addSubWindow( player );
    player->show();
  }
}
//=================================================================================================
void MainWindow::checkUpdates()
{
  QProcess *p = new QProcess( this );
  p->setWorkingDirectory( qApp->applicationDirPath() );
  p->setEnvironment( QProcess::systemEnvironment() );

  QStringList params;
  params << "-jar" << PathUtils::nativePath( qApp->applicationDirPath() + "/PipmakAssistantUpdater.jar" );
  params << "-cv" << APP_VERSION;

  if (Singleton<Settings>::Ptr().getValue( "Proxy", "Enable" ).toInt())
  {
    params << "-ph" << Singleton<Settings>::Ptr().getValue( "Proxy", "Host" );
    params << "-pp" << Singleton<Settings>::Ptr().getValue( "Proxy", "Port" );
  }

  p->startDetached( "java", params );
  QTimer::singleShot( 1000, qApp, SLOT(closeAllWindows()) );
}
//=================================================================================================
