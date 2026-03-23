#ifndef GLOBALS_H
#define GLOBALS_H

// Constants (used for thresholds and labels)
extern const int kPassThreshold;
extern const int kMaxCourses;
extern const char* const kLabelTotal;
extern const char* const kLabelAverage;
extern const char* const kLabelPassCount;
extern const char* const kLabelMax;
extern const char* const kResultsHeader;

// Global state (set by reader, read by report for "Courses: N" line)
extern int g_courseCount;
void setGlobalCourseCount(int n);

// Global state for student name (set by NameReader, read by Report)
extern char g_studentName[64];
void setGlobalStudentName(const char* src);

#endif
