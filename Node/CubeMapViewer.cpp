#include <QMouseEvent>
#include <QImageReader>
#include <QFileDialog>
#include <qmath.h>

#include "../Structures.h"
#include "../PathUtils.h"
#include "../LuaParser/LuaParser.h"
#include "Node.h"
#include "CubeMapViewer.h"

//=================================================================================================
CubeMapViewer::CubeMapViewer( Node *node, QWidget *parent ):
  QGLWidget( setupGLFormat(), parent ),
  node( node ),
  xRot(0),
  yRot(0),
  currentFace(-1)
{
  setMouseTracking( true );
  setFocusPolicy( Qt::StrongFocus );
  setMinimumSize( 250, 250 );
}
//=================================================================================================
CubeMapViewer::~CubeMapViewer()
{
  makeCurrent();
  glDeleteTextures( 6, SkyboxTexture );
  glDeleteTextures( 1, &ramka );
}
//=================================================================================================
void CubeMapViewer::initializeGL()
{
  glShadeModel( GL_SMOOTH );
  glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
  glClearDepth( 1.0f );
  glEnable( GL_DEPTH_TEST );
  glDepthFunc( GL_LEQUAL );
  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
  glEnable( GL_TEXTURE_2D );

  if (!node->getLua()->getCubeMap()->fields["front"].toString().isEmpty())
  {
    loadTexture( PathUtils::nativePath( node->getNodeName() + "/" + node->getLua()->getCubeMap()->fields["front"].toString() ), SkyboxTexture[ CubeMap::FRONT ] );
    loadTexture( PathUtils::nativePath( node->getNodeName() + "/" + node->getLua()->getCubeMap()->fields["right"].toString() ), SkyboxTexture[ CubeMap::RIGHT ] );
    loadTexture( PathUtils::nativePath( node->getNodeName() + "/" + node->getLua()->getCubeMap()->fields["back"].toString() ), SkyboxTexture[ CubeMap::BACK ] );
    loadTexture( PathUtils::nativePath( node->getNodeName() + "/" + node->getLua()->getCubeMap()->fields["left"].toString() ), SkyboxTexture[ CubeMap::LEFT ] );
    loadTexture( PathUtils::nativePath( node->getNodeName() + "/" + node->getLua()->getCubeMap()->fields["top"].toString() ), SkyboxTexture[ CubeMap::TOP ] );
    loadTexture( PathUtils::nativePath( node->getNodeName() + "/" + node->getLua()->getCubeMap()->fields["bottom"].toString() ), SkyboxTexture[ CubeMap::BOTTOM ] );
  }
  else
  {
    loadTexture( ":res/front.png", SkyboxTexture[ CubeMap::FRONT ] );
    loadTexture( ":res/right.png", SkyboxTexture[ CubeMap::RIGHT ] );
    loadTexture( ":res/back.png", SkyboxTexture[ CubeMap::BACK ] );
    loadTexture( ":res/left.png", SkyboxTexture[ CubeMap::LEFT ] );
    loadTexture( ":res/top.png", SkyboxTexture[ CubeMap::TOP ] );
    loadTexture( ":res/bottom.png", SkyboxTexture[ CubeMap::BOTTOM ] );
  }

  loadPNGTexture( ":res/ramka.png", ramka );

  QImageReader reader;

  for (int i = 0; i < reader.supportedImageFormats().count(); ++i)
    formats << QString("*.%1").arg( reader.supportedImageFormats().at(i).data() );

  menu = new QMenu( this );

  aSelectFaceImage = new QAction( QIcon(":res/image.png"), "", this );
  connect( aSelectFaceImage, SIGNAL(triggered()), this, SLOT(selectFaceImage()) );

  menu->addAction( aSelectFaceImage );
}
//=================================================================================================
void CubeMapViewer::paintGL()
{
  renderScene();
}
//=================================================================================================
void CubeMapViewer::resizeGL( int w, int h )
{
  if (!h)
    h = 1;

  glViewport( 0, 0, w, h );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  glPerspective( 90.0f, (GLfloat)w / (GLfloat)h, 0.1f, 1000.0f );

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
}
//=================================================================================================
void CubeMapViewer::mousePressEvent( QMouseEvent *e )
{
  QGLWidget::mousePressEvent(e);

  if (e->isAccepted())
    return;

  if (e->button() == Qt::LeftButton)
  {
    ptrMousePosition = e->pos();
  }
  else if (e->button() == Qt::RightButton)
  {
    int face = faceAtPosition( e->pos() );

    if (face < 0)
      return;

    QString caption = "";

    if (face == CubeMap::FRONT)
      caption = "FRONT";
    else if (face == CubeMap::RIGHT)
      caption = "RIGHT";
    else if (face == CubeMap::BACK)
      caption = "BACK";
    else if (face == CubeMap::LEFT)
      caption = "LEFT";
    else if (face == CubeMap::TOP)
      caption = "TOP";
    else if (face == CubeMap::BOTTOM)
      caption = "BOTTOM";

    aSelectFaceImage->setText( QString( tr("Select %1 image") ).arg( caption ) );
    menu->exec( mapToGlobal( e->pos() ) );
  }

  updateGL();
}
//=================================================================================================
void CubeMapViewer::mouseReleaseEvent( QMouseEvent *e )
{
  QGLWidget::mouseReleaseEvent(e);

  if (e->isAccepted())
    return;

  if (e->button() == Qt::LeftButton)
    ptrMousePosition = e->pos();

  updateGL();
}
//=================================================================================================
void CubeMapViewer::mouseMoveEvent( QMouseEvent *e )
{
  QGLWidget::mouseMoveEvent(e);

  if (e->isAccepted())
    return;

  if (e->buttons() & Qt::LeftButton)
  {
    int dx = e->pos().x() - ptrMousePosition.x();
    int dy = e->pos().y() - ptrMousePosition.y();

    setXRotation( xRot + 8 * dy * 0.2 );
    setYRotation( yRot + 8 * dx * 0.2 );
  }

  ptrMousePosition = e->pos();
  currentFace = faceAtPosition( e->pos() );

  updateGL();
}
//=================================================================================================
QGLFormat CubeMapViewer::setupGLFormat()
{
  QGLFormat f = QGLFormat::defaultFormat();
  f.setSwapInterval(0);
  f.setDoubleBuffer( true );
  f.setSampleBuffers( true );
  f.setSamples(4);
  f.setVersion( 1, 5 );
  f.setProfile( QGLFormat::CompatibilityProfile );

  return f;
}
//=================================================================================================
void CubeMapViewer::glPerspective( double fovy, double aspect, double zNear, double zFar )
{
  GLdouble xmin, xmax, ymin, ymax;

  ymax = zNear * tan( fovy * M_PI / 360.0 );
  ymin = -ymax;

  xmin = ymin * aspect;
  xmax = ymax * aspect;

  glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}
