#ifndef REPORT_H
#define REPORT_H

#include "analysis_result.h"

class Report {
public:
    void print(const AnalysisResult& result) const;
};

#endif
