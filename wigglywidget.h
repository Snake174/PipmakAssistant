#ifndef WIGGLYWIDGET_H
#define WIGGLYWIDGET_H

#include <QBasicTimer>
#include <QWidget>

class WigglyWidget : public QWidget
{
  Q_OBJECT

  QBasicTimer timer;
  QString text;
  int step;

  public:
    WigglyWidget(QWidget *parent = 0);

    void setText( const QString &newText ) { text = newText; }

  protected:
    void paintEvent( QPaintEvent *event );
    void timerEvent( QTimerEvent *event );
};

#endif
