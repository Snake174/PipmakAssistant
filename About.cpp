#include <QPlainTextEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextStream>
#include <QFile>
#include <QIcon>

#include "About.h"

//=================================================================================================
About::About( QWidget *parent ):
  QDialog( parent )
{
  QPlainTextEdit *te = new QPlainTextEdit( this );
  te->setReadOnly( true );

  QFile f(":res/about.txt");
  f.open( QIODevice::ReadOnly );
  QTextStream ts( &f );
  ts.setAutoDetectUnicode( true );
  ts.setGenerateByteOrderMark( false );
  te->appendHtml( QT_TRANSLATE_NOOP( "About", ts.readAll().replace("APP_VERSION", APP_VERSION).replace("QT_VERSION_STR", QT_VERSION_STR) ) );
  f.close();

  QVBoxLayout *vl = new QVBoxLayout( this );
  vl->addWidget( te );

  setWindowIcon( QIcon(":res/about.png") );
  setWindowTitle( tr("About Program") );
}
//=================================================================================================
