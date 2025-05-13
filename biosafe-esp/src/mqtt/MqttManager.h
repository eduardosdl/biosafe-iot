#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <AsyncMqttClient.h>
#include <Ticker.h>
#include <ESP8266WiFi.h>
#include "../display/DisplayManager.h"
#include "../fingerprint/FingerprintManager.h"
#include "../lock/LockManager.h"

class MqttManager {
public:
    // Construtor
    MqttManager(DisplayManager* display, FingerprintManager* fingerprint, LockManager* lock);
    
    // Métodos
    void begin(IPAddress host, uint16_t port, const char* username, const char* password);
    void connect();
    void subscribeTopics();
    void publishData(const char* topic, const char* data);
    
    // Getters
    bool isConnected() const;
    
private:
    // Callbacks
    void onConnect(bool sessionPresent);
    void onDisconnect(AsyncMqttClientDisconnectReason reason);
    void onSubscribe(uint16_t packetId, uint8_t qos);
    void onUnsubscribe(uint16_t packetId);
    void onMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
    void onPublish(uint16_t packetId);
    
    // Variáveis privadas
    AsyncMqttClient _mqttClient;
    Ticker _mqttReconnectTimer;
    DisplayManager* _display;
    FingerprintManager* _fingerprint;
    LockManager* _lock;
    bool _connected;
};

#endif // MQTT_MANAGER_H