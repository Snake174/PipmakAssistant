#ifndef OSDAB_ZIP_P__H
#define OSDAB_ZIP_P__H

#include "zip.h"
#include "zipentry_p.h"

#include <QtCore/QFileInfo>
#include <QtCore/QObject>
#include <QtCore/QtGlobal>

#include <zconf.h>

/*!
  zLib authors suggest using larger buffers (128K or 256K) for (de)compression (especially for inflate())
  we use a 256K buffer here - if you want to use this code on a pre-iceage mainframe please change it ;)
*/
#define ZIP_READ_BUFFER (256*1024)

namespace Zip {

  class ZipPrivate : public QObject
  {
      Q_OBJECT

    public:
      // uLongf from zconf.h
      typedef uLongf crc_t;

      ZipPrivate();
      virtual ~ZipPrivate();

      QMap<QString,ZipEntryP*>* headers;

      QIODevice* device;
      QFile* file;

      char buffer1[ZIP_READ_BUFFER];
      char buffer2[ZIP_READ_BUFFER];

      unsigned char* uBuffer;

      const crc_t* crcTable;

      QString comment;
      QString password;

      Zip::ErrorCode createArchive(QIODevice* device);
      Zip::ErrorCode closeArchive();
      void reset();

      bool zLibInit();

      bool containsEntry(const QFileInfo& info) const;

      Zip::ErrorCode addDirectory(const QString& path, const QString& root,
                                  Zip::CompressionOptions options, Zip::CompressionLevel level,
                                  int hierarchyLevel, int* addedFiles = 0);
      Zip::ErrorCode addFiles(const QStringList& paths, const QString& root,
                              Zip::CompressionOptions options, Zip::CompressionLevel level,
                              int* addedFiles);

      Zip::ErrorCode createEntry(const QFileInfo& file, const QString& root,
                                 Zip::CompressionLevel level);
      Zip::CompressionLevel detectCompressionByMime(const QString& ext);

      inline quint32 updateChecksum(const quint32& crc, const quint32& val) const;

      inline void encryptBytes(quint32* keys, char* buffer, qint64 read);

      inline void setULong(quint32 v, char* buffer, unsigned int offset);
      inline void updateKeys(quint32* keys, int c) const;
      inline void initKeys(quint32* keys) const;
      inline int decryptByte(quint32 key2) const;

      inline QString extractRoot(const QString& p, Zip::CompressionOptions o);

    private slots:
      void deviceDestroyed(QObject*);

    private:
      int compressionStrategy(const QString& path, QIODevice& file) const;
      Zip::ErrorCode deflateFile(const QFileInfo& fileInfo,
                                 quint32& crc, qint64& written, const Zip::CompressionLevel& level, quint32** keys);
      Zip::ErrorCode storeFile(const QString& path, QIODevice& file,
                               quint32& crc, qint64& written, quint32** keys);
      Zip::ErrorCode compressFile(const QString& path, QIODevice& file,
                                  quint32& crc, qint64& written, const Zip::CompressionLevel& level, quint32** keys);
      Zip::ErrorCode do_closeArchive();
      Zip::ErrorCode writeEntry(const QString& fileName, const ZipEntryP* h, quint32& szCentralDir);
      Zip::ErrorCode writeCentralDir(quint32 offCentralDir, quint32 szCentralDir);
  };

}

#endif // OSDAB_ZIP_P__H
