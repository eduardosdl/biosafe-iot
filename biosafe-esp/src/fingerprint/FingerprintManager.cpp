#include "FingerprintManager.h"
#include <ArduinoJson.h>

#define lockPin 16 // D0

// Construtor
FingerprintManager::FingerprintManager(uint8_t rx_pin, uint8_t tx_pin, DisplayManager* display) 
    : _serial(rx_pin, tx_pin), _finger(&_serial), _display(display), _count(0), _lockPin(lockPin) {
}

// Inicialização do módulo
bool FingerprintManager::begin(uint32_t baudrate) {
    _finger.begin(baudrate);
    
    _display->showMessage("Conectando sensor de impressão digital...");
    delay(100);
    
    if (_finger.verifyPassword()) {
        _display->showMessage("Sensor de impressão digital detectado!");
        pinMode(_lockPin, OUTPUT);
        digitalWrite(_lockPin, LOW);
        return true;
    } else {
        _display->showMessage("Sensor NÃO encontrado!");
        return false;
    }
}

// Exibe informações sobre o módulo
void FingerprintManager::showModuleInfo() {
    _finger.getTemplateCount();
    _count = _finger.templateCount;
    String infoMessage = "Temos " + String(_count) + " digitais amazenadas.";
    _display->showMessage(infoMessage.c_str());
    _display->showMessage("Coloque o dedo no sensor para abrir a fechadura", 50, 1, false);
    startAuth();
}

// Define o callback para publicação no MQTT
void FingerprintManager::setMqttPublish(MqttPublish callback) {
    _mqttPublish = callback;
}

void FingerprintManager::startAuth() {
    _currentAuthState = AUTH_WAIT_FINGER;
}

// Inicia o processo de cadastro de impressão digital
void FingerprintManager::startEnroll() {
    _enrollId = _finger.templateCount + 1; // Incrementa o ID para o novo cadastro
    _currentEnrollState = WAIT_FIRST_FINGER;
    _currentAuthState = AUTH_IDLE;
    _display->showMessage("Posicione o dedo...", 50);
}

// Exclui uma impressão digital pelo ID
bool FingerprintManager::deleteFingerprint(uint16_t id) {
    uint8_t p = _finger.deleteModel(id);
    
    if (p == FINGERPRINT_OK) {
        _display->showMessage("Digital excluída com sucesso!");
        _count--; // Atualiza o contador local
        return true;
    }
    
    _display->showMessage("Falha ao excluir a digital.");
    return false;
}

// Limpa todo o banco de impressões digitais
bool FingerprintManager::emptyDatabase() {
    uint8_t p = _finger.emptyDatabase();
    
    if (p == FINGERPRINT_OK) {
        _display->showMessage("Todas as digitais foram excluídas.");
        _count = 0; // Zera o contador local
        return true;
    }
    
    _display->showMessage("Falha ao limpar o banco de digitais.");
    return false;
}

// Retorna o número de templates armazenados
int FingerprintManager::getTemplateCount() {
    _finger.getTemplateCount();
    _count = _finger.templateCount;
    return _count;
}

// Retorna se o módulo está em processo de cadastro
bool FingerprintManager::isEnrolling() {
    return _currentEnrollState != IDLE;
}

void FingerprintManager::updateAuthProcess() {

    if (_currentAuthState == AUTH_IDLE) return;
    
    switch (_currentAuthState) {
        case AUTH_WAIT_FINGER:
            if (_finger.getImage() == FINGERPRINT_OK) {
                _currentAuthState = AUTH_PROCESS_IMAGE;
            }
            break;

        case AUTH_PROCESS_IMAGE:
            if (_finger.image2Tz() == FINGERPRINT_OK) {
                _currentAuthState = AUTH_IDENTIFY;
            } else {
                _currentAuthState = AUTH_FAILED;
            }
            break;

        case AUTH_IDENTIFY:
            if (_finger.fingerSearch() == FINGERPRINT_OK) {
                int fingerId = _finger.fingerID;

                _display->setBackgroundColor(0x05E3);
                _display->setTextColor(ST7735_BLACK);
                _display->showMessage("Processando...", 50, 2, false);

                sendMqttMessage("auth/init", fingerId, true);
                delay(1000);
                _currentAuthState = AUTH_IDLE;
            } else {
                Serial.println("Digital não reconhecida.");
                delay(2000);
                _currentAuthState = AUTH_FAILED;
            }
            break;

        case AUTH_FAILED:
            Serial.println("Auth failed.");
            _currentAuthState = AUTH_WAIT_FINGER;
            break;
        default:
            break;
    }
}

// Atualiza o processo de cadastro de impressão digital
void FingerprintManager::updateEnrollProcess() {
    if (_currentEnrollState == IDLE) return;

    switch (_currentEnrollState) {
        case WAIT_FIRST_FINGER:
            if (_finger.getImage() == FINGERPRINT_OK) {
                _finger.image2Tz(1);
                _display->showMessage("Remova o dedo...", 50);
                _currentEnrollState = WAIT_REMOVE_FINGER;
            }
            break;

        case WAIT_REMOVE_FINGER:
            if (_finger.getImage() == FINGERPRINT_NOFINGER) {
                _display->showMessage("Coloque o dedo novamente...", 50);
                _currentEnrollState = WAIT_SECOND_FINGER;
            }
            break;

        case WAIT_SECOND_FINGER:
            if (_finger.getImage() == FINGERPRINT_OK) {
                _finger.image2Tz(2);
                _display->showMessage("Criando modelo...", 50);
                _currentEnrollState = CREATE_MODEL;
            }
            break;

        case CREATE_MODEL:
            if (_finger.createModel() == FINGERPRINT_OK) {
                _currentEnrollState = STORE_MODEL;
            } else {
                _currentEnrollState = FAILED;
            }
            break;

        case STORE_MODEL:
            if (_finger.storeModel(_enrollId) == FINGERPRINT_OK) {
                _display->showMessage("Cadastro completo!", 50);                
                delay(1000);
                sendMqttMessage("fingerprint/register/status", _enrollId, true);
                showModuleInfo();
                _currentEnrollState = IDLE;
            } else {
                _currentEnrollState = FAILED;
            }
            break;
            
        case FAILED:
            _display->showMessage("Erro ao salvar.", 50);
            delay(1000);
            sendMqttMessage("fingerprint/register/status", _enrollId, false);
            showModuleInfo();
            _currentEnrollState = IDLE;
            break;

        default:
            break;
    }
}

void FingerprintManager::sendMqttMessage(const char* topic, const int16_t id, const bool status) {
    if (_mqttPublish) {
        JsonDocument doc;
        doc["status"] = status ? "success" : "error";
        doc["id"] = id;
        String buf;
        serializeJson(doc, buf);
        _mqttPublish(topic, buf.c_str());
    }
}