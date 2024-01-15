#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <iomanip>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->input->setUndoRedoEnabled(false);
    ui->output->setUndoRedoEnabled(false);

    ui->input->setWordWrapMode(QTextOption::WrapMode::NoWrap);
    ui->output->setWordWrapMode(QTextOption::WrapMode::NoWrap);

    connect(ui->openButton,    &QPushButton::clicked, this, &MainWindow::open);
    connect(ui->processButton, &QPushButton::clicked, this, &MainWindow::process);
    connect(ui->saveButton,    &QPushButton::clicked, this, &MainWindow::save);

    connect(this, &MainWindow::finished, this, &MainWindow::handle);

    ui->openButton->setEnabled(true);
    ui->processButton->setEnabled(true);

    ui->inputBufferInput->setRange(4 * 1024, 100 * 1024);
    ui->inputBufferInput->setValue(20 * 1024);
    ui->inputBufferInput->setSingleStep(1024);

    ui->outputBufferInput->setRange(1024, 100 * 1024);
    ui->outputBufferInput->setValue(10 * 1024);
    ui->outputBufferInput->setSingleStep(1024);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::log(const std::string &message)
{
    std::time_t now = std::time(0);
    std::tm *time = std::localtime(&now);

    try {
        std::ofstream logFile("Log.txt", std::ios::app);
        logFile << "[" << std::put_time(time, "%H:%M:%S %d-%m-%Y") << "] " << message << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error logging: " << e.what() << std::endl;
    }
}

void MainWindow::processBuffer(const std::vector<char> &buffer, std::streamsize size)
{
    std::string chunk(buffer.data(), size);
    ui->input->insertPlainText(QString::fromStdString(chunk));
}

std::vector<WordCounter> MainWindow::countWords()
{
    std::map<std::string, int> wordCount;
    std::vector<WordCounter> result;

    try {
        std::stringstream stream(ui->input->toPlainText().toStdString());
        std::string line;

        while (std::getline(stream, line)) {
            std::string word;

            std::replace_if(line.begin(), line.end(), [](char c) {
                return static_cast<unsigned char>(c) > 127;
            }, ' ');

            std::stringstream ss(line);
            while (ss >> word) {
                wordCount[word]++;
            }
        }
    } catch (const std::exception &e) {
        log("Error counting words: " + std::string(e.what()));
    }

    for (const auto &entry : wordCount) {
        result.emplace_back(entry.first, entry.second);
    }

    return result;
}

void MainWindow::handle()
{
    ui->output->clear();

    for (const auto &wc : m_result) {
        ui->output->appendPlainText(
            QStringLiteral("%1,%2").arg(QString::fromUtf8(wc.word.c_str())).arg(wc.count)
        );
    }

    log("Text processing is done!");
    ui->statusBar->showMessage("Done!", 2000);
}

void MainWindow::open()
{
    m_filepath = QFileDialog::getOpenFileName(
        this,
        tr("Open file"),
        QApplication::applicationDirPath(),
        tr("Text files (*.txt *.csv)")
    );

    std::ifstream file;

    try {
        file.open(m_filepath.toStdString());
    } catch (const std::exception &e) {
        auto message = "Unable to open file!";

        log(message);
        std::cerr << message << std::endl;
        ui->statusBar->showMessage(message, 2000);
        return;
    }

    ui->input->setEnabled(false);
    ui->input->clear();

    ui->openButton->setEnabled(false);
    ui->processButton->setEnabled(false);
    ui->saveButton->setEnabled(false);

    const std::streamsize chunkSize = static_cast<std::streamsize>(ui->inputBufferInput->value());

    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::streamsize totalBytesRead = 0;

    std::vector<char> buffer(chunkSize);

    while (file) {
        file.read(buffer.data(), static_cast<std::streamsize>(chunkSize));
        processBuffer(buffer, file.gcount());
        totalBytesRead += file.gcount();

        int progress = static_cast<int>((static_cast<double>(totalBytesRead) / fileSize) * 100);
        ui->statusBar->showMessage(QStringLiteral("%1%").arg(progress), 2000);

        QApplication::processEvents();
    }

    file.close();

    ui->input->setEnabled(true);
    ui->openButton->setEnabled(true);
    ui->processButton->setEnabled(true);

    log("File loading is done!");
    ui->statusBar->showMessage("Done!", 2000);
}

void MainWindow::process()
{
    ui->processButton->setEnabled(false);
    ui->saveButton->setEnabled(false);

    QFuture<void> future = QtConcurrent::run([=]() {
        std::vector<WordCounter> wordCounters = countWords();
        std::sort(wordCounters.begin(), wordCounters.end());

        m_result = wordCounters;
    });
    QFutureWatcher<void> watcher;
    watcher.setFuture(future);

    QEventLoop loop;

    connect(&watcher, &QFutureWatcher<void>::finished, this, [=, &loop]() {
        ui->processButton->setEnabled(true);
        ui->saveButton->setEnabled(true);

        emit finished();
        loop.exit();
    });

    loop.exec();
}

void MainWindow::save()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        tr("Save file"),
        QApplication::applicationDirPath() + QDir::separator() + "ResultPairs.txt",
        tr("Text files (*.txt *.csv)")
    );

    std::ofstream file;

    try {
        file.open(fileName.toStdString());
    } catch (const std::exception &e) {
        auto message = "Unable to open file!";

        log(message);
        std::cerr << message << std::endl;
        ui->statusBar->showMessage(message, 2000);
        return;
    }

    ui->output->setEnabled(false);

    ui->openButton->setEnabled(false);
    ui->processButton->setEnabled(false);
    ui->saveButton->setEnabled(false);

    auto text = ui->output->toPlainText().toStdString();

    const std::streamsize chunkSize = static_cast<std::streamsize>(ui->outputBufferInput->value());

    std::vector<char> buffer(chunkSize);

    for (size_t i = 0; i < text.size(); i += chunkSize) {
        size_t bytesToWrite = std::min(static_cast<size_t>(chunkSize), text.size() - i);
        std::copy(text.begin() + i, text.begin() + i + bytesToWrite, buffer.begin());

        file.write(buffer.data(), bytesToWrite);
    }

    file.close();

    ui->output->setEnabled(true);

    ui->openButton->setEnabled(true);
    ui->processButton->setEnabled(true);
    ui->saveButton->setEnabled(true);

    log("File saving is done!");
    ui->statusBar->showMessage("Done!", 2000);
}
