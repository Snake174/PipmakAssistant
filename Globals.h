#ifndef GLOBALS_H
#define GLOBALS_H

//=================================================================================================
#include <QObject>
#include <QMap>

#include "MainWindow.h"
//=================================================================================================
class Globals : public QObject
{
  Q_OBJECT

  public:
    explicit Globals( QObject *parent = 0 );
    ~Globals();

    MainWindow *mainWindow;
};
//=================================================================================================

#endif // GLOBALS_H
