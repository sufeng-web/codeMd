#ifndef GLOBALS_H
#define GLOBALS_H

// Constants (used for thresholds and labels)
extern const int PASS_THRESHOLD;
extern const int MAX_COURSES;

extern const char* const LABEL_TOTAL;
extern const char* const LABEL_AVERAGE;
extern const char* const LABEL_PASS_COUNT;
extern const char* const LABEL_MAX;
extern const char* const RESULTS_HEADER;

// Global state (set by reader, read by report for "Courses: N" line)
extern int g_courseCount;
void SetGlobalCourseCount(int n);

// Global state for student name (set by NameReader, read by Report)
extern char g_studentName[64];
void SetGlobalStudentName(const char* src);

#endif
