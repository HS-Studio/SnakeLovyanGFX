#include "main.h"

// ST7789-Display initialisieren
LGFX tft;  // unser Displayobjekt

// Canvas Buffer    // Canvas-Puffer
LGFX_Sprite* Canvas;

Accessory accessory(Wire1); // Accessory-Objekt erstellen

void setup()
{
  // Initialize Serial Monitor    // Serial Monitor initialisieren
  Serial.begin(115200);
  delay(2000);

  Serial.println("[INFO] Joystick Kalibrieren...");
  Serial.println("[INFO] Den Joystick loslassen bevor das Programm gestartet wird.");
  Serial.println("[INFO] Nach dem Start den Joystick in alle Richtungen bewegen");
  Serial.println();

  Serial.println("[INFO] Joystick calibration...");
  Serial.println("[INFO] Release the joystick before starting the program.");
  Serial.println("[INFO] After starting, move the joystick in all directions.");
  Serial.println();

  if (!LittleFS.begin()) {
    Serial.println("Fehler beim Starten von LittleFS!");
    while (1);
  } else {
    Serial.println("LittleFS erfolgreich gestartet!");
  }

  Wire1.begin();
  delay(500);
  accessory.begin();

  // Speicher reservieren
  snake = (Segment*)malloc(sizeof(Segment) * snake_size);
  if (!snake)
  {
    Serial.println("[ERROR] Speicherzuweisung fehlgeschlagen!");
    while (1);
  }

  // Initialize Canvas    //Canvas initialisieren
  Canvas = (LGFX_Sprite *)malloc(sizeof(LGFX_Sprite));
  if (!Canvas)
  {
    Serial.println("[ERROR] RAM-Allocation fehlgeschlagen!");
    while (1);
  }

  tft.init();
  tft.setRotation(2);
  tft.fillScreen(TFT_BLACK);

  new (Canvas) LGFX_Sprite(&tft);
  Canvas->createSprite(canv_w, canv_h);
  Canvas->setColorDepth(16);
  Canvas->fillScreen(TFT_BLACK);
  Canvas->setRotation(tft.getRotation());

  // Initialize Joystick    // Joystick initialisieren

  if (Wire1.requestFrom(0x52, 1))
  {
    wiichuck = true;// WiiChuck available  // Joystick ist vorhanden
    Serial.println("[INFO] WiiChuck gefunden!");
    if (accessory.type == WIICLASSIC)
    {
      accessory.readData(); // Daten vom Joystick lesen
      joy_x = accessory.getJoyXLeft();
      joy_y = accessory.getJoyYLeft();

      joyXMin = accessory.getJoyXLeft() - 10;
      joyXMax = accessory.getJoyXLeft() + 10;
      joyXCenter = accessory.getJoyXLeft();

      joyYMin = accessory.getJoyYLeft() - 10;
      joyYMax = accessory.getJoyYLeft() + 10;
      joyYCenter = accessory.getJoyYLeft();
    }
    else if (accessory.type == NUNCHUCK)
    {
      accessory.readData(); // Daten vom Joystick lesen
      joy_x = accessory.getJoyX();
      joy_y = accessory.getJoyY();

      joyXMin = accessory.getJoyX() - 10;
      joyXMax = accessory.getJoyX() + 10;
      joyXCenter = accessory.getJoyX();

      joyYMin = accessory.getJoyY() - 10;
      joyYMax = accessory.getJoyY() + 10;
      joyYCenter = accessory.getJoyY();
    }
  }
  else
  {
    Serial.println("[INFO] WiiChuck nicht gefunden!"); // Joystick nicht gefunden
    wiichuck = false; // WiiChuck not available    // WiiChuck ist nicht vorhanden
    Wire1.end(); // Beende den I2C-Bus

    pinMode(PIN_JOY_X, INPUT);
    pinMode(PIN_JOY_Y, INPUT);

    joy_x = analogRead(PIN_JOY_X);
    joy_y = analogRead(PIN_JOY_Y);

    joyXMin = analogRead(PIN_JOY_X) - 10;
    joyXMax = analogRead(PIN_JOY_X) + 10;
    joyXCenter = analogRead(PIN_JOY_X);

    joyYMin = analogRead(PIN_JOY_Y) - 10;
    joyYMax = analogRead(PIN_JOY_Y) + 10;
    joyYCenter = analogRead(PIN_JOY_Y);
  }

  // Initialize Game variables    // Spielvariablen initialisieren
  resetGame();
}

