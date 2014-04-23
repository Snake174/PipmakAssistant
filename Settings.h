#ifndef SETTINGS_H
#define SETTINGS_H

//=================================================================================================
#include <QObject>
#include <QMap>
#include <QSettings>
#include <QTextCodec>
//=================================================================================================
class Set : public QSettings
{
  public:
    Set( const QString &fileName, Format format, QObject *parent = 0 ):
      QSettings( fileName, format, parent )
    {
      QTextCodec *codec = QTextCodec::codecForName("UTF-8");
      setIniCodec( codec );
    }
};
//=================================================================================================
QT_BEGIN_NAMESPACE
  class QSettings;
QT_END_NAMESPACE
//=================================================================================================
class Settings : public QObject
{
  Q_OBJECT

  Set *set;

  public:
    explicit Settings( QObject *parent = 0 );
    ~Settings();

    void setFile( const QString &fileName );
    QString getValue( const QString &section, const QString &name );
    void setValue( const QString &section, const QString &name, const QString &value );
    QMap<QString, QString> getKeyValueMap( const QString &section );
    void setKeyValueMap( const QString &section, const QMap<QString, QString> &kvm );
    void clearSection( const QString &section );
    void clear( const QString &section, const QString &key );
    bool keyExist( const QString &section, const QString &key );
};
//=================================================================================================

#endif // SETTINGS_H
