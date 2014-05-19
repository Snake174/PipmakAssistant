#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//=================================================================================================
#include <QMainWindow>
#include <QProcess>
#include <QAction>
//=================================================================================================
QT_BEGIN_NAMESPACE
  class QTabWidget;
  class QTreeView;
  class ProjectList;
  class ProjectTree;
  class SettingsDialog;
  class PipmakResource;
  class FileDownloader;
QT_END_NAMESPACE
//=================================================================================================
class MainWindow : public QMainWindow
{
  Q_OBJECT

  QTabWidget *projectTab;
  ProjectList *projectList;
  ProjectTree *projectTree;
  SettingsDialog *settings;
  PipmakResource *pipmakResource;
  FileDownloader *updater;
  QList<QAction *> actions;

  public:
    MainWindow( QWidget *parent = 0 );
    ~MainWindow();

    void centerAndResize();
    void readSettings();

  protected:
    void showEvent( QShowEvent *e );

  private:
    void createMenus();
    void createDocks();
    void removeDir( const QString &dirName );

  private slots:
    void loadProject( const QString &path );
    void createProject();
    void closeProject( int index );
    void setCurrentProject( int index );
    void openProject();
    void runProject();
    void processStarted();
    void processFinished( int exitCode, QProcess::ExitStatus exitStatus );
    void processError( QProcess::ProcessError error );
    void editNode( const QString &nodeName );
    void showProjectLegend();
    void showCodeGenerator();
    void showSettings();
    void showAbout();
    void notify( const QString &nodeName, const QStringList &images, QSize maxSize );
    void playSound( const QString &name );
    void checkUpdates();
};
//=================================================================================================

#endif // MAINWINDOW_H
