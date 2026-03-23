#ifndef __DEBUG_PRACTICE_STATISTICS_H__
#define __DEBUG_PRACTICE_STATISTICS_H__

#include "analysis_result.h"

class Statistics {
public:
    AnalysisResult compute(const int scores[], int count) const;
};

#endif
