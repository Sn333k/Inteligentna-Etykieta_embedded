#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FS.h>  // SPIFFS

const char* ssid = "UPC9704699";
const char* password = "v2uaakvjadEe";

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "Serwer ESP8266 działa!");
}

void handleUpload() {
  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
    Serial.printf("Rozpoczęto upload: %s\n", upload.filename.c_str());
    SPIFFS.remove("/received.bmp");
    File f = SPIFFS.open("/received.bmp", "w");
    if (!f) Serial.println("Nie udało się utworzyć pliku!");
    f.close();
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    Serial.printf("Zapis do SPIFFS: %u bajtów\n", upload.currentSize);
    File f = SPIFFS.open("/received.bmp", "a");
    if (f) {
      f.write(upload.buf, upload.currentSize);
      f.close();
    } else {
      Serial.println("Błąd otwierania pliku do zapisu");
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    Serial.printf("Upload zakończony, rozmiar: %u\n", upload.totalSize);
  }
}


void setup() {
  Serial.begin(115200);
  delay(1000);

  if (!SPIFFS.begin()) {
    Serial.println("Błąd montowania SPIFFS");
    return;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Łączenie z WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Połączono! IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);

  server.on("/upload", HTTP_OPTIONS, []() {
  // Odpowiedź na preflight (CORS)
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
  server.send(204);
});

server.on("/upload", HTTP_POST, []() {
  // Główna odpowiedź po uploadzie
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "Upload OK");
}, handleUpload);

  server.begin();
  Serial.println("Serwer HTTP uruchomiony");
}

void loop() {
  server.handleClient();
}
