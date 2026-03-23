#include "globals.h"
#include <cstddef>

const int  PASS_THRESHOLD = 60;
const int MAX_COURSES = 10;

const char* const LABEL_TOTAL = "Total: ";
const char* const LABEL_AVERAGE = "Average: ";
const char* const LABEL_PASS_COUNT = "Pass count: ";
const char* const LABEL_MAX = "Max: ";
const char* const RESULTS_HEADER = "\n--- Results ---\n";

int g_courseCount = 0;

void SetGlobalCourseCount(int n) {
    g_courseCount = n;
}

char g_studentName[64] = {};

void SetGlobalStudentName(const char* src) {
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