void loop()
{
  handleJoyStick();

  if (millis() - lastMoveTime >= timeInterval) {
    lastMoveTime = millis();
    moveSnake();  // Deine Snake-Logik
  }
  drawGame();
}

void handleJoyStick()
{
  // read joystick data     // Joystick-Daten lesen
  if (wiichuck)
  {
    if (accessory.type == WIICLASSIC)
    {
      accessory.readData(); // Daten vom Joystick lesen
      joy_x = accessory.getJoyXLeft();
      joy_y = accessory.getJoyYLeft();
    }
    else if (accessory.type == NUNCHUCK)
    {
      accessory.readData(); // Daten vom Joystick lesen
      joy_x = accessory.getJoyX();
      joy_y = accessory.getJoyY();
    }
  }
  else
  {
    joy_x = analogRead(PIN_JOY_X);
    joy_y = analogRead(PIN_JOY_Y);
  }
  
  //Serial.print(joy_x);
  //Serial.print(",");
  //Serial.println(joy_y);

  // Joystick calibration    // Joystick Kalibrierung
  if (joy_x < joyXMin) joyXMin = joy_x;
  if (joy_x > joyXMax) joyXMax = joy_x;
  if (joy_y < joyYMin) joyYMin = joy_y;
  if (joy_y > joyYMax) joyYMax = joy_y;
  
  joy_x = customMap(joy_x, joyXMin, joyXCenter, joyXMax, -127, 127);
  joy_y = customMap(joy_y, joyYMin, joyYCenter, joyYMax, -127, 127);

  //Serial.print(joy_x);
  //Serial.print(",");
  //Serial.println(joy_y);

  // Joystick movement    // Joystick Bewegung
  // 0: up | 1: right | 2: down | 3: left
  if (wiichuck)
  {
    if (snake_direction == snake_next_direction)
    {
      if (joy_x < -75) snake_next_direction = 3; // up
      else if (joy_x > 75) snake_next_direction = 1; // down
      else if (joy_y < -75) snake_next_direction = 2; // left
      else if (joy_y > 75) snake_next_direction = 0; // right
    }
  } else {
    if (snake_direction == snake_next_direction)
    {
      if (joy_x < -75) snake_next_direction = 0; // up
      else if (joy_x > 75) snake_next_direction = 2; // down
      else if (joy_y < -75) snake_next_direction = 3; // left
      else if (joy_y > 75) snake_next_direction = 1; // right
    }
  }
}

