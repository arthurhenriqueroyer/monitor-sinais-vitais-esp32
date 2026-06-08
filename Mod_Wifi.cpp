#include "Mod_Wifi.h"

void inicializarWifi() {
    Serial.print("[REDE] Conectando ao Wi-Fi: ");
    Serial.println(WIFI_SSID);
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\n[REDE] Wi-Fi Conectado com sucesso!");
    Serial.print("[REDE] IP: ");
    Serial.println(WiFi.localIP());
}

void verificarConexaoWifi() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("[REDE] Conexão perdida. Tentando reconectar...");
        WiFi.disconnect();
        WiFi.reconnect();
    }
}