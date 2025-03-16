#include "fileAccess.h"

void startFS(void){
    if (!SPIFFS.begin(true)) {
        Serial.println("Falha ao montar SPIFFS");
        return;
    }
    cleanUpOldCSVFiles();
}

void cleanUpOldCSVFiles() {
  std::vector<String> csvFiles;

  // 1. List all CSV files
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while (file) {
    String filename = file.name();
    if (filename.endsWith(".csv")) {
      csvFiles.push_back(filename);
    }
    file.close();
    file = root.openNextFile();
    yield(); // Prevent WDT timeout
  }
  root.close();

  // 2. Sort files in numerical order (assuming format "dataXXXX.csv")
  std::sort(csvFiles.begin(), csvFiles.end(), [](const String &a, const String &b) {
    // Extract numeric part from "dataXXXX.csv"
    int numA = a.substring(5, a.length() - 4).toInt(); // "data0001.csv" -> 1
    int numB = b.substring(5, b.length() - 4).toInt();
    return numA > numB; // Sort descending (newest first)
  });

  // 3. Delete all but first 10 files
  for (size_t i = QUANT_CSV_FILES; i < csvFiles.size(); i++) {
    SPIFFS.remove("/"+csvFiles[i]);
    Serial.printf("Deleted: %s\n", csvFiles[i].c_str());
    yield(); // Prevent WDT timeout
  }

  Serial.printf("Kept %d/%d CSV files\n", 
              std::min(10, (int)csvFiles.size()), 
              csvFiles.size());
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

// String getNextAvailableFilename() {
//     for (int i = 0; i <= 9999; i++) {
//       char filename[15]; // "dataXXXX.csv\0" (14 chars + null terminator)
//       snprintf(filename, sizeof(filename), "/data%04d.csv", i);
  
//       if (!SPIFFS.exists(filename)) {
//         return String(filename);
//       }
//     }
//     return ""; // No available filename found
// }
String getNextAvailableFilename() {
    int maxNumber = -1;
  
    // 1. Find the highest existing file number
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    while (file) {
      String filename = file.name();
      if (filename.startsWith("data") && filename.endsWith(".csv")) {
        // Extract numeric part (e.g., "data0025.csv" → 25)
        String numberStr = filename.substring(4, filename.length() - 4);
        int number = numberStr.toInt();
        if (number > maxNumber) {
          maxNumber = number;
        }
      }
      file.close();
      file = root.openNextFile();
      yield(); // Prevent watchdog timeout
    }
    root.close();
  
    // 2. Generate the next filename (maxNumber + 1)
    char filename[15];
    if (maxNumber == -1) {
      // No files found: start at 0000
      snprintf(filename, sizeof(filename), "/data%04d.csv", 0);
    } else if (maxNumber < 9999) {
      // Increment highest number
      snprintf(filename, sizeof(filename), "/data%04d.csv", maxNumber + 1);
    } else {
      // All slots (0000-9999) are used
      return "";
    }
  
    // 3. Double-check availability (optional)
    if (SPIFFS.exists(filename)) {
      Serial.println("Conflict: Filename already exists!");
      return "";
    }
  
    return String(filename);
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