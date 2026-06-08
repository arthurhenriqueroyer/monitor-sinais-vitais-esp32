#include "Config.h"
#include "Mod_Wifi.h"
#include "Mod_SensorQueda.h"
#include "Mod_SinaisVitais.h"
#include "Mod_BotaoSOS.h"
#include "Mod_ServidorWeb.h" // Trocamos a Notificacao pelo Servidor Web

TaskHandle_t TaskRede;
TaskHandle_t TaskSensores;

// Variáveis para passar os dados do Núcleo 1 para o site
extern int batimentoMedio; // Pega o valor do arquivo Mod_SinaisVitais

// --- NÚCLEO 0: INTERNET E SITE ---
void codigoRede(void * parameter) {
    for(;;) {
        verificarConexaoWifi();
        manterServidorWeb(); // Mantém o site no ar respondendo aos navegadores
        vTaskDelay(10 / portTICK_PERIOD_MS); 
    }
}

// --- NÚCLEO 1: SENSORES ---
void codigoSensores(void * parameter) {
    bool estadoQueda = false;
    bool estadoSOS = false;
    unsigned long tempoAlerta = 0;

    for(;;) {
        // 1. Lê o botão SOS
        if (verificarBotaoSOS()) {
            Serial.println("[EMERGÊNCIA] Botão SOS pressionado!");
            estadoSOS = true;
            tempoAlerta = millis();
        }

        // 2. Lê a Queda
        if (verificarQueda()) {
            Serial.println("[EMERGÊNCIA] Queda Detectada!");
            estadoQueda = true;
            tempoAlerta = millis();
        }

        // 3. Lê o Coração
        lerSinaisVitais();

        // 4. Envia os dados atuais para o site
        atualizarDadosWeb(batimentoMedio, estadoQueda, estadoSOS);

        // Desliga o alerta visual do site após 8 segundos
        if ((estadoQueda || estadoSOS) && (millis() - tempoAlerta > 8000)) {
            estadoQueda = false;
            estadoSOS = false;
        }
        
        vTaskDelay(10 / portTICK_PERIOD_MS); 
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("\n[SISTEMA] Iniciando SO Médico (FreeRTOS)...");

    inicializarWifi();
    inicializarSensorQueda();
    inicializarSinaisVitais();
    inicializarBotaoSOS();
    
    // Inicia o site logo após conectar no Wi-Fi
    inicializarServidorWeb(); 

    xTaskCreatePinnedToCore(codigoRede, "TaskRede", 10000, NULL, 1, &TaskRede, 0);
    xTaskCreatePinnedToCore(codigoSensores, "TaskSensores", 10000, NULL, 2, &TaskSensores, 1);
}

void loop() {
    vTaskDelete(NULL); 
}