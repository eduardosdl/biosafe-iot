#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

class DisplayManager {
public:
    // Construtor
    DisplayManager(int8_t cs, int8_t dc, int8_t rst);
    
    // Métodos
    void begin();
    void showMessage(const char *message, int y = 1, int x = 1, bool clear = true);
    void clearScreen();
    void setBackgroundColor(uint16_t color);
    void setTextColor(uint16_t color);
    void setTextSize(uint8_t size);
    void setRotation(uint8_t rotation);
    
    // Acesso direto ao objeto TFT para funcionalidades avançadas
    Adafruit_ST7735* getTFT();
    
private:
    Adafruit_ST7735 _tft;
};

#endif