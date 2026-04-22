#ifndef FILECOPIER_H
#define FILECOPIER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QFileInfo>

class FileCopier : public QObject {
    Q_OBJECT
public:
    explicit FileCopier(QObject *parent = nullptr) : QObject(parent) {}

public slots:
    // 执行复制的核心槽函数
    void process(const QString &sourcePath, const QString &targetPath);

signals:
    void progressUpdated(int percent);       // 总体百分比
    void statusMessage(QString msg);         // 当前正在干什么
    void finished(bool success, QString msg);// 完成信号
};

#endif