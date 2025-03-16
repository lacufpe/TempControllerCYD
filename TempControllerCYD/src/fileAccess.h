// fileAccess.h
#ifndef FILEACCESS_H
#define FILEACCESS_H

#include <SPIFFS.h>
#include <vector>
#include <algorithm>

#define QUANT_CSV_FILES 10

void cleanUpOldCSVFiles(void);

void startFS(void);

String initializeNewFile();

const char* createCSVLine(int nowTime, float temperature, int setpoint);

void appendToCSV(const char* filename, const char* line);

#endif
// AMDG