void moveSnake()
{
  // Snake direction change    // Snake Richtungswechsel
  if (snake_next_direction != snake_direction)
  {
    if ((snake_direction == 0 && snake_next_direction == 2) || (snake_direction == 2 && snake_next_direction == 0) ||
        (snake_direction == 1 && snake_next_direction == 3) || (snake_direction == 3 && snake_next_direction == 1))
    {
      snake_next_direction = snake_direction; // Snake kann nicht in die entgegengesetzte Richtung fahren
    }
    else
    {
      snake_direction = snake_next_direction;
    }
  }

  // Snake Head movement    // Snake Kopf Bewegung
  if (snake[0].x == snake[0].x_next && snake[0].y == snake[0].y_next)
  {
    if (snake_direction == 0) snake[0].y_next -= grid_size; // Up
    else if (snake_direction == 1) snake[0].x_next += grid_size; // Right
    else if (snake_direction == 2) snake[0].y_next += grid_size; // Down
    else if (snake_direction == 3) snake[0].x_next -= grid_size; // Left

    // Update für den Körper
    for (int i = snake_lenght - 1; i > 0; i--)
    {
      snake[i].x_next = snake[i - 1].x;
      snake[i].y_next = snake[i - 1].y;
    }
  }

  // Bewegen, aber nur wenn es noch nicht angekommen ist
  for (int i = 0; i < snake_lenght; i++)
  {
    if (snake[i].x < snake[i].x_next) 
    {
      snake[i].x += snake_speed;
      if (snake[i].x > snake[i].x_next) snake[i].x = snake[i].x_next;
    }
    if (snake[i].x > snake[i].x_next) 
    {
      snake[i].x -= snake_speed;
      if (snake[i].x < snake[i].x_next) snake[i].x = snake[i].x_next;
    }
    if (snake[i].y < snake[i].y_next) 
    {
      snake[i].y += snake_speed;
      if (snake[i].y > snake[i].y_next) snake[i].y = snake[i].y_next;
    }
    if (snake[i].y > snake[i].y_next) 
    {
      snake[i].y -= snake_speed;
      if (snake[i].y < snake[i].y_next) snake[i].y = snake[i].y_next;
    }
  }

  // Snake collision with food    // Snake Kollision mit Nahrung
  if (snake[0].x_next == food.x && snake[0].y_next == food.y)
  {
    growSnake();
  }

  // Snake collision with walls    // Snake Kollision mit Wänden
  if (snake[0].x_next < grid_x_offset || snake[0].x_next >= grid_w * grid_size + grid_x_offset || snake[0].y_next < grid_y_offset || snake[0].y_next >= grid_h * grid_size + grid_y_offset)
  {
    resetGame();
  }

  // Snake collision with itself    // Snake Kollision mit sich selbst
  for (int i = 1; i < snake_lenght; i++)
  {
    if (snake[0].x_next == snake[i].x_next && snake[0].y_next == snake[i].y_next)
    {
      resetGame();
    }
  }
}

void growSnake()
{
  score++;
  snake_lenght++;

  if (snake_lenght >= snake_size)  // Falls Speicher voll
  {
      snake_size += 10;  // Erhöhe um 10 Segmente
      snake = (Segment*)realloc(snake, sizeof(Segment) * snake_size);
      if (!snake)
      {
          Serial.println("[ERROR] Speicherzuweisung fehlgeschlagen!");
          while (1);
      }
  }

  snake[snake_lenght - 1].x = snake[snake_lenght - 2].x;
  snake[snake_lenght - 1].y = snake[snake_lenght - 2].y;
  snake[snake_lenght - 1].x_next = snake[snake_lenght - 2].x_next;
  snake[snake_lenght - 1].y_next = snake[snake_lenght - 2].y_next;

  food.x_next = random(0, grid_w) * grid_size + grid_x_offset;
  food.y_next = random(0, grid_h) * grid_size + grid_y_offset;

  for (int i = snake_lenght - 1; i > 0; i--)
  {
    if (food.x_next == snake[i].x_next && food.y_next == snake[i].y_next)
    {
      food.x_next = random(0, grid_w) * grid_size + grid_x_offset;
      food.y_next = random(0, grid_h) * grid_size + grid_y_offset;
      i = snake_lenght - 1; // Restart loop
    } else {
      // Nur wenn es nicht mit dem Snake Körper kollidiert
      food.x = food.x_next; 
      food.y = food.y_next;
    }
  }

  timeInterval = getIntervalForScore(score);

  if (score == 100) {
    snake_speed = 2; // Snake speed increase
  }
}

