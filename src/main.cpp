#include "main.h"

// ST7789-Display initialisieren
LGFX tft;  // unser Displayobjekt

// Canvas Buffer    // Canvas-Puffer
LGFX_Sprite* Canvas;

void setup()
{
  // Initialize Serial Monitor    // Serial Monitor initialisieren
  Serial.begin(115200);
  delay(2000);

  Serial.println("[INFO] Joystick Kalibrieren...");
  Serial.println("[INFO] Den Joystick loslassen bevor das Programm gestartet wird.");
  Serial.println("[INFO] Nach dem Start den Joystick in alle Richtungen bewegen");

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
  pinMode(PIN_JOY_X, INPUT);
  pinMode(PIN_JOY_Y, INPUT);

  joyXMin = analogRead(PIN_JOY_X) - 50;
  joyXMax = analogRead(PIN_JOY_X) + 50;
  joyXCenter = analogRead(PIN_JOY_X);

  joyYMin = analogRead(PIN_JOY_Y) - 50;
  joyYMax = analogRead(PIN_JOY_Y) + 50;
  joyYCenter = analogRead(PIN_JOY_Y);

  // Initialize Game variables    // Spielvariablen initialisieren
  resetGame();
}

void loop()
{
  // Joystick calibration    // Joystick Kalibrierung
  joy_x = analogRead(PIN_JOY_X);
  joy_y = analogRead(PIN_JOY_Y);

  //Serial.print(joy_x);
  //Serial.print(",");
  //Serial.println(joy_y);

  if (joy_x < joyXMin) joyXMin = joy_x;
  if (joy_x > joyXMax) joyXMax = joy_x;
  if (joy_y < joyYMin) joyYMin = joy_y;
  if (joy_y > joyYMax) joyYMax = joy_y;
  
  joy_x = customMap(joy_x, joyXMin, joyXCenter, joyXMax, -100, 100);
  joy_y = customMap(joy_y, joyYMin, joyYCenter, joyYMax, -100, 100);

  Serial.print(joy_x);
  Serial.print(",");
  Serial.println(joy_y);

  // Joystick movement    // Joystick Bewegung
  if (snake_direction == snake_next_direction)
  {
    if (joy_x < -75) snake_next_direction = 0; // up
    else if (joy_x > 75) snake_next_direction = 2; // down
    else if (joy_y < -75) snake_next_direction = 3; // left
    else if (joy_y > 75) snake_next_direction = 1; // right
  }

  // 3: left | 1: right | 0: up | 2: down

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
      if (snake[i].x > snake[i].x_next) snake[i].x = snake[i].x_next;  // Korrektur!
    }
    if (snake[i].x > snake[i].x_next) 
    {
      snake[i].x -= snake_speed;
      if (snake[i].x < snake[i].x_next) snake[i].x = snake[i].x_next;  // Korrektur!
    }
    if (snake[i].y < snake[i].y_next) 
    {
      snake[i].y += snake_speed;
      if (snake[i].y > snake[i].y_next) snake[i].y = snake[i].y_next;  // Korrektur!
    }
    if (snake[i].y > snake[i].y_next) 
    {
      snake[i].y -= snake_speed;
      if (snake[i].y < snake[i].y_next) snake[i].y = snake[i].y_next;  // Korrektur!
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

  drawGame();
}

void drawGame()
{
  Canvas->fillScreen(TFT_BLACK);

  // Gitter zeichnen    // Draw grid    macht das Spiel sehr langsam
  /*
  for (int i = 0; i < grid_w ; i++)
  {
    for (int j = 0; j < grid_h; j++)
    {
      int x = i * grid_size + grid_x_offset - grid_size / 2 +1;
      int y = j * grid_size + grid_y_offset - grid_size / 2 +1;
      if (j % 2 == 0)
      {
        if (i % 2 == 0) Canvas->fillRect(x, y, grid_size, grid_size, 0x558A); // Gitter zeichnen
        else Canvas->fillRect(x, y, grid_size, grid_size, 0x4428); // Gitter zeichnen
      }
      else
      {
        if (i % 2 == 0) Canvas->fillRect(x, y, grid_size, grid_size, 0x4428); // Gitter zeichnen
        else Canvas->fillRect(x, y, grid_size, grid_size, 0x558A); // Gitter zeichnen
      }
    }
  }
  */

  // Rahmen zeichnen    // Draw border
  Canvas->drawRoundRect(grid_x_offset - grid_size / 2 -1, grid_y_offset - grid_size / 2-1, grid_w * grid_size + 2, grid_h * grid_size + 2, 10, TFT_WHITE); // Rahmen zeichnen
  
  
  // Snake zeichnen    // Draw snake
  for (int i = 0; i < snake_lenght; i++)
  {
    Canvas->fillCircle(snake[i].x, snake[i].y, snake[0].size, snake_color);
    //Canvas->fillCircle(snake[i].x_next, snake[i].y_next, 3, TFT_RED);
  }
  // Food zeichnen    // Draw food
  Canvas->fillCircle(food.x, food.y, food.size, food_color);

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

    snake[i].size = grid_size / 2;
  }

  food.x = random(0, grid_w) * grid_size + grid_x_offset;
  food.y = random(0, grid_h) * grid_size + grid_y_offset;
  food.size = grid_size / 2;
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

  if (score % 25 == 0) snake_speed++; // Alle 5 Punkte Geschwindigkeit erhöhen
  if (snake_speed > grid_size / 2) snake_speed = grid_size / 2; // Max. Geschwindigkeit = halbe Grid-Größe
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
