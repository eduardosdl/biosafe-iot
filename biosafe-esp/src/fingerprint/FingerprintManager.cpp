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

// inicia o processo de autenticação
void FingerprintManager::startAuth() {
    _currentState = FINGERPRINT_AUTH;
    _currentAuthState = AUTH_FINGER_WAIT;
    _currentEnrollState = ENROLL_IDLE;
}

// Inicia o processo de cadastro de impressão digital
void FingerprintManager::startEnroll() {
    _enrollId = _finger.templateCount + 1;
    _currentState = FINGERPRINT_ENROLL;
    _currentAuthState = AUTH_IDLE;
    _currentEnrollState = ENROLL_FIRST_CAPTURE;
    _display->showMessage("Posicione o dedo para cadastrar...", 50);
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

// Atualiza o estado do módulo
void FingerprintManager::update() {
    switch (_currentState) {
        case FINGERPRINT_IDLE:
            break;
        case FINGERPRINT_AUTH:
            updateAuthProcess();
            break;
        case FINGERPRINT_ENROLL:
            updateEnrollProcess();
            break;
    }
}

// Atualiza o processo de autenticação
void FingerprintManager::updateAuthProcess() {
    switch (_currentAuthState) {
        case AUTH_IDLE:
            // não realiza nada
            break;
        case AUTH_FINGER_WAIT:
            // Aguarda o dedo ser colocado no sensor
            if (_finger.getImage() == FINGERPRINT_OK) {
                _currentAuthState = AUTH_FINGER_PROCESS;
            }
            break;

        case AUTH_FINGER_PROCESS:
            // Processa a imagem da digital
            if (_finger.image2Tz() == FINGERPRINT_OK) {
                _currentAuthState = AUTH_IDENTIFY;
            } else {
                _currentAuthState = AUTH_FAILED;
            }
            break;

        case AUTH_IDENTIFY:
            // Tenta identificar a digital
            if (_finger.fingerSearch() == FINGERPRINT_OK) {
                int fingerId = _finger.fingerID;

                _display->setBackgroundColor(0x05E3);
                _display->setTextColor(ST7735_BLACK);
                _display->showMessage("Processando...", 50, 2, false);

                sendMqttMessage("auth/init", fingerId, true);
                _currentAuthState = AUTH_IDLE;
            } else {
                _currentAuthState = AUTH_FAILED;
            }
            break;

        case AUTH_FAILED:
            // Falha na autenticação
            _display->setBackgroundColor(0xe8a3);
            _display->setTextColor(ST7735_WHITE);
            _display->showMessage("Sem reconhecimento", 45, 2, false);
            _display->showMessage("Tente novamente.", 55, 2, false);
            sendMqttMessage("auth/init", 0, false);
            delay(3000);
            showModuleInfo();
            break;

        default:
            break;
    }
}

// Atualiza o processo de cadastro
void FingerprintManager::updateEnrollProcess() {
    switch (_currentEnrollState) {
        case ENROLL_IDLE:
            // não realiza nada
            break;
        case ENROLL_FIRST_CAPTURE:
            // Aguarda o dedo ser colocado no sensor
            if (_finger.getImage() == FINGERPRINT_OK) {
                _finger.image2Tz(1);
                _display->showMessage("Remova o dedo...", 50);
                _currentEnrollState = ENROLL_FIRST_REMOVE;
            }
            break;

        case ENROLL_FIRST_REMOVE:
            // Aguarda o dedo ser removido do sensor
            if (_finger.getImage() == FINGERPRINT_NOFINGER) {
                _display->showMessage("Coloque o dedo novamente...", 50);
                _currentEnrollState = ENROLL_SECOND_CAPTURE;
            }
            break;

        case ENROLL_SECOND_CAPTURE:
            // Aguarda o dedo ser colocado novamente no sensor
            if (_finger.getImage() == FINGERPRINT_OK) {
                _finger.image2Tz(2);
                _display->showMessage("Remova o dedo...", 50);
                _currentEnrollState = ENROLL_SECOND_REMOVE;
            }
            break;

        case ENROLL_SECOND_REMOVE:
            // Aguarda o dedo ser removido do sensor
            if (_finger.getImage() == FINGERPRINT_NOFINGER) {
                _display->showMessage("Criando modelo...", 50);
                _currentEnrollState = ENROLL_CREATE_MODEL;
            }
            break;

        case ENROLL_CREATE_MODEL:
            // Cria o modelo da digital
            if (_finger.createModel() == FINGERPRINT_OK) {
                _currentEnrollState = ENROLL_STORE_MODEL;
            } else {
                _currentEnrollState = ENROLL_FAILED;
            }
            break;

        case ENROLL_STORE_MODEL:
            // Armazena o modelo da digital
            if (_finger.storeModel(_enrollId) == FINGERPRINT_OK) {
                _display->setBackgroundColor(0x05E3);
                _display->setTextColor(ST7735_BLACK);
                _display->showMessage("Cadastro completo", 50, 2, false);               
                delay(2000);
                sendMqttMessage("register/status", _enrollId, true);
                showModuleInfo();
            } else {
                _currentEnrollState = ENROLL_FAILED;
            }
            break;

        case ENROLL_FAILED:
            // Falha no cadastro
            _display->setBackgroundColor(0xbbcf);
            _display->setTextColor(ST7735_WHITE);
            _display->showMessage("Falha ao cadastrar.", 45, 2, false);
            _display->showMessage("Solicite o cadastro novamente.", 55, 2, false);
            sendMqttMessage("register/status", _enrollId, false);
            delay(2000);
            showModuleInfo();
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