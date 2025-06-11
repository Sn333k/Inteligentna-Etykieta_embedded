A simple web-based tool for wirelessly uploading black-and-white images to an E-Ink display connected to a NodeMCU v3 (ESP8266). 
The microcontroller runs as a standalone Wi-Fi Access Point, hosting a basic web interface for uploading .pbm images directly to the display.

**System Components**:

ESP8266 Microcontroller:

Creates its own Wi-Fi Access Point (epaper-ap)

Hosts a minimal web server with an HTML upload form

Accepts .pbm image files (binary PBM format: P4)

Parses and renders the image on a 1-bit E-Ink display via SPI

Connecting:

E-PAPER SHIELD - NODEMCUv3

EPD_BUSY - D0

EPD_RST - D1
 
EPD_DC - D2

EPD_CS - D4

EPD_CLK - D5

EPD_DIN - D7

EPD_GND - GND

EPD_VCC - 3V3

**Supported Image Format:**

PBM (Portable Bitmap) format, binary version (P4)

Image resolution must be 200×200 pixels

1-bit color depth (black and white only)

**How to Use**
Power on the ESP8266 — it creates a Wi-Fi AP named epaper-ap with password epaper123

Connect your device (e.g., phone/laptop) to the AP

Start a WEB frontend

Open a browser and go to 'YourIP':3000

Use the upload form to select and submit a .pbm file

The E-Ink display will update with the uploaded image


Convert images on the frontend directly to PBM

Wi-Fi configuration via captive portal
