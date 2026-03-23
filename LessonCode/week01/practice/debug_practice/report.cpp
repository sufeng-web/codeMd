#include "report.h"
#include "globals.h"
#include <iostream>
#include <iomanip>

void Report::print(const AnalysisResult& result) const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << kResultsHeader;
    std::cout << "Student: " << g_studentName << "\n";
    std::cout << kLabelTotal << result.total << "\n";
    std::cout << kLabelTotal << result.average << "\n";
    std::cout << kLabelPassCount << result.passCount << "\n";
    std::cout << kLabelMax << result.maxScore << "\n";
    std::cout << "Courses: " << g_courseCount << "\n";
}
