#ifndef MOD_SENSORQUEDA_H
#define MOD_SENSORQUEDA_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

void inicializarSensorQueda();
bool verificarQueda();

#endif