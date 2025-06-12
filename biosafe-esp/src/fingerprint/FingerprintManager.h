#ifndef FINGERPRINT_MANAGER_H
#define FINGERPRINT_MANAGER_H

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include "../display/DisplayManager.h"
#include <functional>

// enum de estados para o módulo de impressão digital
enum FingerprintState {
    FINGERPRINT_IDLE,
    FINGERPRINT_AUTH,
    FINGERPRINT_ENROLL,
};

// enum de estados para o processo de autenticação
enum AuthState {
    AUTH_IDLE,
    AUTH_FINGER_WAIT,
    AUTH_FINGER_PROCESS,
    AUTH_IDENTIFY,
    AUTH_FAILED
};

// enum de estados para o processo de registro
enum EnrollState {
    ENROLL_IDLE,
    ENROLL_FIRST_CAPTURE,
    ENROLL_FIRST_REMOVE,
    ENROLL_SECOND_CAPTURE,
    ENROLL_SECOND_REMOVE,
    ENROLL_CREATE_MODEL,
    ENROLL_STORE_MODEL,
    ENROLL_WAIT_SECOND_FINGER,
    ENROLL_FAILED
};

// Define um tipo de função de callback para mensagens MQTT
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
    void stopAuth();
    void startEnroll();
    bool deleteFingerprint(uint16_t id);
    bool emptyDatabase();
    
    // Métodos de controle
    void updateAuthProcess();
    void updateEnrollProcess();
    void update();
    
    // Getters
    int getTemplateCount();

private:
    SoftwareSerial _serial;
    Adafruit_Fingerprint _finger;
    DisplayManager* _display;
    FingerprintState _currentState = FINGERPRINT_IDLE;
    AuthState _currentAuthState = AUTH_IDLE;
    EnrollState _currentEnrollState = ENROLL_IDLE;
    int _count;
    uint8_t _lockPin;
    uint8_t _enrollId = 0;
    MqttPublish _mqttPublish;

    void sendMqttMessage(const char* topic, const int16_t id, const bool status);
};

#endif // FINGERPRINT_MANAGER_H