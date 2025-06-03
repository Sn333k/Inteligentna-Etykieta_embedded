📡E-Ink Wi-Fi Uploader
A simple web-based tool for wirelessly uploading black-and-white images to a 1.54" 200x200px E-Ink display connected to a NodeMCU v3 (ESP8266). The microcontroller runs as a standalone Wi-Fi Access Point, hosting a basic web interface for uploading .pbm images directly to the display.

🔧 System Components
1. ⚙️ ESP8266 Microcontroller
Creates its own Wi-Fi Access Point (epaper-ap)

Hosts a minimal web server with an HTML upload form

Accepts .pbm image files (binary PBM format: P4)

Parses and renders the image on a 1-bit E-Ink display via SPI

2. 🌐 Web Interface (Frontend)
Built using basic HTML or a ReactJS app

Allows users to select and upload .pbm files

Sends image data via a POST request to the ESP8266 at /upload

🖼️ Supported Image Format
PBM (Portable Bitmap) format, binary version (P4)

Image resolution must be 200×200 pixels

1-bit color depth (black and white only)

Can be generated using tools like GIMP, ImageMagick, or via custom frontend logic

📲 How to Use
Power on the ESP8266 — it creates a Wi-Fi AP named epaper-ap with password epaper123

Connect your device (e.g., phone/laptop) to the AP

Open a browser and go to http://192.168.4.1

Use the upload form to select and submit a .pbm file

The E-Ink display will update with the uploaded image

⚠️ Notes
Only PBM files with binary P4 format are supported.

The file must match the display resolution (200×200 px) exactly.

The system is designed for use with Waveshare-compatible E-Ink displays using a DESPI-C02 SPI interface.

Ensure image rendering logic matches the wiring (rotation, alignment, etc.).

📦 Future Improvements
Add real-time preview of the image before upload

Support grayscale (4-level or 8-level)

Convert images on the frontend directly to PBM

Wi-Fi configuration via captive portal