#ifndef STATISTICS_H
#define STATISTICS_H

#include "analysis_result.h"

class Statistics {
public:
    AnalysisResult compute(const int scores[], int count) const;
};

#endif
