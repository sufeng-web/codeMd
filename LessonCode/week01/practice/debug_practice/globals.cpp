#include "globals.h"
#include <cstddef>

const int kPassThreshold = 60;
const int kMaxCourses = 10;

const char* const kLabelTotal = "Total: ";
const char* const kLabelAverage = "Average: ";
const char* const kLabelPassCount = "Pass count: ";
const char* const kLabelMax = "Max: ";
const char* const kResultsHeader = "\n--- Results ---\n";

int g_courseCount = 0;

void setGlobalCourseCount(int n) {
    g_courseCount = n;
}

char g_studentName[64] = {};

void setGlobalStudentName(const char* src) {
    if (!src) {
        g_studentName[0] = '\0';
        return;
    }
    std::size_t i = 0;
    while (src[i] != '\0' && i < 63) {
        g_studentName[i] = src[i];
        ++i;
    }
    g_studentName[i] = '\0';
}