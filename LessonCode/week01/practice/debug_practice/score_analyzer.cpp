/**
 * Debug Detective Practice (multi-file, class-based).
 *
 * Expected: input 5 scores 80, 72, 90, 65, 88 and a student name (e.g. "Alice")
 *   -> total=395, average=79.00, pass count=4, max=90, Courses: 5, Student: Alice
 *
 * Bugs are spread across: NameReader (string), ScoreReader, Statistics, Report,
 * main, globals. Use breakpoints, Watch (globals), Call Stack, Step Into (F11).
 */

#include "score_reader.h"
#include "name_reader.h"
#include "statistics.h"
#include "report.h"
#include "globals.h"
#include <iostream>

int main() {
    std::cout << "=== Score Analyzer (Debug Detective) ===\n";

    NameReader nameReader;
    if (!nameReader.readName()) {
        std::cout << "Invalid name input.\n";
        return 1;
    }
    setGlobalStudentName(nameReader.getName());

    ScoreReader reader;
    if (!reader.readInput()) {
        std::cout << "Invalid input.\n";
        return 1;
    }

    setGlobalCourseCount(g_courseCount);

    Statistics stats;
    AnalysisResult result = stats.compute(reader.getScores(), reader.getCount());

    Report report;
    report.print(result);

    return 0;
}
