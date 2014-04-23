
#ifndef OSDAB_UNZIP_P__H
#define OSDAB_UNZIP_P__H

#include "unzip.h"
#include "zipentry_p.h"

#include <QtCore/QObject>
#include <QtCore/QtGlobal>

// zLib authors suggest using larger buffers (128K or 256K) for (de)compression (especially for inflate())
// we use a 256K buffer here - if you want to use this code on a pre-iceage mainframe please change it ;)
#define UNZIP_READ_BUFFER (256*1024)

namespace Zip {

  class UnzipPrivate : public QObject
  {
      Q_OBJECT

    public:
      UnzipPrivate();

      // Replace this with whatever else you use to store/retrieve the password.
      QString password;

      bool skipAllEncrypted;

      QMap<QString,ZipEntryP*>* headers;

      QIODevice* device;
      QFile* file;

      char buffer1[UNZIP_READ_BUFFER];
      char buffer2[UNZIP_READ_BUFFER];

      unsigned char* uBuffer;
      const quint32* crcTable;

      // Central Directory (CD) offset
      quint32 cdOffset;
      // End of Central Directory (EOCD) offset
      quint32 eocdOffset;

      // Number of entries in the Central Directory (as to the EOCD record)
      quint16 cdEntryCount;

      // The number of detected entries that have been skipped because of a non compatible format
      quint16 unsupportedEntryCount;

      QString comment;

      UnZip::ErrorCode openArchive(QIODevice* device);

      UnZip::ErrorCode seekToCentralDirectory();
      UnZip::ErrorCode parseCentralDirectoryRecord();
      UnZip::ErrorCode parseLocalHeaderRecord(const QString& path, const ZipEntryP& entry);

      void closeArchive();

      UnZip::ErrorCode extractFile(const QString& path, const ZipEntryP& entry, const QDir& dir, UnZip::ExtractionOptions options);
      UnZip::ErrorCode extractFile(const QString& path, const ZipEntryP& entry, QIODevice* device, UnZip::ExtractionOptions options);

      UnZip::ErrorCode testPassword(quint32* keys, const QString& file, const ZipEntryP& header);
      bool testKeys(const ZipEntryP& header, quint32* keys);

      bool createDirectory(const QString& path);

      inline void decryptBytes(quint32* keys, char* buffer, qint64 read);

      inline quint32 getULong(const unsigned char* data, quint32 offset) const;
      inline quint64 getULLong(const unsigned char* data, quint32 offset) const;
      inline quint16 getUShort(const unsigned char* data, quint32 offset) const;
      inline int decryptByte(quint32 key2) const;
      inline void updateKeys(quint32* keys, int c) const;
      inline void initKeys(const QString& pwd, quint32* keys) const;

      inline QDateTime convertDateTime(const unsigned char date[2], const unsigned char time[2]) const;

    private slots:
      void deviceDestroyed(QObject*);

    private:
      UnZip::ErrorCode extractStoredFile(const quint32 szComp, quint32** keys,
                                         quint32& myCRC, QIODevice* outDev, UnZip::ExtractionOptions options);
      UnZip::ErrorCode inflateFile(const quint32 szComp, quint32** keys,
                                   quint32& myCRC, QIODevice* outDev, UnZip::ExtractionOptions options);
      void do_closeArchive();
  };

}

#endif // OSDAB_UNZIP_P__H
