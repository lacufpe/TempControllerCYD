#include "myWebServer.h"

// SSID and password for ESP as Access Point
const char* ssid = "TempController";
const char* password = "12345678";

unsigned int htmlUpdateTime;

// Start WebServer and WebSocket
AsyncWebServer server(80);
WebSocketsServer webSocket(81);

// A ser chamado pelo startAcq()
void sendReseteGraphCommand(){
    webSocket.broadcastTXT("{\"tipo\":\"limpar\"}");  // Envia comando para limpar o gráfico
}

// String generateDownloadLinks() {
//     String html = "<html><body><h1>Download CSV Files</h1><ul>";

//     int fileCount = 0;
//     for (int i = 9999; i >= 0 && fileCount < 10; i--) { // Check from highest to lowest
//         char filename[15];
//         snprintf(filename, sizeof(filename), "/data%04d.csv", i);

//         if (SPIFFS.exists(filename)) {
//         html += "<li><a href=\"/download?file=";
//         html += filename;
//         html += "\">";
//         html += filename;
//         html += "</a></li>";
//         fileCount++;
//         }
//     }

//     html += "</ul></body></html>";
//     return html;
// }
String generateDownloadLinks() {
    String html = "<html><body><h1>Download CSV Files</h1><ul>";
    File root = SPIFFS.open("/");
    File file;
    int fileCount = 0;
    std::vector<String> files;
  
    // Collect all CSV files
    while ((file = root.openNextFile())) {
      if (String(file.name()).endsWith(".csv")) {
        files.push_back(String(file.name()));
      }
      file.close();
    }
  
    // Sort files in descending order (assuming filenames are sortable)
    std::sort(files.begin(), files.end(), [](const String &a, const String &b) {
      return a > b;
    });
  
    // Add top 10 files to HTML
    for (const String &filename : files) {
      if (fileCount >= 10) break;
      html += "<li><a href=\"/download?file=";
      html += filename;
      html += "\">";
      html += filename;
      html += "</a></li>";
      fileCount++;
    }
  
    html += "</ul></body></html>";
    return html;
}

void handleDownload(AsyncWebServerRequest *request) {
    if (request->hasParam("file")) {
        String filename = String("/") + (request->getParam("file")->value());
        // Serial.println(filename);
        if (SPIFFS.exists(filename)) {
            // File downloadFile = SPIFFS.open(filename, "r");
            // if (downloadFile) {
            //     request->send(downloadFile, filename, "application/octet-stream");
            //     downloadFile.close();
            request->send(SPIFFS, filename, "application/octet-stream", true);
        } else {
        request->send(404, "text/plain", "File not found.");
        }
    } else {
        request->send(400, "text/plain", "Missing file parameter.");
    }
}

void handleWebSocketMessage(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
    if (type == WStype_TEXT) {
        String mensagem = String((char*)payload);
        Serial.println("Recebido: " + mensagem);

        if (mensagem == "{\"tipo\":\"iniciar\"}") {
            startAcq();
        } else if (mensagem == "{\"tipo\":\"parar\"}") {
            stopAcq();
        }
    }
}

void startNetwork() {
    WiFi.softAP(ssid, password);
    Serial.println("Rede criada!");
    Serial.print("ESP32 IP: ");
    Serial.println(WiFi.softAPIP());

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", "text/html");
        Serial.println("Received request");
    });
    server.on("/downloads",HTTP_GET,[](AsyncWebServerRequest *request) {
        request->send(200,"text/html",generateDownloadLinks());
    });
    server.on("/download", HTTP_GET, handleDownload);

    server.begin();
    webSocket.begin();
    webSocket.onEvent(handleWebSocketMessage);
}



void handleNetwork() {
    webSocket.loop();

    static unsigned long lastSend = 0;
    if (aquisicaoAtiva && millis() - lastSend > 1000) {  // Envia dados apenas se estiver ativo
        lastSend = millis();

        int temperatura = temperatureValues[measIdx];
        int setpoint = setpointValues[measIdx];
        int tempo = timeValues[measIdx];

        String json = "{\"tipo\":\"medida\", \"tempo\": " + String(tempo) + 
                      ", \"temperatura\": " + String(temperatura) + 
                      ", \"setpoint\": " + String(setpoint) + "}";

        webSocket.broadcastTXT(json);
        measIdx++; // Lembrar de tirar isto depois
    }
}

void SPIFFS_test(void){
    if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  File file = SPIFFS.open("/index.html");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }
  
  Serial.println("File Content:");
  while(file.available()){
    Serial.write(file.read());
  }
  file.close();
}



//AMDG