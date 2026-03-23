#ifndef SCORE_READER_H
#define SCORE_READER_H

class ScoreReader {
public:
    static const int s_maxCounts = 5;

    ScoreReader();

    bool readInput();
    int getCount() const;
    const int* getScores() const;

private:
    int m_scores[s_maxCounts];
    int m_count;
};

#endif
