#include "statistics.h"
#include "globals.h"

AnalysisResult Statistics::compute(const int scores[], int count) const {
    AnalysisResult result = { 0, 0.0, 0, 0 };

    for (int i = 0; i < count; ++i) {
        result.total += scores[i];
    }

    result.average = static_cast<double>(result.total) / count;

    int passCount = 0;
    for (int i = 0; i < count; ++i) {
        if (scores[i] >= PASS_THRESHOLD)
            passCount++;
    }
    result.passCount = passCount;

    int maxVal = 0;
    for (int i = 0; i < count; ++i) {
        if (scores[i] > maxVal) {
            maxVal = scores[i];
        }
    }
    result.maxScore = maxVal;

    return result;
}
