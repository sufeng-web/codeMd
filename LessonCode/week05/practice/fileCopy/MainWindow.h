#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QLabel>
#include <QThread>
#include "FileCopier.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onBrowseSource();
    void onBrowseTarget();
    void onStartCopy();
    void onCopyFinished(bool success, QString msg);

private:
    QLineEdit *srcEdit, *destEdit;
    QProgressBar *progressBar;
    QLabel *statusLabel;
    QPushButton *startBtn;
    
    QThread *workerThread; // 后台线程
};

#endif