//=================================================================================================
void CubeMapViewer::glPickMatrix( double x, double y, double width, double height, int viewport[] )
{
  GLfloat m[ 16 ];
  GLfloat sx, sy;
  GLfloat tx, ty;

  sx = viewport[2] / width;
  sy = viewport[3] / height;
  tx = (viewport[2] + 2.0 * (viewport[0] - x)) / width;
  ty = (viewport[3] + 2.0 * (viewport[1] - y)) / height;

  #define M( row, col ) m[ col * 4 + row ]
  M( 0, 0 ) = sx;
  M( 0, 1 ) = 0.0;
  M( 0, 2 ) = 0.0;
  M( 0, 3 ) = tx;
  M( 1, 0 ) = 0.0;
  M( 1, 1 ) = sy;
  M( 1, 2 ) = 0.0;
  M( 1, 3 ) = ty;
  M( 2, 0 ) = 0.0;
  M( 2, 1 ) = 0.0;
  M( 2, 2 ) = 1.0;
  M( 2, 3 ) = 0.0;
  M( 3, 0 ) = 0.0;
  M( 3, 1 ) = 0.0;
  M( 3, 2 ) = 0.0;
  M( 3, 3 ) = 1.0;
  #undef M

  glMultMatrixf(m);
}
//=================================================================================================
void CubeMapViewer::loadTexture( const QString &fileName, unsigned int &id )
{
  QImage image;
  image.load( fileName, PathUtils::getExt( fileName ).toUpper().toLatin1().data() );
  image = convertToGLFormat( image );

  glGenTextures( 1, &id );
  glBindTexture( GL_TEXTURE_2D, id );
  glTexImage2D( GL_TEXTURE_2D, 0, 3, (GLsizei)image.width(), (GLsizei)image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits() );
  // задаём линейную фильтрацию вблизи
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  // задаём линейную фильтрацию вдали
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  // при фильтрации игнорируются тексели, выходящие за границу текстуры для s координаты
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F );
  // при фильтрации игнорируются тексели, выходящие за границу текстуры для t координаты
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F );
  // цвет текселя полностью замещает цвет фрагмента фигуры
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
}
//=================================================================================================
void CubeMapViewer::loadPNGTexture( const QString &fileName, unsigned int &id )
{
  QImage image = QImage( fileName );

  glGenTextures( 1, &id );
  glBindTexture( GL_TEXTURE_2D, id );
  GLuint *pTexData = new GLuint[ image.width() * image.height() ];
  GLuint *sdata = (GLuint *)image.bits();
  GLuint *tdata = pTexData;

  for (int y = 0; y < image.height(); ++y)
  {
    for (int x = 0; x < image.width(); ++x)
    {
      *tdata = ((*sdata & 255) << 16) | (((*sdata >> 8) & 255) << 8)
          | (((*sdata >> 16) & 255) << 0) | (((*sdata >> 24) & 255) << 24);
      ++sdata;
      ++tdata;
    }
  }

  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pTexData );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F );
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

  delete[] pTexData;
  pTexData = 0;
}
//=================================================================================================
void CubeMapViewer::reloadTexture( const QString &fileName, int index )
{
  makeCurrent();
  glDeleteTextures( 1, &SkyboxTexture[ index ] );
  loadTexture( fileName, SkyboxTexture[ index ] );
  updateGL();
}
//=================================================================================================
int CubeMapViewer::faceAtPosition( const QPoint &pos )
{
  int objectsFound = 0;         // Общее количество кликнутых обьектов
  int viewportCoords[4] = {0};  // Массив для хранения экранных координат

  // Переменная для хранения ID обьектов, на которые мы кликнули.
  // Мы делаем массив в 32 элемента, т.к. OpenGL также сохраняет другую
  // информацию, которая нам сейчас не нужна. Для каждого обьекта нужно
  // 4 слота.
  unsigned int selectBuffer[ 32 ] = {0};

  // glSelectBuffer регистрирует массив как буфер выбора обьектов. Первый параметр - размер
  // массива. Второй - сам массив для хранения информации.
  glSelectBuffer( 32, selectBuffer );   // Регистрируем буфер для хранения выбранных обьектов

  // Эта функция возвращает информацию о многих вещах в OpenGL. Мы передаём GL_VIEWPOR,
  // чтобы получить координаты экрана. Функция сохранит их в переданном вторым параметром массиве
  // в виде top, left, bottom, right.
  glGetIntegerv( GL_VIEWPORT, viewportCoords ); // Получаем текущие координаты экрана

  // Теперь выходим из матрицы GL_MODELVIEW и переходим в матрицу GL_PROJECTION.
  // Это даёт возможность использовать X и Y координаты вместо 3D.
  glMatrixMode( GL_PROJECTION );  // Переходим в матрицу проекции
  glPushMatrix();                 // Переходим в новые экранные координаты

  // Эта функция делает так, что фреймбуфер не изменяется при рендере в него, вместо этого
  // происходит запись имён (ID) примитивов, которые были бы отрисованы при режиме
  // GL_RENDER. Информация помещается в selectBuffer.
  glRenderMode( GL_SELECT );  // Позволяет рендерить обьекты без изменения фреймбуфера
  glLoadIdentity();           // Сбросим матрицу проекции

  // gluPickMatrix позволяет создавать матрицу проекции около нашего курсора. Проще говоря,
  // рендерится только область, которую мы укажем (вокруг курсора). Если обьект рендерится
  // в этой области, его ID сохраняется (Вот он, смысл всей функции).
  // Первые 2 параметра - X и Y координаты начала, следующие 2 - ширина и высота области
  // отрисовки. Последний параметр - экранные координаты. Заметьте, мы вычитаем 'y' из
  // НИЖНЕЙ экранной координаты. Мы сделали это, чтобы перевернуть Y координаты.
  // В 3д-пространстве нулевые y-координаты начинаются внизу, а в экранных координатах
  // 0 по y находится вверху. Также передаём регион 2 на 2 пиксела для поиска в нём обьекта.
  // Это может быть изменено как вам удобнее.
  glPickMatrix( pos.x(), viewportCoords[3] - pos.y(), 2, 2, viewportCoords );

  // Далее просто вызываем нашу нормальную функцию gluPerspective, точно так же, как
  // делали при инициализации.
  glPerspective( 90.0f, (GLfloat)width() / (GLfloat)height(), 0.1f, 1000.0f );

  glMatrixMode( GL_MODELVIEW ); // Возвращаемся в матрицу GL_MODELVIEW

  renderScene();          // Теперь рендерим выбранную зону для выбора обьекта

  // Если мы вернёмся в нормальный режим рендеринга из режима выбора, glRenderMode
  // возвратит число обьектов, найденных в указанном регионе (в gluPickMatrix()).
  objectsFound = glRenderMode( GL_RENDER ); // Вернемся в режим отрисовки и получим число обьектов

  glMatrixMode( GL_PROJECTION );    // Вернемся в привычную матрицу проекции
  glPopMatrix();                    // Выходим из матрицы

  glMatrixMode( GL_MODELVIEW );     // Вернемся в матрицу GL_MODELVIEW

  // УФФ! Это было немного сложно. Теперь нам нужно выяснить ID выбранных обьектов.
  // Если они есть - objectsFound должно быть как минимум 1.
  if (objectsFound > 0)
  {
    // Если мы нашли более 1 обьекта, нужно проверить значения глубины всех
    // выбоанных обьектов. Обьект с МЕНЬШИМ значением глубины - ближайший
    // к нам обьект, значит и щелкнули мы на него. В зависимости от того, что
    // мы программируем, нам могут понадобится и ВСЕ выбранные обьекты (если
    // некоторые были за ближайшим), но в этом уроке мы позаботимся только о
    // переднем обьекте. Итак, как нам получить значение глубины? Оно сохранено
    // в буфере выбора (selectionBuffer). Для каждого обьекта в нем 4 значения.
    // Первое - "число имен в массиве имен на момент события, далее минимум и
    // максимум значений глубины для всех вершин, которые были выбраны при прошлом
    // событии, далее по содержимое массива имен, нижнее имя - первое;
    // ("the number of names in the name stack at the time of the event, followed
    // by the minimum and maximum depth values of all vertices that hit since the
    // previous event, then followed by the name stack contents, bottom name first.") - MSDN.
    // Единстве, что нам нужно - минимальное значение глубины (второе значение) и
    // ID обьекта, переданного в glLoadName() (четвертое значение).
    // Итак, [0-3] - данные первого обьекта, [4-7] - второго, и т.д...
    // Будте осторожны, так как если вы отображаете на экране 2Д текст, он будет
    // всегда находится как ближайший обьект. Так что убедитесь, что отключили вывод
    // текста при рендеринге в режиме GL_SELECT. Я для этого использую флаг, передаваемый
    // в RenderScene(). Итак, получим обьект с минимальной глубиной!

    // При старте установим ближайшую глубину как глубину первого обьекта.
    // 1 - это минимальное Z-значение первого обьекта.
    unsigned int lowestDepth = selectBuffer[1];

    // Установим выбранный обьект как первый при старте.
    // 3 - ID первого обьекта, переданный в glLoadName().
    int selectedObject = selectBuffer[3];

    // Проходим через все найденные обьекты, начиная со второго (значения первого
    // мы присвоили изначально).
    for (int i = 1; i < objectsFound; ++i)
    {
      // Проверяем, не ниже ли значение глубины текущего обьекта, чем предидущего.
      // Заметьте, мы умножаем i на 4 (4 значения на каждый обьект) и прибавляем 1 для глубины.
      if (selectBuffer[ (i * 4) + 1 ] < lowestDepth)
      {
        // Установим новое низшее значение
        lowestDepth = selectBuffer[ (i * 4) + 1 ];

        // Установим текущий ID обьекта
        selectedObject = selectBuffer[ (i * 4) + 3 ];
      }
    }

    // Вернем выбранный обьект
    return selectedObject;
  }

  // Если не щелкнули ни на 1 обьект, вернём -1
  return -1;
}
//=================================================================================================
void CubeMapViewer::renderScene()
{
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glLoadIdentity();
  glRotatef( xRot, 1.0f, 0.0f, 0.0f );
  glRotatef( yRot, 0.0f, 1.0f, 0.0f );

  Draw_Skybox( 0, 0, 0, 256, 256, 256 );
}
//=================================================================================================
void CubeMapViewer::Draw_Skybox( float x, float y, float z, float width, float height, float length )
{
  x = x - width / 2;
  y = y - height / 2;
  z = z - length / 2;

  glInitNames();

  glPushName( CubeMap::FRONT );
  glBindTexture( GL_TEXTURE_2D, SkyboxTexture[ CubeMap::FRONT ] );
  glColor3f( 1.0f, 1.0f, 1.0f );
  glBegin( GL_QUADS );
    glTexCoord2f( 1.0f, 0.0f ); glVertex3f( x + width, y,	z );
    glTexCoord2f( 1.0f, 1.0f ); glVertex3f( x + width, y + height, z );
    glTexCoord2f( 0.0f, 1.0f ); glVertex3f( x, y + height, z );
    glTexCoord2f( 0.0f, 0.0f ); glVertex3f( x, y,	z );
  glEnd();
  glPopName();

  glPushName( CubeMap::RIGHT );
  glBindTexture( GL_TEXTURE_2D, SkyboxTexture[ CubeMap::RIGHT ] );
  glColor3f( 1.0f, 1.0f, 1.0f );
  glBegin( GL_QUADS );
    glTexCoord2f( 1.0f, 0.0f ); glVertex3f( x + width, y, z + length );
    glTexCoord2f( 1.0f, 1.0f ); glVertex3f( x + width, y + height, z + length );
    glTexCoord2f( 0.0f, 1.0f ); glVertex3f( x + width, y + height, z );
    glTexCoord2f( 0.0f, 0.0f ); glVertex3f( x + width, y, z );
  glEnd();
  glPopName();

  glPushName( CubeMap::BACK );
  glBindTexture( GL_TEXTURE_2D, SkyboxTexture[ CubeMap::BACK ] );
  glColor3f( 1.0f, 1.0f, 1.0f );
  glBegin( GL_QUADS );
    glTexCoord2f( 0.0f, 0.0f ); glVertex3f( x + width, y,	z + length );
    glTexCoord2f( 0.0f, 1.0f ); glVertex3f( x + width, y + height, z + length );
    glTexCoord2f( 1.0f, 1.0f ); glVertex3f( x, y + height, z + length );
    glTexCoord2f( 1.0f, 0.0f ); glVertex3f( x, y, z + length );
  glEnd();
  glPopName();

  glPushName( CubeMap::LEFT );
  glBindTexture( GL_TEXTURE_2D, SkyboxTexture[ CubeMap::LEFT ] );
  glColor3f( 1.0f, 1.0f, 1.0f );
  glBegin( GL_QUADS );
    glTexCoord2f( 1.0f, 0.0f ); glVertex3f( x, y, z );
    glTexCoord2f( 1.0f, 1.0f ); glVertex3f( x, y + height, z );
    glTexCoord2f( 0.0f, 1.0f ); glVertex3f( x, y + height, z + length );
    glTexCoord2f( 0.0f, 0.0f ); glVertex3f( x, y, z + length );
  glEnd();
  glPopName();

  glPushName( CubeMap::TOP );
  glBindTexture( GL_TEXTURE_2D, SkyboxTexture[ CubeMap::TOP ] );
  glColor3f( 1.0f, 1.0f, 1.0f );
  glBegin( GL_QUADS );
    glTexCoord2f( 1.0f, 0.0f ); glVertex3f( x + width, y + height, z );
    glTexCoord2f( 1.0f, 1.0f ); glVertex3f( x + width, y + height, z + length );
    glTexCoord2f( 0.0f, 1.0f ); glVertex3f( x, y + height, z + length );
    glTexCoord2f( 0.0f, 0.0f ); glVertex3f( x, y + height, z );
  glEnd();
  glPopName();

  glPushName( CubeMap::BOTTOM );
  glBindTexture( GL_TEXTURE_2D, SkyboxTexture[ CubeMap::BOTTOM ] );
  glColor3f( 1.0f, 1.0f, 1.0f );
  glBegin( GL_QUADS );
    glTexCoord2f( 1.0f, 0.0f ); glVertex3f( x + width, y, z + length );
    glTexCoord2f( 1.0f, 1.0f ); glVertex3f( x + width, y, z );
    glTexCoord2f( 0.0f, 1.0f ); glVertex3f( x, y, z );
    glTexCoord2f( 0.0f, 0.0f ); glVertex3f( x, y, z + length );
  glEnd();
  glPopName();


  glEnable( GL_BLEND );
  glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  if (currentFace == CubeMap::FRONT)
  {
    glBindTexture( GL_TEXTURE_2D, ramka );
    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    glBegin( GL_QUADS );
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f( x + width, y,	z );
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f( x + width, y + height, z );
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f( x, y + height, z );
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f( x, y,	z );
    glEnd();
  }
  else if (currentFace == CubeMap::RIGHT)
  {
    glBindTexture( GL_TEXTURE_2D, ramka );
    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    glBegin( GL_QUADS );
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f( x + width, y, z + length );
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f( x + width, y + height, z + length );
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f( x + width, y + height, z );
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f( x + width, y, z );
    glEnd();
  }
  else if (currentFace == CubeMap::BACK)
  {
    glBindTexture( GL_TEXTURE_2D, ramka );
    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    glBegin( GL_QUADS );
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f( x + width, y,	z + length );
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f( x + width, y + height, z + length );
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f( x, y + height, z + length );
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f( x, y, z + length );
    glEnd();
  }
  else if (currentFace == CubeMap::LEFT)
  {
    glBindTexture( GL_TEXTURE_2D, ramka );
    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    glBegin( GL_QUADS );
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f( x, y, z );
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f( x, y + height, z );
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f( x, y + height, z + length );
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f( x, y, z + length );
    glEnd();
  }
  else if (currentFace == CubeMap::TOP)
  {
    glBindTexture( GL_TEXTURE_2D, ramka );
    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    glBegin( GL_QUADS );
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f( x + width, y + height, z );
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f( x + width, y + height, z + length );
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f( x, y + height, z + length );
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f( x, y + height, z );
    glEnd();
  }
  else if (currentFace == CubeMap::BOTTOM)
  {
    glBindTexture( GL_TEXTURE_2D, ramka );
    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
    glBegin( GL_QUADS );
      glTexCoord2f( 1.0f, 0.0f ); glVertex3f( x + width, y, z + length );
      glTexCoord2f( 1.0f, 1.0f ); glVertex3f( x + width, y, z );
      glTexCoord2f( 0.0f, 1.0f ); glVertex3f( x, y, z );
      glTexCoord2f( 0.0f, 0.0f ); glVertex3f( x, y, z + length );
    glEnd();
  }

  glDisable( GL_BLEND );
}
//=================================================================================================
void CubeMapViewer::qNormalizeAngle( int &angle )
{
  while (angle < 0)
    angle += 360;

  while (angle > 360)
    angle -= 360;
}
//=================================================================================================
void CubeMapViewer::setXRotation( int angle )
{
  qNormalizeAngle( angle );

  if (angle != xRot)
    xRot = angle;
}
//=================================================================================================
void CubeMapViewer::setYRotation( int angle )
{
  qNormalizeAngle( angle );

  if (angle != yRot)
    yRot = angle;
}
//=================================================================================================
void CubeMapViewer::selectFaceImage()
{
  QAction *act = dynamic_cast<QAction *>( sender() );

  if (act)
  {
    QString caption = act->text();
    QString fileName = QFileDialog::getOpenFileName( this, caption, "", QString( tr("Image Files (%1)") ).arg( formats.join(" ") ) );

    if (!fileName.isEmpty())
    {
      if (caption.contains("FRONT"))
        reloadTexture( fileName, CubeMap::FRONT );
      else if (caption.contains("RIGHT"))
        reloadTexture( fileName, CubeMap::RIGHT );
      else if (caption.contains("BACK"))
        reloadTexture( fileName, CubeMap::BACK );
      else if (caption.contains("LEFT"))
        reloadTexture( fileName, CubeMap::LEFT );
      else if (caption.contains("TOP"))
        reloadTexture( fileName, CubeMap::TOP );
      else if (caption.contains("BOTTOM"))
        reloadTexture( fileName, CubeMap::BOTTOM );
    }
  }
}
//=================================================================================================
