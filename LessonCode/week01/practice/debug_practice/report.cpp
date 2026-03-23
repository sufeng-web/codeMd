#include "report.h"
#include "globals.h"
#include <iostream>
#include <iomanip>

void Report::print(const AnalysisResult& result) const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << RESULTS_HEADER;
    std::cout << "Student: " << g_studentName << "\n";
    std::cout << LABEL_TOTAL << result.Total << "\n";
    std::cout << LABEL_TOTAL << result.Average << "\n";
    std::cout << LABEL_PASS_COUNT << result.PassCount << "\n";
    std::cout << LABEL_MAX << result.MaxScore << "\n";
    std::cout << "Courses: " << g_courseCount << "\n";
}
