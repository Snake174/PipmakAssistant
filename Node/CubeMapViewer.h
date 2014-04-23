#ifndef CUBEMAPVIEWER_H
#define CUBEMAPVIEWER_H

//=================================================================================================
#include <QGLWidget>
#include <QMenu>
//=================================================================================================
QT_BEGIN_NAMESPACE
class Node;
QT_END_NAMESPACE
//=================================================================================================
class CubeMapViewer : public QGLWidget
{
  Q_OBJECT

  Node *node;
  unsigned int SkyboxTexture[6];
  unsigned int ramka;
  float xRot;
  float yRot;
  int currentFace;
  QPoint ptrMousePosition;
  QMenu *menu;
  QAction *aSelectFaceImage;
  QStringList formats;

  public:
    explicit CubeMapViewer( Node *node, QWidget *parent = 0 );
    ~CubeMapViewer();

  protected:
    void initializeGL();
    void paintGL();
    void resizeGL( int w, int h );
    void mousePressEvent( QMouseEvent *e );
    void mouseReleaseEvent( QMouseEvent *e );
    void mouseMoveEvent( QMouseEvent *e );

  private:
    QGLFormat setupGLFormat();
    void glPerspective( double fovy, double aspect, double zNear, double zFar );
    void glPickMatrix( double x, double y, double width, double height, int viewport[4] );
    void loadTexture( const QString &fileName, unsigned int &id );
    void loadPNGTexture( const QString &fileName, unsigned int &id );
    void reloadTexture( const QString &fileName, int index );
    int faceAtPosition( const QPoint &pos );
    void renderScene();
    void Draw_Skybox( float x, float y, float z, float width, float height, float length );
    void qNormalizeAngle( int &angle );
    void setXRotation( int angle );
    void setYRotation( int angle );

  private slots:
    void selectFaceImage();
};
//=================================================================================================

#endif // CUBEMAPVIEWER_H
