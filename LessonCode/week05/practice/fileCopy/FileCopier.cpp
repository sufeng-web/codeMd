#include "FileCopier.h"
#include <QDirIterator>

void FileCopier::process(const QString &sourcePath, const QString &targetPath) 
{
    QDir sourceDir(sourcePath);
    QDir targetDir(targetPath);

    // 1. 扫描所有文件
    QStringList files;
    QDirIterator it(sourcePath, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) 
    {
        files << it.next();
    }

    int totalFiles = files.count();
    if (totalFiles == 0) {
        emit finished(true, "源目录为空");
        return;
    }

    // 2. 逐个复制
    for (int i = 0; i < totalFiles; ++i) {
        QString srcFile = files[i];
        
        // 计算相对路径，以便在目标目录建立相同的目录结构
        QString relativePath = sourceDir.relativeFilePath(srcFile);
        QString destFile = targetDir.absoluteFilePath(relativePath);

        // 确保目标子目录存在
        QFileInfo destInfo(destFile);
        QDir().mkpath(destInfo.absolutePath());

        emit statusMessage(QString("正在复制: %1").arg(destInfo.fileName()));

        // 执行复制（如果目标已存在则删除，或根据需要修改）
        if (QFile::exists(destFile)) {
            QFile::remove(destFile);
        }
        
        if (!QFile::copy(srcFile, destFile)) {
            emit finished(false, "无法复制文件: " + srcFile);
            return;
        }

        // 更新进度
        int progress = static_cast<int>((static_cast<double>(i + 1) / totalFiles) * 100);
        emit progressUpdated(progress);
    }

    emit finished(true, "全部文件复制完成！");
}