#include "MqttManager.h"
#include <ArduinoJson.h>

// Construtor
MqttManager::MqttManager(DisplayManager* display, FingerprintManager* fingerprint, LockManager* lock)
    : _display(display), _fingerprint(fingerprint), _lock(lock), _connected(false) {
}

// Inicializa o cliente MQTT
void MqttManager::begin(IPAddress host, uint16_t port, const char* username, const char* password) {
    // Configurar callbacks do MQTT
    _mqttClient.onConnect([this](bool sessionPresent) {
        this->onConnect(sessionPresent);
    });
    _mqttClient.onDisconnect([this](AsyncMqttClientDisconnectReason reason) {
        this->onDisconnect(reason);
    });
    _mqttClient.onSubscribe([this](uint16_t packetId, uint8_t qos) {
        this->onSubscribe(packetId, qos);
    });
    _mqttClient.onUnsubscribe([this](uint16_t packetId) {
        this->onUnsubscribe(packetId);
    });
    _mqttClient.onMessage([this](char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
        this->onMessage(topic, payload, properties, len, index, total);
    });
    _mqttClient.onPublish([this](uint16_t packetId) {
        this->onPublish(packetId);
    });
    
    _mqttClient.setServer(host, port);
    
    if (username && password) {
        _mqttClient.setCredentials(username, password);
    }
    
    _display->showMessage("MQTT configurado");
}

// Configurar o callback para solicitar o registro
void MqttManager::setRequestEnroll(RequestEnroll requestEnroll) {
    _requestEnroll = requestEnroll;
}

// Configurar o callback para solicitar abertura da fechadura
void MqttManager::setRequestOpen(RequestOpen requestOpen) {
    _requestOpen = requestOpen;
}

// Configurar o callback para solicitar fechamento da fechadura
void MqttManager::setRequestClosed(RequestCloed requestClosed) {
    _requestClosed = requestClosed;
}

// Conectar ao broker MQTT
void MqttManager::connect() {
    _display->showMessage("Conectando ao MQTT...");
    _mqttClient.connect();
}

// Handler para quando conectar ao MQTT
void MqttManager::onConnect(bool sessionPresent) {
    _connected = true;
    _display->showMessage("Conectado ao MQTT");
    
    subscribeTopics();

    _mqttClient.publish("verify", 2, false, "Conectado ao MQTT");
    _fingerprint->showModuleInfo();
}

// Handler para quando desconectar do MQTT
void MqttManager::onDisconnect(AsyncMqttClientDisconnectReason reason) {
    _connected = false;
    _display->showMessage("Desconectado do MQTT");
    
    if (WiFi.isConnected()) {
        _mqttReconnectTimer.once(2, [this]() { this->connect(); });
    }
}

// Handler para confirmação de inscrição
void MqttManager::onSubscribe(uint16_t packetId, uint8_t qos) {
    Serial.println("Inscrição confirmada");
    Serial.print(" packetId: ");
    Serial.println(packetId);
    Serial.print(" qos: ");
    Serial.println(qos);
}

// Handler para confirmação de cancelamento de inscrição
void MqttManager::onUnsubscribe(uint16_t packetId) {
    Serial.println("Cancelamento de inscrição confirmado");
    Serial.print(" packetId: ");
    Serial.println(packetId);
}

// Handler para mensagens recebidas
void MqttManager::onMessage(
    char* topic, char* payload,
    AsyncMqttClientMessageProperties properties,
    size_t len,
    size_t index,
    size_t total
) {
    // deserialização do JSON Recebido
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
        Serial.print("Erro ao analisar JSON: ");
        Serial.println(error.c_str());
        return;
    }
    
    // Verifica o tópico e executa a ação correspondente
    if (strcmp(topic, "register/init") == 0) {
        const char* msg = doc["msg"];

        if (strcmp(msg, "init") == 0) {
            if (_requestEnroll) {
                _requestEnroll();
            }
        }
    } else if (strcmp(topic, "auth/status") == 0) {
        bool status = doc["status"].as<bool>();
        
        if (status) {
            uint8_t userId = doc["userId"].as<uint8_t>();
            const char* username = doc["username"];
            
            _lock->unlock(userId, username);
        } else {
            _display->showMessage("Autenticação falhou");
            delay(2000);
            _fingerprint->showModuleInfo();
        }
    } else if (strcmp(topic, "lock/state/open") == 0) {
        if (_requestOpen) {
            _requestOpen();
        }
    } else if (strcmp(topic, "lock/state/closed") == 0) {
        if (_requestClosed) {
            _requestClosed();
        }
    }
}

// Handler para confirmação de publicação
void MqttManager::onPublish(uint16_t packetId) {
    Serial.println("Publicação confirmada");
    Serial.print(" packetId: ");
    Serial.println(packetId);
}

// Inscrição nos tópicos
void MqttManager::subscribeTopics() {
    _mqttClient.subscribe("register/init", 2);
    _mqttClient.subscribe("auth/status", 2);
    _mqttClient.subscribe("lock/state/open", 2);
    _mqttClient.subscribe("lock/state/closed", 2);
}

// Publicar dados de digital
void MqttManager::publishData(const char* topic, const char* data) {
    Serial.print("Publicando dados: ");
    Serial.println(topic);
    Serial.print(" -> ");
    Serial.println(data);
    _mqttClient.publish(topic, 2, false, data);
}

// Verificar se está conectado
bool MqttManager::isConnected() const {
    return _connected;
}