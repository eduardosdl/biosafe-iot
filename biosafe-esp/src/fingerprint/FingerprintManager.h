#ifndef FINGERPRINT_MANAGER_H
#define FINGERPRINT_MANAGER_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include "../display/DisplayManager.h"
#include <functional>

enum AuthState {
    AUTH_IDLE,
    AUTH_WAIT_FINGER,
    AUTH_PROCESS_IMAGE,
    AUTH_IDENTIFY,
    AUTH_SUCCESS,
    AUTH_FAILED
};

enum EnrollState {
    IDLE,
    WAIT_FIRST_FINGER,
    PROCESS_FIRST_IMAGE,
    WAIT_REMOVE_FINGER,
    WAIT_SECOND_FINGER,
    PROCESS_SECOND_IMAGE,
    CREATE_MODEL,
    STORE_MODEL,
    FAILED
};

typedef void (*MqttPublish)(const char* topic, const char* payload);

class FingerprintManager {
public:
    // Construtor
    FingerprintManager(uint8_t rx_pin, uint8_t tx_pin, DisplayManager* display);
    
    // Métodos de inicialização
    bool begin(uint32_t baudrate = 57600);
    void showModuleInfo();
    void setMqttPublish(MqttPublish callback);
    
    // Operações com impressão digital
    void startAuth();
    void startEnroll();
    bool deleteFingerprint(uint16_t id);
    bool emptyDatabase();
    
    // Métodos para obtenção de informações
    int getTemplateCount();
    bool isEnrolling();
    
    // Métodos de controle
    void updateAuthProcess();
    void updateEnrollProcess();

private:
    SoftwareSerial _serial;
    Adafruit_Fingerprint _finger;
    DisplayManager* _display;
    EnrollState _currentEnrollState = IDLE;
    AuthState _currentAuthState = AUTH_IDLE;
    int _count;
    uint8_t _lockPin;
    uint8_t _enrollId = 0;
    MqttPublish _mqttPublish;

    void sendMqttMessage(const char* topic, const int16_t id, const bool status);
};

#endif // FINGERPRINT_MANAGER_H