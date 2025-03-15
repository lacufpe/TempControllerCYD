// fileAccess.h
#ifndef FILEACCESS_H
#define FILEACCESS_H

#include <SPIFFS.h>

void startFS(void);

String initializeNewFile();

const char* createCSVLine(int nowTime, float temperature, int setpoint);

void appendToCSV(const char* filename, const char* line);

#endif
// AMDG