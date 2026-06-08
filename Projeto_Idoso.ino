#include "Config.h"
#include "Mod_Wifi.h"
#include "Mod_Notificacao.h"

// Controle de tempo para evitar o envio massivo de mensagens seguidas
unsigned long tempoUltimoAlerta = 0;
const unsigned long INTERVALO_SEGURANCA = 60000; // Bloqueia novos alertas por 1 minuto

void setup() {
    // Inicializa o canal de comunicação serial para depuração no VS Code
    Serial.begin(115200);
    Serial.println("\n=== INICIALIZANDO MONITOR DE SAÚDE DO IDOSO ===");

    // Inicializa cada subsistema isoladamente
    inicializarWifi();
    inicializarNotificacoes();

    Serial.println("=== SISTEMA PRONTO E MONITORANDO ===");
}

void loop() {
    // Garante a resiliência da rede a cada ciclo
    verificarConexaoWifi();

    // VARIÁVEIS DE TESTE (Nas próximas seções vêm os cálculos dos sensores)
    bool quedaDetectada = false; // Mude para true para simular o comportamento
    
    if (quedaDetectada) {
        // Verifica se o intervalo de proteção de tempo já passou
        if (millis() - tempoUltimoAlerta > INTERVALO_SEGURANCA) {
            
            String textoAlerta = "🚨 *ALERTA CRÍTICO: IMPACTO DETECTADO!* 🚨\n\n";
            textoAlerta += "O dispositivo detectou um movimento brusco compatível com queda.\n";
            textoAlerta += "Por favor, entre em contato ou envie socorro imediatamente.";

            // Executa o envio através do módulo especializado
            if (enviarAlertaUrgente(textoAlerta)) {
                tempoUltimoAlerta = millis(); // Reseta o temporizador de segurança
            }
        }
    }

    delay(200); // Ritmo de varredura do loop principal (200 milissegundos)
}