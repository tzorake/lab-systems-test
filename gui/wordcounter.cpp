#include "wordcounter.h"

WordCounter::WordCounter(const std::string &w, int c)
    : word(w)
    , count(c)
{

}

bool WordCounter::operator<(const WordCounter &other) const
{
    if (count == other.count) {
        return word < other.word;
    }
    return count > other.count;
}

QDebug operator<<(QDebug debug, const WordCounter &wc)
{
    QDebugStateSaver stateSaver(debug);
    debug << QStringLiteral("WordCounter(%1 %2)").arg(QString::fromUtf8(wc.word.c_str())).arg(wc.count);
    return debug;
}
