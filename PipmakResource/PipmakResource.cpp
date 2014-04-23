#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFileInfo>

#include "zip.h"
#include "unzip.h"
#include "PipmakResource.h"

//=================================================================================================
PipmakResource::PipmakResource( QObject *parent ):
  QObject( parent )
{
}
//=================================================================================================
PipmakResource::~PipmakResource()
{
}
//=================================================================================================
bool PipmakResource::decompress( const QString &file, const QString &out )
{
  if (!QFile::exists( file ))
  {
    qDebug() << "File does not exist";
    return false;
  }

  Zip::UnZip::ErrorCode ec;
  Zip::UnZip uz;

  ec = uz.openArchive( file );

  if (ec != Zip::UnZip::Ok)
  {
    qDebug() << "Failed to open archive: " << uz.formatError( ec );
    return false;
  }

  ec = uz.extractAll( out );

  if (ec != Zip::UnZip::Ok)
  {
    qDebug() << "Extraction failed: " << uz.formatError( ec );
    uz.closeArchive();
    return false;
  }

  return true;
}
//=================================================================================================
bool PipmakResource::compress( const QString &zip, const QString &dir )
{
  QFileInfo fi( dir );

  if (!fi.isDir())
  {
    qDebug() << "Directory does not exist.";
    return false;
  }

  Zip::Zip::ErrorCode ec;
  Zip::Zip uz;

  ec = uz.createArchive( zip );

  if (ec != Zip::Zip::Ok)
  {
    qDebug() << "Unable to create archive: " << uz.formatError( ec );
    return false;
  }

  ec = uz.addDirectory( dir, 0, Zip::Zip::IgnoreRoot, Zip::Zip::Store );

  if (ec != Zip::Zip::Ok)
    qDebug() << "Unable to add directory: " << uz.formatError( ec );

  if (uz.closeArchive() != Zip::Zip::Ok)
    qDebug() << "Unable to close the archive: " << uz.formatError( ec );

  return ec == Zip::Zip::Ok;
}
//=================================================================================================
