#ifndef MOD_SINAISVITAIS_H
#define MOD_SINAISVITAIS_H

#include <Wire.h>
#include "MAX30105.h" 
#include "heartRate.h" 

void inicializarSinaisVitais();
void lerSinaisVitais();
bool alertaFrequenciaCardiaca();

#endif