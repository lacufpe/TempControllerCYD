#include "fileAccess.h"

void startFS(void){
    if (!SPIFFS.begin(true)) {
        Serial.println("Falha ao montar SPIFFS");
        return;
    }
}

const char* createCSVLine(int nowTime, float temperature, int setpoint) {
    static char csvLine[50]; // Static buffer to hold the CSV line
  
    snprintf(csvLine, sizeof(csvLine), "%d,%.2f,%d\n", nowTime, temperature, setpoint);
  
    return csvLine;
}

void appendToCSV(const char* filename, const char* line){
    File file = SPIFFS.open(filename, "a");
    if (!file) {
        Serial.println("Failed to open file for appending");
        return;
    }
    
    if (file.print(line)) {
        Serial.println("Line appended");
    } else {
        Serial.println("File write failed");
    }
    file.close();
}

String getNextAvailableFilename() {
    for (int i = 0; i <= 9999; i++) {
      char filename[15]; // "dataXXXX.csv\0" (14 chars + null terminator)
      snprintf(filename, sizeof(filename), "/data%04d.csv", i);
  
      if (!SPIFFS.exists(filename)) {
        return String(filename);
      }
    }
    return ""; // No available filename found
}
  
String initializeNewFile() {
    String filename = getNextAvailableFilename();

    if (filename != "") {
        File newFile = SPIFFS.open(filename, "w"); // Create new file in write mode
        if (newFile) {
            newFile.println("timestamp,temperature,setpoint"); // Write header
            newFile.close();
            Serial.print("New file created: ");
            Serial.println(filename);
        } else {
            Serial.println("Failed to create new file.");
        }
    } else {
        Serial.println("No available filename found.");
    }
    return filename;
}
  

// AMDG