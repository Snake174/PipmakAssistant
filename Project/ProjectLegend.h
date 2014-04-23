#ifndef PROJECTLEGEND_H
#define PROJECTLEGEND_H

//=================================================================================================
#include <QMdiSubWindow>
#include <QMdiArea>
//=================================================================================================
QT_BEGIN_NAMESPACE
  class LegendViewer;
  class LegendScene;
QT_END_NAMESPACE
//=================================================================================================
class ProjectLegend : public QMdiSubWindow
{
  Q_OBJECT

  QMdiArea *mdi;
  QString projectName;
  LegendScene *scene;

  public:
    explicit ProjectLegend( const QString &projectName, QWidget *parent = 0 );
    ~ProjectLegend();

    QString getProjectName() const;

  protected:
    void closeEvent( QCloseEvent *e );

  signals:
    void editNode( const QString & );
};
//=================================================================================================

#endif // PROJECTLEGEND_H
