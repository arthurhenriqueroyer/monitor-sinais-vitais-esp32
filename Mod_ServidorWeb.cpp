#include "Mod_ServidorWeb.h"
#include <WiFi.h>

WebServer servidor(80);

int webBpm = 0;
bool webQueda = false;
bool webSos = false;

const char paginaHTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="pt-BR">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Painel Médico Avançado</title>
    <style>
        /* Estilo Base Premium */
        body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #12121e; color: #fff; text-align: center; margin: 0; padding: 20px; transition: background-color 0.5s; }
        .container { max-width: 600px; margin: auto; background: #1e1e2f; padding: 30px; border-radius: 20px; box-shadow: 0 15px 40px rgba(0,0,0,0.6); border: 1px solid #2a2a40; }
        h1 { color: #00d2ff; font-size: 2em; margin-bottom: 5px; }
        .subtitle { color: #888; font-size: 0.9em; margin-bottom: 30px; }
        
        /* Cartões de Informação */
        .card { background: #2a2a40; margin: 20px 0; padding: 30px; border-radius: 15px; font-size: 1.2em; border: 1px solid #3f3f5a; transition: all 0.3s; position: relative; }
        .valor { font-size: 3.5em; font-weight: bold; color: #00ff88; margin-top: 10px; display: flex; justify-content: center; align-items: center; gap: 15px; }
        
        /* Cores de Alerta e Animações */
        .alerta { background-color: #ff3333 !important; color: white !important; animation: piscar 0.8s infinite alternate; border-color: #ff0000; box-shadow: 0 0 20px rgba(255, 51, 51, 0.5); }
        
        /* O Coração Pulsante */
        .coracao { display: inline-block; color: #ff4c4c; animation: bater 0.8s infinite alternate; }
        @keyframes bater { 0% { transform: scale(1); } 100% { transform: scale(1.25); } }
        @keyframes piscar { 0% { transform: scale(1); } 100% { transform: scale(1.02); } }
        
        /* Indicador de Status (Online/Offline) */
        .badge-status { display: inline-block; padding: 8px 18px; border-radius: 25px; font-size: 0.85em; font-weight: bold; background: #333; color: #aaa; margin-bottom: 20px; letter-spacing: 1px; }
        .online { background: #004d26; color: #00ff88; border: 1px solid #00ff88; }
        .offline { background: #4d0000; color: #ff4c4c; border: 1px solid #ff4c4c; }
    </style>
    <script>
        setInterval(function() {
            // Tenta buscar os dados
            fetch('/dados')
                .then(response => {
                    if(!response.ok) throw new Error("Falha na rede");
                    return response.json();
                })
                .then(data => {
                    // 1. Atualiza Status para ONLINE
                    let statusElem = document.getElementById('status-conexao');
                    statusElem.innerText = "● ONLINE (Sincronizado)";
                    statusElem.className = "badge-status online";

                    // 2. Atualiza os Batimentos e o Coração
                    let bpmElem = document.getElementById('bpm-valor');
                    let iconeCoracao = "<span class='coracao'>❤️</span>";
                    
                    if (data.bpm === 0) {
                        bpmElem.innerHTML = "<span style='color: #888; font-size: 0.6em;'>Aguardando Leitura...</span>";
                    } else {
                        bpmElem.innerHTML = data.bpm + " <span style='font-size: 0.4em; color: #aaa;'>BPM</span> " + iconeCoracao;
                        
                        // Fica vermelho se os batimentos estiverem anormais
                        if (data.bpm > 120 || data.bpm < 50) bpmElem.style.color = "#ff4c4c"; 
                        else bpmElem.style.color = "#00ff88";
                    }

                    // 3. Verifica Alertas e Muda a Cor do Fundo
                    let corpo = document.body;
                    let msg = document.getElementById('mensagem');
                    
                    if (data.queda || data.sos) {
                        corpo.style.backgroundColor = "#330000"; // Fundo vermelho bem escuro
                        if (data.queda && data.sos) msg.innerHTML = "🚨 MÚLTIPLAS EMERGÊNCIAS: QUEDA E SOS!";
                        else if (data.queda) msg.innerHTML = "🚨 ALERTA: QUEDA DETECTADA!";
                        else if (data.sos) msg.innerHTML = "🆘 ALERTA: BOTÃO DE PÂNICO ACIONADO!";
                        msg.className = "card alerta";
                    } else {
                        corpo.style.backgroundColor = "#12121e"; // Fundo azul marinho escuro
                        msg.innerHTML = "✅ Paciente Estável";
                        msg.className = "card";
                    }
                })
                .catch(error => {
                    // SE O ESP32 CAIR OU DESLIGAR, ISSO AQUI ACONTECE:
                    document.getElementById('status-conexao').innerText = "● OFFLINE (Sem Sinal)";
                    document.getElementById('status-conexao').className = "badge-status offline";
                    document.getElementById('mensagem').innerHTML = "⚠️ Atenção: Conexão Perdida com o Aparelho!";
                    document.getElementById('mensagem').className = "card";
                    document.getElementById('bpm-valor').innerHTML = "--";
                    document.body.style.backgroundColor = "#1a1a1a";
                });
        }, 1000);
    </script>
</head>
<body>
    <div class="container">
        <h1>Monitor de Sinais Vitais</h1>
        <div class="subtitle">Sistema Integrado de Telemetria Médica (ESP32)</div>
        
        <div id="status-conexao" class="badge-status">Buscando Sinal...</div>
        
        <div class="card">
            <div style="text-transform: uppercase; letter-spacing: 1px; font-size: 0.8em; color: #aaa;">Frequência Cardíaca Atual</div>
            <div id="bpm-valor" class="valor">
                <span style="color: #888; font-size: 0.6em;">Iniciando Sensores...</span>
            </div>
        </div>

        <div id="mensagem" class="card" style="font-weight: bold; font-size: 1.4em;">
            🔄 Estabelecendo Comunicação...
        </div>
    </div>
</body>
</html>
)rawliteral";

void inicializarServidorWeb() {
    servidor.on("/", []() {
        servidor.send(200, "text/html", paginaHTML);
    });

    servidor.on("/dados", []() {
        String json = "{";
        json += "\"bpm\":" + String(webBpm) + ",";
        json += "\"queda\":" + String(webQueda ? "true" : "false") + ",";
        json += "\"sos\":" + String(webSos ? "true" : "false");
        json += "}";
        servidor.send(200, "application/json", json);
    });

    servidor.begin();
    Serial.println("[WEB] Servidor Web inicializado. Dashboard Online!");
}

void manterServidorWeb() {
    servidor.handleClient();
}

void atualizarDadosWeb(int bpm, bool queda, bool sos) {
    webBpm = bpm;
    webQueda = queda;
    webSos = sos;
}