#ifndef OSDAB_ZIPGLOBAL__H
#define OSDAB_ZIPGLOBAL__H

#include <QtCore/QDateTime>
#include <QtCore/QtGlobal>

#ifndef OSDAB_NAMESPACE
#define OSDAB_ZIP_MANGLE(x) zip_##x
#else
#define OSDAB_ZIP_MANGLE(x) x
#endif

namespace Zip {

int OSDAB_ZIP_MANGLE(currentUtcOffset)();
QDateTime OSDAB_ZIP_MANGLE(fromFileTimestamp)(const QDateTime& dateTime);
bool OSDAB_ZIP_MANGLE(setFileTimestamp)(const QString& fileName, const QDateTime& dateTime);

}

#endif // OSDAB_ZIPGLOBAL__H
