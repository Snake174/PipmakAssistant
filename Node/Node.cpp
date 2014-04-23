#include <QFrame>
#include <QPushButton>
#include <QSignalMapper>
#include <QStackedWidget>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QToolButton>

#include <qmath.h>

#include "../PathUtils.h"
#include "../LuaParser/LuaParser.h"
#include "HotSpotEditor.h"
#include "PatchEditor.h"
#include "../ScriptEditor/ScriptEditor.h"
#include "SceneEditor.h"
#include "Node.h"

//=================================================================================================
Node::Node( const QString &nodeName, QWidget *parent ):
  QMdiSubWindow( parent ),
  mdi( (QMdiArea *)parent ),
  nodeName( nodeName ),
  stack(0),
  buttonsLayout(0),
  lua(0)
{
  lua = new LuaParser( nodeName + "/node.lua", this );

  QTabWidget *frame = new QTabWidget( this );

  QToolButton *tbSave = new QToolButton( frame );
  tbSave->setIconSize( QSize( 16, 16 ) );
  tbSave->setIcon( QIcon(":res/save.png") );
  tbSave->setToolTip( tr("Save Location") );

  frame->setCornerWidget( tbSave, Qt::TopLeftCorner );

  frame->addTab( new SceneEditor( this, frame ), QIcon(":res/scene.png"), tr("Scene") );
  frame->addTab( new HotSpotEditor( this, frame ), QIcon(":res/hotspots.png"), tr("Hotspots") );
  frame->addTab( new PatchEditor( this, frame ), QIcon(":res/patch.png"), tr("Patches") );
  frame->addTab( new ScriptEditor( this, frame ), QIcon(":res/script.png"), tr("Description") );

  setWidget( frame );
  setWindowFlags( Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint | Qt::WindowTitleHint );
  setMinimumSize( 640, 480 );
  setWindowIcon( QIcon(":res/location.png") );
  setWindowTitle( tr("Location") + " : " + PathUtils::getName( nodeName ) );
}
//=================================================================================================
Node::~Node()
{
}
//=================================================================================================
QString Node::getNodeName() const
{
  return PathUtils::nativePath( nodeName );
}
//=================================================================================================
LuaParser *Node::getLua()
{
  return lua;
}
//=================================================================================================
QImage *Node::getEquirectImage()
{
  QImage *faces[6];
  int w = 360;
  int h = 180;

  QStringList cubeMapImages;
  cubeMapImages << lua->getCubeMap()->fields["front"].toString()
                << lua->getCubeMap()->fields["right"].toString()
                << lua->getCubeMap()->fields["back"].toString()
                << lua->getCubeMap()->fields["left"].toString()
                << lua->getCubeMap()->fields["top"].toString()
                << lua->getCubeMap()->fields["bottom"].toString();

  for (int i = 0; i < cubeMapImages.count(); ++i)
  {
    QImage image;
    QString s = QString("%1/%2").arg( nodeName ).arg( cubeMapImages.at(i) );

    if (!image.load( s, PathUtils::getExt(s).toUpper().toLatin1().constData() ))
    {
      qDebug("Can't load image");
      return 0;
    }

    faces[i] = new QImage( image.convertToFormat( QImage::Format_RGB888 ) );
  }

  QImage *equirect = new QImage( w, h, QImage::Format_RGB888 );

  for (int i = 0; i < w; ++i)
  {
    for (int j = 0; j < h; ++j)
    {
      float az = (i + 0.5f) / w * 2 * (float)M_PI;
      float el = ((-0.5f - j) / h + 0.5f) * (float)M_PI;
      int f = 0;

      while (az > M_PI / 4)
      {
        az -= M_PI / 2;
        f++;
      }

      if (f == 4)
        f = 0;

      float x = tanf( az );
      float y = -sqrtf( 1 + x * x ) * tanf( el );

      if (y < -1)
      {
        f = 4;
        az = (i + 0.5f) / w * 360 * (float)M_PI / 180;
        x = sinf( az ) / tanf( el );
        y = cosf( az ) / tanf( el );
      }
      else if (y > 1)
      {
        f = 5;
        az = (i + 0.5f) / w * 360 * (float)M_PI / 180;
        x = -sinf( az ) / tanf( el );
        y = cosf( az ) / tanf( el );
      }

      int k = (int)floorf( (faces[f]->width() - 1) * (x + 1) / 2 + 0.5f );
      int l = (int)floorf( (faces[f]->height() - 1) * (y + 1) / 2 + 0.5f );

      ((unsigned char *)(equirect->bits()))[ j * equirect->bytesPerLine() + 3 * i + 0 ] = ((unsigned char *)(faces[f]->bits()))[ l * faces[f]->bytesPerLine() + 3 * k + 0 ];
      ((unsigned char *)(equirect->bits()))[ j * equirect->bytesPerLine() + 3 * i + 1 ] = ((unsigned char *)(faces[f]->bits()))[ l * faces[f]->bytesPerLine() + 3 * k + 1 ];
      ((unsigned char *)(equirect->bits()))[ j * equirect->bytesPerLine() + 3 * i + 2 ] = ((unsigned char *)(faces[f]->bits()))[ l * faces[f]->bytesPerLine() + 3 * k + 2 ];
    }
  }

  for (int i = 0; i < cubeMapImages.count(); ++i)
  {
    delete faces[i];
    faces[i] = 0;
  }

  return equirect;
}
//=================================================================================================
QImage *Node::getHotspotMapImage()
{
  if (lua->getHotSpotMap().fileName.isEmpty())
    return 0;

  QImage *img = new QImage( QImage( PathUtils::nativePath( nodeName + "/" + lua->getHotSpotMap().fileName ) ).convertToFormat( QImage::Format_ARGB32 ) );
  unsigned int *bits = (unsigned int *)img->bits();
  int imgSize = img->width() * img->height();

  for (int i = 0; i < imgSize; ++i)
  {
    if ((qRed(bits[i]) == 255) && (qGreen(bits[i]) == 255) && (qBlue(bits[i]) == 255))
      bits[i] = qRgba( 0, 0, 0, 0 );
  }

  return img;
}
//=================================================================================================
void Node::closeEvent( QCloseEvent *e )
{
  mdi->removeSubWindow( this );
  e->accept();
}
//=================================================================================================