void drawGame()
{
  Canvas->clear(TFT_BLACK);

  //pushRawToSpriteBuffered(*Canvas, "/snake_bg.raw", grid_x_offset - grid_size / 2 -1, grid_y_offset - grid_size / 2 -1, 230, 230);

  // Rahmen zeichnen    // Draw border
  Canvas->drawRoundRect(grid_x_offset - grid_size / 2 -1, grid_y_offset - grid_size / 2-1, grid_w * grid_size + 2, grid_h * grid_size + 2, 10, TFT_WHITE); // Rahmen zeichnen
  
  // Food zeichnen    // Draw food
  Canvas->fillCircle(food.x, food.y, food.size, food_color);
  
  // Snake zeichnen    // Draw snake
  for (int i = 0; i < snake_lenght; i++)
  {
    if (i == 0) // Kopf
    {
      Canvas->fillCircle(snake[i].x, snake[i].y, snake[0].size+1, snake_color_head); // Kopf
    }
    else
    {
      Canvas->fillCircle(snake[i].x, snake[i].y, snake[0].size, snake_color);
    }
    //Canvas->fillCircle(snake[i].x_next, snake[i].y_next, 3, TFT_RED);
  }



  // Score zeichnen    // Draw score
  Canvas->setTextColor(TFT_WHITE);
  Canvas->setTextSize(2);
  Canvas->setCursor(10, 10);
  Canvas->print("Score: ");
  Canvas->print(score);

  Canvas->pushSprite(0, 0);
}

void resetGame()
{
  score = 0;
  snake_direction = 1;                    // 0: up, 1: right, 2: down, 3: left
  snake_next_direction = snake_direction; // 0: up, 1: right, 2: down, 3: left
  snake_lenght = 4;                       // Snake Länge
  snake_speed = 1;                        // Snake Geschwindigkeit
  timeInterval = 25;

  for (int i = 0; i < snake_lenght; i++)
  {
    snake[i].x = grid_size * (10 - i) + grid_x_offset;
    snake[i].y = grid_size * 10 + grid_y_offset;

    if (i == 0) // Kopf
    {
      snake[i].x_next = snake[i].x + grid_size; // Erster Schritt nach rechts
      snake[i].y_next = snake[i].y;
    }
    else // Körper folgt dem vorherigen Segment
    {
      snake[i].x_next = snake[i - 1].x;
      snake[i].y_next = snake[i - 1].y;
    }

    snake[i].size = 5;
  }

  food.x = random(0, grid_w) * grid_size + grid_x_offset;
  food.y = random(0, grid_h) * grid_size + grid_y_offset;
  food.size = grid_size / 2;
}

uint8_t getIntervalForScore(uint16_t score) 
{
  uint8_t index = score / 10;  // alle 10 Punkte schneller
  if (index >= sizeof(intervalTable)) index = sizeof(intervalTable) - 1;
  return intervalTable[index];
}

float customMap(long x, long in_min, long in_center, long in_max, long out_min, long out_max)
{
  if (x < in_center)
  {
    return (float)(x - in_center) / (in_center - in_min) * (out_min) * -1; // Skalierung für den Bereich links vom Mittelpunkt
  }
  else
  {
    return (float)(x - in_center) / (in_max - in_center) * (out_max); // Skalierung für den Bereich rechts vom Mittelpunkt
  }
}

bool drawRawFile(LGFX& lcd, const char* path, int32_t x, int32_t y, int32_t w, int32_t h)
{
  File file = LittleFS.open(path, "r");
  if (!file || file.isDirectory())
  {
    Serial.println("[Fehler] Datei konnte nicht geöffnet werden!");
    return false;
  }

  uint16_t* buffer = (uint16_t*) malloc(w * h * sizeof(uint16_t));
  if (!buffer)
  {
    Serial.println("[Fehler] Speicherzuweisung fehlgeschlagen!");
    file.close();
    return false;
  }

  if (file.read((uint8_t*)buffer, w * h * 2) != (w * h * 2))
  {
    Serial.println("[Fehler] Lesen der Datei fehlgeschlagen!");
    free(buffer);
    file.close();
    return false;
  }

  lcd.pushImage(x, y, w, h, buffer);

  free(buffer);
  file.close();
  return true;
}

