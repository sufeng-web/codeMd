#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QPointer>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), workerThread(nullptr) {
    auto *central = new QWidget();
    auto *layout = new QVBoxLayout(central);

    // 路径选择 UI
    srcEdit = new QLineEdit();
    destEdit = new QLineEdit();
    auto *btnSrc = new QPushButton("选择源目录");
    auto *btnDest = new QPushButton("选择目标目录");

    auto *h1 = new QHBoxLayout(); h1->addWidget(srcEdit); h1->addWidget(btnSrc);
    auto *h2 = new QHBoxLayout(); h2->addWidget(destEdit); h2->addWidget(btnDest);

    progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    statusLabel = new QLabel("就绪");
    startBtn = new QPushButton("开始批量复制");

    layout->addLayout(h1);
    layout->addLayout(h2);
    layout->addWidget(progressBar);
    layout->addWidget(statusLabel);
    layout->addWidget(startBtn);

    setCentralWidget(central);
    setMinimumWidth(500);

    // 绑定信号
    connect(btnSrc, &QPushButton::clicked, this, &MainWindow::onBrowseSource);
    connect(btnDest, &QPushButton::clicked, this, &MainWindow::onBrowseTarget);
    connect(startBtn, &QPushButton::clicked, this, &MainWindow::onStartCopy);
}

MainWindow::~MainWindow() {
    if (workerThread) {
        workerThread->quit();
        workerThread->wait();
    }
}

void MainWindow::onBrowseSource() {
    srcEdit->setText(QFileDialog::getExistingDirectory(
        this,
        "选择源目录",
        "",
        QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog
    ));
}

void MainWindow::onBrowseTarget() {
    destEdit->setText(QFileDialog::getExistingDirectory(
        this, 
        "选择目标目录", 
        "", 
        QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog
    ));
}

void MainWindow::onStartCopy() {
    if (srcEdit->text().isEmpty() || destEdit->text().isEmpty()) return;

    startBtn->setEnabled(false);
    progressBar->setValue(0);

    workerThread = new QThread();
    FileCopier *worker = new FileCopier();
    worker->moveToThread(workerThread);

    // 1. 线程启动时，执行复制逻辑
    connect(workerThread, &QThread::started, [=](){
        worker->process(srcEdit->text(), destEdit->text());
    });

    // 2. 接收进度和状态
    connect(worker, &FileCopier::progressUpdated, progressBar, &QProgressBar::setValue);
    connect(worker, &FileCopier::statusMessage, statusLabel, &QLabel::setText);

    // 3. 任务结束处理
    connect(worker, &FileCopier::finished, this, &MainWindow::onCopyFinished);
    
    // 4. 清理内存：任务结束后销毁对象，线程退出
    connect(worker, &FileCopier::finished, worker, &QObject::deleteLater);
    connect(worker, &FileCopier::finished, workerThread, &QThread::quit);

    workerThread->start();
}

void MainWindow::onCopyFinished(bool success, QString msg) {
    startBtn->setEnabled(true);
    statusLabel->setText(msg);
    if (!success) QMessageBox::critical(this, "错误", msg);
}