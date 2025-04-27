// main.h
#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <Adafruit_ST7789.h>
#include <stdio.h>
#include "LGFX_SPI_ST7789.h"

#include <FS.h>
#include <LittleFS.h>

#include "snake_bg.h"

#include <Wire.h>
#include <WiiChuck.h>

#define RAW_BUFFER_LINES 140

// Konfiguration für den ST7789-Bildschirm
#define SCREEN_W  240
#define SCREEN_H  280

#define TFT_DC  5
#define TFT_CS  8
#define TFT_RST 9

// Joystick variables
#define PIN_JOY_X A0
#define PIN_JOY_Y A1

bool wiichuck;

uint16_t joyXMin, joyXMax, joyXCenter, joyYMin, joyYMax, joyYCenter;
int16_t joy_x, joy_y;

// Canvas-Größe
static uint16_t screen_w = 240;
static uint16_t screen_h = 280;

static uint16_t canv_w = screen_w;
static uint16_t canv_h = screen_h;

extern LGFX tft;
extern LGFX_Sprite* Canvas;

// Game variables
uint8_t score;
static uint16_t snake_color = ST77XX_GREEN;
static uint16_t snake_color_head = ST77XX_YELLOW;
static uint16_t food_color = ST77XX_ORANGE;

uint16_t grid_size = 12; // Größe der Zellen in Pixel
uint16_t grid_w = 19; // Anzahl der Zellen in der Breite
uint16_t grid_h = 19; // Anzahl der Zellen in der Höhe
uint8_t grid_x_offset = (screen_w - (grid_w * grid_size)) / 2 + (grid_size / 2); // Horizontal zentriert
uint8_t grid_y_offset = grid_size * 3; // Offset für die Y-Achse in Pixel

struct Segment
{
    int16_t x;
    int16_t x_next;
    int16_t y;
    int16_t y_next;
    uint8_t size;
};

uint8_t snake_speed = 1; // Geschwindigkeit der Snake in Pixel pro Frame
uint8_t snake_direction = 0; // 0: up, 1: right, 2: down, 3: left
uint8_t snake_next_direction = 0; // Nächste Richtung, die der Snake folgen soll

Segment* snake = NULL; // Initial NULL setzen
uint8_t snake_size = 10; // Startgröße
uint8_t snake_lenght = 4; // Anfangslänge

Segment food;

unsigned long lastMoveTime = 0;
unsigned long timeInterval = 25;  // Start: alle 200 ms ein Schritt

const uint8_t intervalTable[] = 
  {
    25, // Score  0-9
    23, // Score  10-19
    21, // Score  20-29
    19, // Score  30-39
    17, // Score  40-49
    16, // Score  50+
  };

// Functions
void handleJoyStick();
void moveSnake();
void growSnake();
void drawGame();
void resetGame();
uint8_t getIntervalForScore(uint16_t score);
float customMap(long x, long in_min, long in_center, long in_max, long out_min, long out_max);

bool drawRawFile(LGFX& lcd, const char* path, int32_t x, int32_t y, int32_t w, int32_t h);
bool pushRawToSprite(LGFX_Sprite& sprite, const char* path, int32_t x, int32_t y, int32_t w, int32_t h);
bool drawRawFileBuffered(LGFX& lcd, const char* path, int32_t x, int32_t y, int32_t w, int32_t h);
bool pushRawToSpriteBuffered(LGFX_Sprite& sprite, const char* path, int32_t x, int32_t y, int32_t w, int32_t h);
#endif