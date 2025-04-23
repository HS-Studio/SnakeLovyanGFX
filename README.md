# Snake-Spiel für den ESP32

## Projektbeschreibung
Dieses Projekt ist eine Implementierung des klassischen Snake-Spiels auf einem ESP32-Mikrocontroller mit einem **Adafruit ST7789 TFT-Display** und einem **Joystick** zur Steuerung. Die Bewegung der Schlange erfolgt rasterbasiert, mit anpassbarer Geschwindigkeit und Offset-Einstellungen.

## Features
- Rasterbasierte Bewegung der Schlange
- Dynamische Geschwindigkeitserhöhung im Spielverlauf
- Grid kann mit `grid_x_offset` und `grid_y_offset` positioniert werden
- Wachsender Snake-Körper bei Nahrungsaufnahme
- Kollisionserkennung (Wände, Eigenkörper, Nahrung)
- Grafische Darstellung mit Adafruit GFX
- Joystick-Steuerung für Richtungsänderung

## Hardware-Voraussetzungen
### Benötigte Komponenten
- ESP32 Mikrocontroller
- Adafruit ST7789 TFT-Display (240x280 oder 240x240 Pixel)
- Analog-Joystick (2 Achsen, z. B. KY-023 Modul)
- Verbindungskabel, Breadboard (falls notwendig)

### Verkabelung
| Komponente | ESP32 Pin |
|------------|-----------|
| TFT DC     | GPIO 5    |
| TFT CS     | GPIO 8    |
| TFT RST    | GPIO 9    |
| Joystick X | A0        |
| Joystick Y | A1        |

## Installation & Einrichtung (PlatformIO)
### Benötigte Software
- **Visual Studio Code**
- **PlatformIO Plugin**

### Projekt einrichten
1. Öffne **Visual Studio Code**.
2. Installiere das **PlatformIO Plugin** (falls noch nicht geschehen).
3. **Projekt von GitHub klonen**:
   ```sh
   git clone <GITHUB-REPOSITORY-URL>
   ```
4. **PlatformIO-Projekt in VS Code öffnen**:
   Öffne das PlatformIO Home Menü und wähle **Import Existing Project**.
5. **Bibliotheken installieren**:
   - Falls nicht automatisch installiert, füge folgende Bibliotheken in `platformio.ini` hinzu:
   ```ini
   [env:adafruit_qtpy_esp32s2]
   platform = espressif32
   board = adafruit_qtpy_esp32s2
   framework = arduino
   lib_deps = 
      adafruit/Adafruit ST7735 and ST7789 Library@^1.11.0
      lovyan03/LovyanGFX@^1.2.0
   ```
6. **Code kompilieren & auf den ESP32 hochladen**.

## Spielmechanik
### Bewegung
- Die Schlange bewegt sich in einem Grid (`grid_size`)
- Richtungswechsel nur orthogonal (links, rechts, oben, unten)
- Bewegung durch Joystick-Eingabe (`joy_x`, `joy_y`)

### Spielregeln
- **Fressen**: Berührt die Schlange das Essen, wächst sie.
- **Kollision**: Wenn der Kopf die Wand oder den Körper berührt, startet das Spiel neu.
- **Geschwindigkeit**: Steigt mit wachsender Schlange (`snake_speed`).

## Anpassungen & Erweiterungen
### Einstellungen (Änderung in `main.h` möglich)
- `grid_size`: Rastergröße
- `grid_x_offset`, `grid_y_offset`: Position des Grids auf dem Bildschirm
- `snake_speed`: Anfangsgeschwindigkeit der Schlange
- `snake_lenght`: Startlänge der Schlange

## Erweiterungsideen
- Highscore speichern
- Mehrere Spielmodi (z. B. Geschwindigkeitssprünge)
- Soundeffekte 
- Hintergrundanimationen

## Fazit
Dieses ESP32-Projekt bietet ein klassisches Snake-Spiel mit einer anpassbaren Rastermechanik. Viel Spaß beim Programmieren und Spielen!

