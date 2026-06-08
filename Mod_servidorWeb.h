#ifndef MOD_SERVIDORWEB_H
#define MOD_SERVIDORWEB_H

#include <WebServer.h>

void inicializarServidorWeb();
void manterServidorWeb();
void atualizarDadosWeb(int bpm, bool queda, bool sos);

#endif