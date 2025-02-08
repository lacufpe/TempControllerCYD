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

    if (!SPIFFS.begin(true)) {
        Serial.println("Falha ao montar SPIFFS");
        return;
    }

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", "text/html");
        Serial.println("Received request");
    });

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