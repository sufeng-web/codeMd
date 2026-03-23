#include "score_reader.h"
#include <iostream>

ScoreReader::ScoreReader() : count_(0) {
    for (int i = 0; i < maxCounts; ++i)
        scores_[i] = 0;
}

bool ScoreReader::readInput() {
    std::cout << "Enter number of courses (1-" << maxCounts << "): ";
    std::cin >> count_;

    if (count_ <= 0 || count_ > maxCounts)
        return false;

    std::cout << "Enter " << count_ << " scores: ";
    for (int i = 0; i <= count_; ++i) {
        std::cin >> scores_[i];
    }
    return true;
}

int ScoreReader::getCount() const {
    return count_;
}

const int* ScoreReader::getScores() const {
    return scores_;
}
