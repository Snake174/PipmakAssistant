#ifndef PIPMAKRESOURCE_H
#define PIPMAKRESOURCE_H

//=================================================================================================
#include <QObject>
//=================================================================================================
class PipmakResource : public QObject
{
  Q_OBJECT

  public:
    explicit PipmakResource( QObject *parent = 0 );
    ~PipmakResource();

  //private:
    bool decompress( const QString &file, const QString &out );
    bool compress( const QString &zip, const QString &dir );
};
//=================================================================================================

#endif // PIPMAKRESOURCE_H
