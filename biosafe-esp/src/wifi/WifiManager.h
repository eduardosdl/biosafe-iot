#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <ESP8266WiFi.h>
#include <Ticker.h>
#include "../display/DisplayManager.h"

typedef void (*ConnectCallback)();

class WifiManager {
public:
    // Construtor
    WifiManager(DisplayManager* display);
    
    // Métodos
    void begin(const char* ssid, const char* password);
    bool isConnected();
    void reconnect();
    void setConnectCallback(ConnectCallback callback);
    
private:
    // Variáveis privadas
    DisplayManager* _display;
    const char* _ssid;
    const char* _password;
    WiFiEventHandler _connectHandler;
    WiFiEventHandler _disconnectHandler;
    Ticker _reconnectTimer;
    ConnectCallback _onConnectCallback;
    
    // Métodos privados
    void onWifiConnect(const WiFiEventStationModeGotIP& event);
    void onWifiDisconnect(const WiFiEventStationModeDisconnected& event);
};

#endif