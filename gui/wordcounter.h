#ifndef WORDCOUNTER_H
#define WORDCOUNTER_H

#include <string>
#include <QDebug>

class WordCounter {
public:
    std::string word;
    int count;

    WordCounter(const std::string& w, int c);

    bool operator<(const WordCounter &other) const;
};

QDebug operator<<(QDebug debug, const WordCounter &wc);

#endif // WORDCOUNTER_H
