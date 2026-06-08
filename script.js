// Centraliza a atualização visual
function updateUI(data) {
    const bpmEl = document.getElementById('bpm-valor');
    const statusEl = document.getElementById('status-msg');
    const statusTag = document.getElementById('status-tag');
    
    // Atualiza BPM
    bpmEl.innerText = data.bpm;
    bpmEl.style.color = (data.bpm > 120 || data.bpm < 50) ? "#ff4c4c" : "#00ff88";

    // Atualiza Status de Segurança
    if (data.queda || data.sos) {
        statusEl.innerText = data.queda ? "🚨 QUEDA DETECTADA" : "🆘 SOS ATIVADO";
        statusEl.parentElement.classList.add('alerta-ativo');
        statusTag.innerText = "ALERTA CRÍTICO";
    } else {
        statusEl.innerText = "Paciente Estável";
        statusEl.parentElement.classList.remove('alerta-ativo');
        statusTag.innerText = "SISTEMA ONLINE";
    }
}

// Simulação de dados
function injectData(bpm, queda, sos) {
    updateUI({ bpm, queda, sos });
}

// Fetch real
setInterval(() => {
    fetch('/dados')
        .then(res => res.json())
        .then(data => updateUI(data))
        .catch(() => console.log("Aguardando hardware..."));
}, 1000);