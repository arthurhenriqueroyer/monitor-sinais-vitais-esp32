#include "Mod_SinaisVitais.h"

MAX30105 sensorCardiaco;

const byte TAXA_MEDIA = 4;
byte taxas[TAXA_MEDIA];
byte taxaAtual = 0;
long ultimoBatimento = 0;
float batimentosPorMinuto = 0;
int batimentoMedio = 0;

void inicializarSinaisVitais() {
    if (!sensorCardiaco.begin(Wire, I2C_SPEED_FAST)) {
        Serial.println("[SAÚDE] MAX30102 não encontrado no I2C.");
    } else {
        sensorCardiaco.setup(); 
        sensorCardiaco.setPulseAmplitudeRed(0x0A); 
        sensorCardiaco.setPulseAmplitudeGreen(0);  
        Serial.println("[SAÚDE] MAX30102 inicializado.");
    }
}

void lerSinaisVitais() {
    long valorIR = sensorCardiaco.getIR();

    if (valorIR < 50000) {
        batimentoMedio = 0; // Sem dedo no sensor
        return; 
    }

    if (checkForBeat(valorIR) == true) {
        long delta = millis() - ultimoBatimento;
        ultimoBatimento = millis();
        batimentosPorMinuto = 60 / (delta / 1000.0);

        if (batimentosPorMinuto < 255 && batimentosPorMinuto > 20) {
            taxas[taxaAtual++] = (byte)batimentosPorMinuto;
            taxaAtual %= TAXA_MEDIA; 

            batimentoMedio = 0;
            for (byte i = 0 ; i < TAXA_MEDIA ; i++) {
                batimentoMedio += taxas[i];
            }
            batimentoMedio /= TAXA_MEDIA;
        }
    }
}

bool alertaFrequenciaCardiaca() {
    if (batimentoMedio == 0) return false;
    // Dispara se < 40 ou > 140 BPM
    if (batimentoMedio > 140 || batimentoMedio < 40) return true;
    return false;
}