#include "DisplayManager.h"

// Construtor
DisplayManager::DisplayManager(int8_t cs, int8_t dc, int8_t rst) : _tft(cs, dc, rst) {
}

// Inicialização do display
void DisplayManager::begin() {
    _tft.initR(INITR_144GREENTAB);
    _tft.fillScreen(ST77XX_BLACK);
    _tft.setTextColor(ST77XX_WHITE);
    _tft.setTextSize(1);
    _tft.setRotation(3);
}

// Exibir mensagem no display
void DisplayManager::showMessage(const char *message, int y, int x, bool clear) {
    if (clear) {
        _tft.fillScreen(ST77XX_BLACK);
        _tft.setTextColor(ST77XX_WHITE);
        _tft.setTextSize(1);
    }
    
    _tft.setCursor(x, y);
    _tft.print(message);

    Serial.println(message);
}

// Limpar a tela
void DisplayManager::clearScreen() {
    _tft.fillScreen(ST77XX_BLACK);
}

void DisplayManager::setBackgroundColor(uint16_t color) {
    _tft.fillScreen(color);
}

// Definir cor do texto
void DisplayManager::setTextColor(uint16_t color) {
    _tft.setTextColor(color);
}

// Definir tamanho do texto
void DisplayManager::setTextSize(uint8_t size) {
    _tft.setTextSize(size);
}

// Definir rotação do display
void DisplayManager::setRotation(uint8_t rotation) {
    _tft.setRotation(rotation);
}

// Obter acesso direto ao objeto TFT para funcionalidades avançadas
Adafruit_ST7735* DisplayManager::getTFT() {
    return &_tft;
}