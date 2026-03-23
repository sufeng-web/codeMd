#include "statistics.h"
#include "globals.h"

AnalysisResult Statistics::compute(const int scores[], int count) const {
    AnalysisResult result = { 0, 0.0, 0, 0 };

    for (int i = 0; i < count; ++i) {
        result.Total += scores[i];
    }

    result.Average = static_cast<double>(result.Total) / count;

    int passCount = 0;
    for (int i = 0; i < count; ++i) {
        if (scores[i] >= PASS_THRESHOLD)
            passCount++;
    }
    result.PassCount = passCount;

    int maxVal = 0;
    for (int i = 0; i < count; ++i) {
        if (scores[i] > maxVal) {
            maxVal = scores[i];
        }
    }
    result.MaxScore = maxVal;

    return result;
}
