#ifndef SCRIPTVIEWER_H
#define SCRIPTVIEWER_H

//=================================================================================================
#include <Qsci/qsciscintilla.h>
//=================================================================================================
class ScriptViewer : public QsciScintilla
{
  Q_OBJECT

  public:
    explicit ScriptViewer( QWidget *parent = 0 );
    ~ScriptViewer();

  protected:
    void keyPressEvent( QKeyEvent *e );
};
//=================================================================================================

#endif // SCRIPTVIEWER_H
