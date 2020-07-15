#ifndef FILEUTILS_H
#define FILEUTILS_H
#include <QDir>

class CFileUtils
{
public:
    CFileUtils();

    static bool ScanData(const QDir &fromDir, const QStringList &filters,QStringList &filePathList);
};

#endif // FILEUTILS_H
