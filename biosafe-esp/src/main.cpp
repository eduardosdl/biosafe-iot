
// bibliotecas modulares
#include "display/DisplayManager.h"
#include "fingerprint/FingerprintManager.h"
#include "wifi/WifiManager.h"
#include "mqtt/MqttManager.h"

// DEFINIÇÃO DE CONSTANTES
// - wifi
// #define WIFI_SSID "SANTOS NEW LINK "
// #define WIFI_PASSWORD "20042006"
#define WIFI_SSID "natan"
#define WIFI_PASSWORD "12345678"
// #define WIFI_SSID "Edla_2.4"
// #define WIFI_PASSWORD "13052203"
// - mqtt
// #define MQTT_HOST IPAddress(192, 168, 1, 7)
#define MQTT_HOST IPAddress(192, 168, 122, 64)
#define MQTT_PORT 1883
#define BROKER_USER "edu"
#define BROKER_PASS "eduardo"

// Definição dos pinos
// - display
#define TFT_CS   15  // Chip Select
#define TFT_RST  4   // Reset
#define TFT_DC   5   // Data/Command
// - digital
#define Finger_Rx 0  // D3
#define Finger_Tx 2  // D4
// - gerais
#define lockPin 16   // D0
#define closeBtnPin 10 // SD3

// INICIALIZAÇÃO DOS MODULOS
// - display tft
DisplayManager display(TFT_CS, TFT_DC, TFT_RST);
// - fingerprint
FingerprintManager fingerprint(Finger_Rx, Finger_Tx, &display);
// - lock
LockManager lock(lockPin, closeBtnPin, &display, &fingerprint);
// - wifi
WifiManager wifi(&display);
// - mqtt
MqttManager mqtt(&display, &fingerprint, &lock);

// variaveis de controle de loop
volatile bool fingerprintEnroll = false;
volatile bool requestOpen = false;
volatile bool requestClosed = false;

// Callbacks
void connectToMqtt() {
    mqtt.connect();
}

void sendMqttMessage (const char* topic, const char* payload) {
    if (mqtt.isConnected()) {
        mqtt.publishData(topic, payload);
    }
}

void requestFingerprintEnroll() {
    fingerprintEnroll = true;
}

void requestFingerprintOpen() {
    requestOpen = true;
}

void requestFingerprintClosed() {
    requestClosed = true;
}

void setup() {
    Serial.begin(115200); 
    
    // inicialização do display
    display.begin();
    display.showMessage("Display configurado");

    // Inicializa o sensor de impressão digital
    if (!fingerprint.begin(57600)) {
        display.showMessage("Sensor NÃO encontrado! Reiniciando em 5s...");
        delay(5000);
        ESP.restart();
    }

    // Inicializa a fechadura
    lock.begin();
    lock.setMqttPublish(sendMqttMessage);
    
    // Inicializa o MQTT
    mqtt.begin(MQTT_HOST, MQTT_PORT, BROKER_USER, BROKER_PASS);
    
    // injeção de callbacks
    mqtt.setRequestEnroll(requestFingerprintEnroll);
    mqtt.setRequestOpen(requestFingerprintOpen);
    mqtt.setRequestClosed(requestFingerprintClosed);
    fingerprint.setMqttPublish(sendMqttMessage);

    // Configuração e início do WiFi
    wifi.begin(WIFI_SSID, WIFI_PASSWORD);
    wifi.setConnectCallback(connectToMqtt);
}

void loop() {
    lock.update();

    if (fingerprintEnroll) {
        fingerprint.startEnroll();
        fingerprintEnroll = false;
    }

    if (requestOpen) {
        if (!lock.isOpen()) {
            Serial.println("Requisição de abertura recebida");
            fingerprint.stopAuth();
            lock.unlock();
        }
        requestOpen = false;
    }

    if (requestClosed) {
        if (lock.isOpen()) {
            Serial.println("Requisição de fechamento recebida");
            lock.lock();
            fingerprint.showModuleInfo();
        }
        requestClosed = false;
    }

    fingerprint.update();
}
