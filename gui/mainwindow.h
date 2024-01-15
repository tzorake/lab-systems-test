#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "wordcounter.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void log(const std::string &message);

    void processBuffer(const std::vector<char> &buffer, std::streamsize size);
    std::vector<WordCounter> countWords();
    void handle();

signals:
    void finished();

private slots:
    void open();
    void process();
    void save();

private:
    std::vector<WordCounter> m_result;
    QString m_filepath;

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
