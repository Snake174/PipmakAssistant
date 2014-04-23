#include <QStringList>

#include "Settings.h"

//=================================================================================================
Settings::Settings( QObject *parent ):
  QObject( parent )
{
}
//=================================================================================================
Settings::~Settings()
{
}
//=================================================================================================
void Settings::setFile( const QString &fileName )
{
  set = new Set( fileName, QSettings::IniFormat, this );
}
//=================================================================================================
QString Settings::getValue( const QString &section, const QString &name )
{
  QString res;

  set->beginGroup( section );
  res = set->value( name, "" ).toString().trimmed();
  set->endGroup();

  return res;
}
//=================================================================================================
void Settings::setValue( const QString &section, const QString &name, const QString &value )
{
  set->beginGroup( section );
  set->setValue( name, value );
  set->endGroup();
}
//=================================================================================================
QMap<QString, QString> Settings::getKeyValueMap( const QString &section )
{
  QMap<QString, QString> res;

  set->beginGroup( section );
  QStringList allKeys = set->allKeys();

  for (int i = 0; i < allKeys.count(); ++i)
    res.insert( allKeys.at(i), set->value( allKeys.at(i) ).toString() );

  set->endGroup();

  return res;
}
//=================================================================================================
void Settings::setKeyValueMap( const QString &section, const QMap<QString, QString> &kvm )
{
  set->beginGroup( section );

  for (QMap<QString, QString>::const_iterator it = kvm.constBegin(); it != kvm.constEnd(); ++it)
    set->setValue( it.key(), it.value() );

  set->endGroup();
}
//=================================================================================================
void Settings::clearSection( const QString &section )
{
  set->beginGroup( section );
  set->clear();
  set->endGroup();
}
//=================================================================================================
void Settings::clear( const QString &section, const QString &key )
{
  set->beginGroup( section );
  set->remove( key );
  set->endGroup();
}
//=================================================================================================
bool Settings::keyExist( const QString &section, const QString &key )
{
  set->beginGroup( section );

  if (set->allKeys().contains( key, Qt::CaseInsensitive ))
  {
    set->endGroup();
    return true;
  }

  set->endGroup();

  return false;
}
//=================================================================================================
