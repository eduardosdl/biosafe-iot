#include "LockManager.h"

// Construtor
LockManager::LockManager(uint8_t lockPin, uint8_t closeBtnPin, DisplayManager* display, FingerprintManager* fingerprint)
    : _lockPin(lockPin), _closeBtnPin(closeBtnPin), _display(display),
    _fingerprint(fingerprint), _lockState(LOCKED) {}

// Inicialização do módulo
void LockManager::begin() {
    pinMode(_lockPin, OUTPUT);
    digitalWrite(_lockPin, LOW);
    pinMode(_closeBtnPin, INPUT_PULLUP);
    
    _display->showMessage("Fechadura inicializada");
}

// Define o callback para publicação no MQTT
void LockManager::setMqttPublish(LockMqttPublish callback) {
    _mqttPublish = callback;
}

// Destrancar a fechadura
bool LockManager::unlock(uint16_t userId, const char* username) {
    digitalWrite(_lockPin, HIGH);
    
    // Verifica se o relé foi acionado corretamente
    if (digitalRead(_lockPin) == HIGH) {
        _display->setBackgroundColor(0x05E3);
        _display->setTextColor(ST7735_BLACK);

        _display->showMessage("Fechadura destrancada", 15, 1, false);
        _display->showMessage("Nome:", 30, 2, false);
        _display->showMessage(username, 50, 2, false);

        sendMqttStatus(userId, true, true);
        _lockState = UNLOCKED;
        
        return true;
    } else {
        _display->showMessage("ERRO: Falha ao destrancar", 50);
        
        sendMqttStatus(userId, false, false);
        return false;
    }
}

// Trancar a fechadura
bool LockManager::lock() {
    digitalWrite(_lockPin, LOW);
    
    if (digitalRead(_lockPin) == LOW) {
        _display->showMessage("Fechadura trancada", 50, 1);
        
        sendMqttStatus(0, false, true);
        _lockState = LOCKED;
        _fingerprint->showModuleInfo();
        
        return true;
    } else {
        _display->showMessage("ERRO: Falha ao trancar", 50);

        sendMqttStatus(0, true, false);
        return false;
    }
}

// Método para atualizar o estado da fechadura (deve ser chamado no loop)
void LockManager::update() {
    if (_lockState == LOCKED) return;
        
    switch (_lockState) {
        case UNLOCKED:
            if (digitalRead(_closeBtnPin) == HIGH) {
                lock();
            }
            break;
        default:
            break;
    }
}

// Envia mensagem MQTT com o estado da fechadura
void LockManager::sendMqttStatus(uint16_t userId, bool unlocked, bool success) {
    if (_mqttPublish) {
        JsonDocument doc;
        doc["status"] = success ? "success" : "error";
        doc["state"] = unlocked ? "unlocked" : "locked";
        doc["userId"] = unlocked ? userId : 0;
        doc["timestamp"] = millis();
        
        String buf;
        serializeJson(doc, buf);
        _mqttPublish("lock/status", buf.c_str());
    }
}
