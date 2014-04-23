#include <QApplication>
#include <QStyleFactory>
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QDebug>

#include "PathUtils.h"
#include "Singleton.h"
#include "MainWindow.h"

//=================================================================================================
/*static inline bool NetworkEnable()
{
  bool validip = false;
  QList<QHostAddress> addrlist = QNetworkInterface::allAddresses();
  int o = -1;

  foreach (QHostAddress addr, addrlist)
  {
    o++;
    QNetworkInterface hop = QNetworkInterface::interfaceFromIndex(o);
    //QString mach = hop.hardwareAddress();
    //QString name = hop.name();
    QString oneip = addr.toString();

    if (!oneip.startsWith("127."))
    {
      bool actual = hop.isValid();

      if (actual)
        validip = true;
    }
  }

  return validip;
}*/
//=================================================================================================
int main( int argc, char *argv[] )
{
  QApplication::setDesktopSettingsAware( false );
  QApplication a( argc, argv );

  Singleton<Settings>::Ptr().setFile( qApp->applicationDirPath() + "/config.cfg" );

  if (Singleton<Settings>::Ptr().getValue( "Scene", "Rotation" ).isEmpty())
  {
    Singleton<Settings>::Ptr().setValue( "Scene", "Rotation", "1" );
    Singleton<Settings>::Ptr().setValue( "Scene", "Time", "10" );
    Singleton<Settings>::Ptr().setValue( "Scene", "Speed", "2" );
  }

  #ifdef Qt5
    if (Singleton<Settings>::Ptr().getValue( "Style", "Current" ).isEmpty())
      Singleton<Settings>::Ptr().setValue( "Style", "Current", "Fusion" );

    QApplication::setStyle( QStyleFactory::create("Fusion") );
  #else
    if (Singleton<Settings>::Ptr().getValue( "Style", "Current" ).isEmpty())
      Singleton<Settings>::Ptr().setValue( "Style", "Current", "Plastique" );

    QApplication::setStyle( QStyleFactory::create("Plastique") );
  #endif

  if (Singleton<Settings>::Ptr().getValue( "Proxy", "Enable" ).isEmpty())
  {
    Singleton<Settings>::Ptr().setValue( "Proxy", "Enable", "0" );
    Singleton<Settings>::Ptr().setValue( "Proxy", "Host", "" );
    Singleton<Settings>::Ptr().setValue( "Proxy", "Port", "0" );
  }

  if (Singleton<Settings>::Ptr().getKeyValueMap("Keyboard").isEmpty())
  {
    Singleton<Settings>::Ptr().setValue( "Keyboard", "NewProject", "Ctrl+N" );
    Singleton<Settings>::Ptr().setValue( "Keyboard", "OpenProjects", "Ctrl+O" );
    Singleton<Settings>::Ptr().setValue( "Keyboard", "RunProject", "Ctrl+R" );
    Singleton<Settings>::Ptr().setValue( "Keyboard", "ProjectLegend", "Ctrl+L" );
    Singleton<Settings>::Ptr().setValue( "Keyboard", "Settings", "F12" );
    Singleton<Settings>::Ptr().setValue( "Keyboard", "Update", "F5" );
  }

  QFile f(":res/style.css");
  f.open( QIODevice::ReadOnly );
  QTextStream ts( &f );
  //ts.setCodec( QTextCodec::codecForName("UTF-8") );
  ts.setAutoDetectUnicode( true );
  ts.setGenerateByteOrderMark( false );
  qApp->setStyleSheet( ts.readAll() );
  f.close();

  MainWindow w;
  w.setWindowIcon( QIcon(":res/project.png") );
  w.centerAndResize();
  w.show();
  w.readSettings();

  return a.exec();
}
//=================================================================================================
