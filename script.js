/**
 * Painel de Monitoramento Médico - Core Lógico
 * Faz requisições assíncronas assinaladas à API Rest do ESP32 (/dados)
 */

const INTERVALO_ATUALIZACAO = 1000; // Tempo em milissegundos (1 segundo)

function atualizarPainelMedico() {
    fetch('/dados')
        .then(response => {
            if (!response.ok) {
                throw new Error("Erro na resposta do servidor.");
            }
            return response.json();
        })
        .then(data => {
            // 1. Atualizar e consolidar o estado ONLINE
            const statusConexao = document.getElementById('status-conexao');
            statusConexao.innerText = "● ONLINE (Sincronizado)";
            statusConexao.className = "badge-status online";

            // 2. Processar e Renderizar os Batimentos Cardíacos
            const bpmValor = document.getElementById('bpm-valor');
            
            if (data.bpm === 0) {
                bpmValor.innerHTML = `<span class="placeholder">Aguardando leitura estável...</span>`;
            } else {
                bpmValor.innerHTML = `${data.bpm} <span class="unidade">BPM</span> <span class="coracao">❤️</span>`;
                
                // Análise de Limites Clínicos (Taquicardia ou Bradicardia)
                if (data.bpm > 120 || data.bpm < 50) {
                    bpmValor.style.color = "#ff4c4c"; // Vermelho Alerta
                } else {
                    bpmValor.style.color = "#00ff88"; // Verde Clínico Normal
                }
            }

            // 3. Triagem de Alertas e Segurança de Fundo
            const corpoPagina = document.body;
            const containerMensagem = document.getElementById('mensagem');
            
            if (data.queda || data.sos) {
                corpoPagina.style.backgroundColor = "#2a0808"; // Background de emergência visual
                
                if (data.queda && data.sos) {
                    containerMensagem.innerHTML = "🚨 INTERVENÇÃO IMEDIATA: QUEDA E SOS ATIVADOS!";
                } else if (data.queda) {
                    containerMensagem.innerHTML = "🚨 ALERTA CRÍTICO: QUEDA DETECTADA DO PACIENTE!";
                } else if (data.sos) {
                    containerMensagem.innerHTML = "🆘 CHAMADO DE URGÊNCIA: BOTÃO DE PÂNICO ACIONADO!";
                }
                
                containerMensagem.className = "card alerta";
            } else {
                // Estado estável padrão
                corpoPagina.style.backgroundColor = "#12121e";
                containerMensagem.innerHTML = "✅ Paciente Clínicamente Estável";
                containerMensagem.className = "card status-estavel";
            }
        })
        .catch(error => {
            // Mecanismo de Proteção ativa contra quedas de sinal (Fail-Safe)
            const statusConexao = document.getElementById('status-conexao');
            const containerMensagem = document.getElementById('mensagem');
            const bpmValor = document.getElementById('bpm-valor');

            statusConexao.innerText = "● OFFLINE (Sinal Interrompido)";
            statusConexao.className = "badge-status offline";
            
            containerMensagem.innerHTML = "⚠️ Sistema de Monitoramento Fora do Ar! Verifique o Dispositivo.";
            containerMensagem.className = "card";
            
            bpmValor.innerHTML = `<span class="placeholder" style="color: #ff4c4c;">Sem Sinal</span>`;
            document.body.style.backgroundColor = "#181818";
        });
}

// Inicializa o Loop de execução contínua
setInterval(atualizarPainelMedico, INTERVALO_ATUALIZACAO);