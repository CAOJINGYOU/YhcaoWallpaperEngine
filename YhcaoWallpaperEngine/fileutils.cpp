#include "fileutils.h"

CFileUtils::CFileUtils()
{

}

bool CFileUtils::ScanData(const QDir &fromDir, const QStringList &filters,QStringList &filePathList)
{
    QFileInfoList fileInfoList = fromDir.entryInfoList(filters, QDir::AllDirs|QDir::Files);
    foreach(QFileInfo fileInfo, fileInfoList)
    {
        if (fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;
        if (fileInfo.isDir())
        {
            if (!ScanData(fileInfo.filePath(), filters,filePathList))
                return false;
        }
        else
        {
            filePathList.append(fileInfo.absoluteFilePath());
        }
    }
    return true;
}
