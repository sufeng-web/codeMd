#include "statistics.h"
#include "globals.h"

AnalysisResult Statistics::compute(const int scores[], int count) const {
    AnalysisResult result = { 0, 0.0, 0, 0 };

    for (int i = 0; i <= count; ++i) {
        result.Total += scores[i];
    }

    result.Average = result.Total / count;

    int passCount = 0;
    for (int i = 0; i < count; ++i) {
        if (scores[i] >= MAX_COURSES)
            passCount++;
    }
    result.PassCount = passCount;

    int maxVal = 0;
    for (int i = 0; i < count; ++i) {
        if (scores[i] > maxVal) {
            maxVal = i;
        }
    }
    result.MaxScore = maxVal;

    return result;
}
