#ifndef __DEBUG_PRACTICE_SCORE_READER_H__
#define __DEBUG_PRACTICE_SCORE_READER_H__

class ScoreReader 
{
public:
    ScoreReader();

    bool readInput();
    int getCount() const;
    const int* getScores() const;
    static const int s_maxCounts = 5;

private:
    int m_scores[s_maxCounts];
    int m_count;
};

#endif
