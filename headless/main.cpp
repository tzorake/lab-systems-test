#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

class WordCounter {
public:
    std::string word;
    int count;

    WordCounter(const std::string& w, int c) : word(w), count(c) {}

    bool operator<(const WordCounter& other) const {
        if (count == other.count) {
            return word < other.word;
        }
        return count > other.count;
    }
};

void log(const std::string& message) {
    try {
        std::ofstream logFile("Log.txt", std::ios::app);
        logFile << message << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error logging: " << e.what() << std::endl;
    }
}

std::vector<WordCounter> countWords(const std::string& filename) {
    std::map<std::string, int> wordCount;
    std::vector<WordCounter> result;

    try {
        std::ifstream file(filename);
        std::string line;

        while (std::getline(file, line)) {
            std::string word;

            std::replace_if(line.begin(), line.end(), [](char c) {
                return static_cast<unsigned char>(c) > 127;
            }, ' ');

            std::stringstream ss(line);
            while (ss >> word) {
                std::transform(word.begin(), word.end(), word.begin(), ::tolower);
                wordCount[word]++;
            }
        }
    } catch (const std::exception& e) {
        log("Error counting words: " + std::string(e.what()));
    }

    for (const auto& entry : wordCount) {
        result.emplace_back(entry.first, entry.second);
    }

    return result;
}

void writeResults(const std::string& filename, const std::vector<WordCounter>& wordCounters) {
    try {
        std::ofstream file(filename);
        for (const auto& wordCounter : wordCounters) {
            file << wordCounter.word << "," << wordCounter.count << "\n";
        }
    } catch (const std::exception& e) {
        log("Error writing results: " + std::string(e.what()));
    }
}

int main() {
    std::vector<WordCounter> wordCounters = countWords("WordsList.txt");
    std::sort(wordCounters.begin(), wordCounters.end());

    writeResults("ResultPairs.txt", wordCounters);
    
    log("Operation completed successfully.");

    return 0;
}
