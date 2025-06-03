#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <GxEPD2_BW.h>
#include <Adafruit_GFX.h>

// E-Ink 1.54" 200x200
#define CS_PIN   2   // D4
#define DC_PIN   4   // D2
#define RST_PIN  5   // D1
#define BUSY_PIN 16  // D0

GxEPD2_BW<GxEPD2_154_D67, GxEPD2_154_D67::HEIGHT> display(GxEPD2_154_D67(CS_PIN, DC_PIN, RST_PIN, BUSY_PIN));

ESP8266WebServer server(80);

const char* ssid = "epaper-ap";
const char* password = "epaper123";

// Strona HTML testowa (jeśli chcesz dodać podgląd)
const char* index_html = R"rawliteral(
<!DOCTYPE html>
<html>
  <body>
    <h2>Wyślij obraz PBM (200x200)</h2>
    <form action="/upload" method="post" enctype="multipart/form-data">
      <input type="file" name="image"><br><br>
      <input type="submit" value="Wyślij">
    </form>
  </body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleUpload() {
  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
    Serial.println("Rozpoczynam odbieranie obrazu...");
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    Serial.printf("Odebrano %d bajtów\n", upload.currentSize);
    renderPBM(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    Serial.println("Zakończono odbiór pliku");
  }
}
int parsePBMHeader(uint8_t* data, size_t length, int& headerEnd, int& width, int& height) {
  int pos = 0;
  int lineStart = 0;
  int lineCount = 0;
  char line[32];

  // Pierwsza linia: format (P4)
  while (pos < length && data[pos] != '\n') pos++;
  if (pos >= length) return -1; // błąd
  memcpy(line, &data[0], pos);
  line[pos] = '\0';
  if (strcmp(line, "P4") != 0) return -2; // nie P4
  lineCount++;
  pos++; // przejdź za \n

  // Odczytuj linie aż znajdziesz rozmiary
  while (lineCount < 3 && pos < length) {
    // Pomijaj komentarze zaczynające się od '#'
    if (data[pos] == '#') {
      // pomiń całą linię komentarza
      while (pos < length && data[pos] != '\n') pos++;
      pos++;
      continue;
    }

    // odczytaj linię
    int start = pos;
    while (pos < length && data[pos] != '\n') pos++;
    int len = pos - start;
    if (len >= sizeof(line)) return -3; // linia za długa
    memcpy(line, &data[start], len);
    line[len] = '\0';

    // linia z wymiarami
    if (lineCount == 1) {
      // format to komentarz lub rozmiary? Jeśli komentarz pominięte powyżej.
      int w = 0, h = 0;
      if (sscanf(line, "%d %d", &w, &h) == 2) {
        width = w;
        height = h;
        lineCount = 3; // zakończ po odczycie rozmiarów
      }
    }
    pos++; // przejdź za \n
  }

  headerEnd = pos;
  return 0;
}

void renderPBM(uint8_t* data, size_t length) {
  int headerEnd = 0;
  int width = 0, height = 0;
  int res = parsePBMHeader(data, length, headerEnd, width, height);
  if (res != 0) {
    Serial.printf("Błąd nagłówka PBM: %d\n", res);
    return;
  }

  int expectedDataSize = (width / 8) * height;
  if ((length - headerEnd) < expectedDataSize) {
    Serial.println("Obraz zbyt mały lub uszkodzony.");
    return;
  }

  display.setRotation(1);
  display.setFullWindow();
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    int byteIndex = 0;
    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x += 8) {
        uint8_t byte = data[headerEnd + byteIndex++];
        for (int b = 0; b < 8; b++) {
          bool pixel = byte & (0x80 >> b);
          display.drawPixel(x + b, y, pixel ? GxEPD_BLACK : GxEPD_WHITE);
        }
      }
    }
  } while (display.nextPage());
}


void setup() {
  Serial.begin(115200);

  display.init();

  WiFi.softAP(ssid, password);
  Serial.print("AP uruchomione. IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", HTTP_GET, handleRoot);

  server.on("/upload", HTTP_OPTIONS, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "POST, OPTIONS");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.send(204);
  });

  server.on("/upload", HTTP_POST, []() {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "text/plain", "Upload OK");
  }, handleUpload);

  server.begin();
  Serial.println("Serwer HTTP uruchomiony");
}


void loop() {
  server.handleClient();
}