bool drawRawFileBuffered(LGFX& lcd, const char* path, int32_t x, int32_t y, int32_t w, int32_t h)
{
  File file = LittleFS.open(path, "r");
  if (!file || file.isDirectory())
  {
    Serial.println("[Fehler] Datei konnte nicht geöffnet werden!");
    return false;
  }

  const int linesPerBuffer = RAW_BUFFER_LINES;
  const int bufferSize = w * linesPerBuffer; // w = Pixel pro Zeile
  uint16_t* buffer = (uint16_t*) malloc(bufferSize * sizeof(uint16_t));
  if (!buffer)
  {
    Serial.println("[Fehler] RAM-Allocation fehlgeschlagen!");
    file.close();
    return false;
  }

  int linesDrawn = 0;
  while (linesDrawn < h)
  {
    int linesToRead = linesPerBuffer;
    if ((linesDrawn + linesToRead) > h)
    {
      linesToRead = h - linesDrawn; // Nicht über das Ende lesen
    }

    size_t bytesToRead = w * linesToRead * 2; // 2 Bytes pro Pixel
    if (file.read((uint8_t*)buffer, bytesToRead) != bytesToRead)
    {
      Serial.println("[Fehler] Lesen der Datei fehlgeschlagen!");
      free(buffer);
      file.close();
      return false;
    }

    lcd.pushImage(x, y + linesDrawn, w, linesToRead, buffer);
    linesDrawn += linesToRead;
  }

  free(buffer);
  file.close();
  return true;
}

bool pushRawToSpriteBuffered(LGFX_Sprite& sprite, const char* path, int32_t x, int32_t y, int32_t w, int32_t h)
{
  File file = LittleFS.open(path, "r");
  if (!file || file.isDirectory())
  {
    Serial.println("[Fehler] Datei konnte nicht geöffnet werden!");
    return false;
  }

  const int linesPerBuffer = RAW_BUFFER_LINES;
  const int bufferSize = w * linesPerBuffer; // w = Pixel pro Zeile
  uint16_t* buffer = (uint16_t*) malloc(bufferSize * sizeof(uint16_t));
  if (!buffer)
  {
    Serial.println("[Fehler] RAM-Allocation fehlgeschlagen!");
    file.close();
    return false;
  }

  int linesDrawn = 0;
  while (linesDrawn < h)
  {
    int linesToRead = linesPerBuffer;
    if ((linesDrawn + linesToRead) > h)
    {
      linesToRead = h - linesDrawn; // Nicht über das Ende lesen
    }

    size_t bytesToRead = w * linesToRead * 2; // 2 Bytes pro Pixel
    if (file.read((uint8_t*)buffer, bytesToRead) != bytesToRead)
    {
      Serial.println("[Fehler] Lesen der Datei fehlgeschlagen!");
      free(buffer);
      file.close();
      return false;
    }

    sprite.pushImage(x, y + linesDrawn, w, linesToRead, buffer);
    linesDrawn += linesToRead;
  }

  free(buffer);
  file.close();
  return true;
}

bool pushRawToSprite(LGFX_Sprite& sprite, const char* path, int32_t x, int32_t y, int32_t w, int32_t h)
{
  File file = LittleFS.open(path, "r");
  if (!file || file.isDirectory())
  {
    Serial.println("[Fehler] Datei konnte nicht geöffnet werden!");
    return false;
  }

  uint16_t* buffer = (uint16_t*) malloc(w * h * sizeof(uint16_t));
  if (!buffer)
  {
    Serial.println("[Fehler] Speicherzuweisung fehlgeschlagen!");
    file.close();
    return false;
  }

  if (file.read((uint8_t*)buffer, w * h * 2) != (w * h * 2))
  {
    Serial.println("[Fehler] Lesen der Datei fehlgeschlagen!");
    free(buffer);
    file.close();
    return false;
  }

  //lcd.pushImage(x, y, w, h, buffer);
  
  sprite.pushImage(x, y, w, h, buffer);

  free(buffer);
  file.close();
  return true;
}