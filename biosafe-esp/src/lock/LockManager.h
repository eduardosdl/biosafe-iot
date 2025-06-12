#ifndef LOCK_MANAGER_H
#define LOCK_MANAGER_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <functional>
#include "../display/DisplayManager.h"
#include "../fingerprint/FingerprintManager.h"

// Define o estado da trava eletrônica
enum LockState {
    LOCKED,     // Fechadura trancada
    UNLOCKED,   // Fechadura destrancada
    ERROR       // Erro na operação da fechadura
};

// Define um tipo de função de callback para mensagens MQTT
typedef void (*LockMqttPublish)(const char* topic, const char* payload);

class LockManager {
public:
    // Construtor
    LockManager(uint8_t lockPin, uint8_t cloneseBtnPin, DisplayManager* display, FingerprintManager* fingerprint);
    
    // Métodos de inicialização
    void begin();
    void setMqttPublish(LockMqttPublish callback);
    
    // Métodos de controle
    bool unlock(uint16_t userId = 0, const char* username = "");
    bool lock();
    void update();

    // Métodos de status
    bool isOpen() const;
    
private:
    uint8_t _lockPin;
    uint8_t _closeBtnPin;
    DisplayManager* _display;
    FingerprintManager* _fingerprint;
    LockState _lockState;
    LockMqttPublish _mqttPublish;

    void sendMqttStatus(uint16_t userId, bool unlocked, bool success);
};

#endif
