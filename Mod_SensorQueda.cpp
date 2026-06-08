#include "Mod_SensorQueda.h"

Adafruit_MPU6050 mpu;
bool emQuedaLivre = false;
unsigned long tempoQuedaLivre = 0;

void inicializarSensorQueda() {
    if (!mpu.begin()) {
        Serial.println("[SENSOR] MPU6050 não encontrado! Verifique SDA(21) e SCL(22).");
    } else {
        mpu.setAccelerometerRange(MPU6050_RANGE_16_G);
        Serial.println("[SENSOR] MPU6050 calibrado (Até 16G).");
    }
}

bool verificarQueda() {
    sensors_event_t a, g, temp;
    if (!mpu.getEvent(&a, &g, &temp)) return false;

    // Converte para Força G
    float ax = a.acceleration.x / 9.81;
    float ay = a.acceleration.y / 9.81;
    float az = a.acceleration.z / 9.81;

    // Magnitude do Vetor 3D
    float magnitude = sqrt(pow(ax, 2) + pow(ay, 2) + pow(az, 2));

    // Estágio 1: Queda Livre
    if (magnitude < 0.4) {
        emQuedaLivre = true;
        tempoQuedaLivre = millis();
    }

    // Estágio 2: Impacto
    if (emQuedaLivre) {
        if (magnitude > 2.5) {
            emQuedaLivre = false;
            return true; // Queda confirmada
        }
        // Timeout: Se passou 1 segundo sem impacto, foi alarme falso
        if (millis() - tempoQuedaLivre > 1000) {
            emQuedaLivre = false;
        }
    }
    return false;
}