// Substitua a função de atualização por esta lógica dual:
function injectData(bpm, queda, sos) {
    // Esta função é chamada pelos botões do Painel de Simulação
    // Vamos processar os dados como se viessem da placa
    processarDados({ bpm: bpm, queda: queda, sos: sos });
}

function processarDados(data) {
    const bpmValor = document.getElementById('bpm-valor');
    const containerMensagem = document.getElementById('mensagem');
    const corpoPagina = document.body;

    // Atualiza BPM
    bpmValor.innerHTML = `${data.bpm} <span class="unidade">BPM</span> <span class="coracao">❤️</span>`;
    bpmValor.style.color = (data.bpm > 120 || data.bpm < 50) ? "#ff4c4c" : "#00ff88";

    // Atualiza Alertas
    if (data.queda || data.sos) {
        corpoPagina.style.backgroundColor = "#2a0808";
        containerMensagem.innerHTML = data.queda ? "🚨 QUEDA DETECTADA!" : "🆘 SOS ATIVADO!";
        containerMensagem.className = "card alerta";
    } else {
        corpoPagina.style.backgroundColor = "#12121e";
        containerMensagem.innerHTML = "✅ Paciente Clínicamente Estável";
        containerMensagem.className = "card status-estavel";
    }
}

// Mantemos o fetch original para quando o ESP32 estiver conectado
setInterval(function() {
    fetch('/dados')
        .then(res => res.json())
        .then(data => processarDados(data))
        .catch(() => console.log("Aguardando conexão com hardware..."));
}, 1000);