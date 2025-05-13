#include "WifiManager.h"

// Construtor
WifiManager::WifiManager(DisplayManager* display) 
    : _display(display), _ssid(nullptr), _password(nullptr), _onConnectCallback(nullptr) {
}

// Inicializar a configuração WiFi
void WifiManager::begin(const char* ssid, const char* password) {
    _ssid = ssid;
    _password = password;
    
    // Configurar handlers de eventos
    _connectHandler = WiFi.onStationModeGotIP(
        [this](const WiFiEventStationModeGotIP& event) {
            this->onWifiConnect(event);
        }
    );
    
    _disconnectHandler = WiFi.onStationModeDisconnected(
        [this](const WiFiEventStationModeDisconnected& event) {
            this->onWifiDisconnect(event);
        }
    );
    
    _display->showMessage("WiFi configurado");
    reconnect();
}

// Verificar se está conectado ao WiFi
bool WifiManager::isConnected() {
    return WiFi.isConnected();
}

// Tentar reconectar ao WiFi
void WifiManager::reconnect() {
    _display->showMessage("Conectando ao Wi-Fi...");
    WiFi.begin(_ssid, _password);
}

// Definir callback para quando conectar ao WiFi
void WifiManager::setConnectCallback(ConnectCallback callback) {
    _onConnectCallback = callback;
}

// Gerencia o evento de conexão ao WiFi
void WifiManager::onWifiConnect(const WiFiEventStationModeGotIP& event) {
    _display->showMessage("Conectado ao Wi-Fi.");
    
    if (_onConnectCallback) {
        _onConnectCallback();
    }
}

// Gerencia o evento de desconexão do WiFi
void WifiManager::onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
    _display->showMessage("Desconectado do Wi-Fi.", 50, 0);
    _reconnectTimer.once(2, [this]() { this->reconnect(); });
}