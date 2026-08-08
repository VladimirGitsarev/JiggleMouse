const SERIAL_BAUD_RATE = 115200;
const FETCH_TIMEOUT_MS = 1500;
const CMD_START = 'START';
const CMD_STOP = 'STOP';
const CMD_GET_ALL = 'GET_ALL';
const CMD_SET_BOOT = 'SET_START_ON_BOOT:';
const MSG_FWD_SPEED = 'FWD Speed: ';
const MSG_REV_SPEED = 'REV Speed: ';
const MSG_SPIN_TIME = 'Spin Time: ';
const MSG_PAUSE_TIME = 'Pause Time: ';
const MSG_START_BOOT = 'Start On Boot: ';
const MSG_SERVO_PIN = 'Servo Pin: D';
const STR_TRUE = 'TRUE';
const STR_MS = 'ms';

let port, writer, reader;
let readPromise, writePromise;

const btnConnect = document.getElementById('btnConnect');
const btnStart = document.getElementById('btnStart');
const btnStop = document.getElementById('btnStop');
const btnGet = document.getElementById('btnGet');
const btnSendBoot = document.getElementById('btnSendBoot');
const statusText = document.getElementById('status');
const terminal = document.getElementById('terminal');
const valBoot = document.getElementById('valBoot');
const actionButtons = document.querySelectorAll('.panel-control button:not(#btnConnect)');
const sendButtons = document.querySelectorAll('.btnSend');

function logToTerminal(msg) {
  terminal.innerHTML += `<div>${msg}</div>`;
  terminal.scrollTop = terminal.scrollHeight;
}

function toggleButtons(enable) {
  actionButtons.forEach(btn => btn.disabled = !enable);
}

function syncInputs(slideId, numId) {
  const slide = document.getElementById(slideId);
  const num = document.getElementById(numId);
  
  slide.addEventListener('input', () => num.value = slide.value);
  num.addEventListener('input', () => slide.value = num.value);
}

function updateUI(numId, slideId, value) {
  const num = document.getElementById(numId);
  const slide = document.getElementById(slideId);
  if (num) num.value = value;
  if (slide) slide.value = value;
}

function parseIncomingSettings(line) {
  if (line.startsWith(MSG_FWD_SPEED)) {
    updateUI('valFwd', 'slideFwd', line.replace(MSG_FWD_SPEED, "").trim());
  } 
  else if (line.startsWith(MSG_REV_SPEED)) {
    updateUI('valRev', 'slideRev', line.replace(MSG_REV_SPEED, "").trim());
  } 
  else if (line.startsWith(MSG_SPIN_TIME)) {
    updateUI('valSpin', 'slideSpin', line.replace(MSG_SPIN_TIME, "").replace(STR_MS, "").trim());
  } 
  else if (line.startsWith(MSG_PAUSE_TIME)) {
    updateUI('valPause', 'slidePause', line.replace(MSG_PAUSE_TIME, "").replace(STR_MS, "").trim());
  } 
  else if (line.startsWith(MSG_START_BOOT)) {
    valBoot.checked = line.includes(STR_TRUE);
  }
  else if (line.startsWith(MSG_SERVO_PIN)) {
    updateUI('valPin', null, line.replace(MSG_SERVO_PIN, "").trim());
  }
}

async function sendCommand(cmd) {
  if (!writer) return;
  await writer.write(cmd + '\n');
  logToTerminal(`<span class="term-sent">Sent: ${cmd}</span>`);
}

async function readLoop() {
  const textDecoder = new TextDecoderStream();
  readPromise = port.readable.pipeTo(textDecoder.writable);
  reader = textDecoder.readable.getReader();
  
  let buffer = "";

  try {
    while (true) {
      const { value, done } = await reader.read();
      if (done) break;
      if (value) {
        buffer += value;
        let lines = buffer.split('\n');
        buffer = lines.pop(); 
        
        lines.forEach(l => {
          let trimmed = l.trim();
          if(trimmed) {
            logToTerminal(`> ${trimmed}`);
            parseIncomingSettings(trimmed);
          }
        });
      }
    }
  } catch (err) {
    console.error(err);
  } finally {
    reader.releaseLock();
  }
}

async function connectSerial() {
  try {
    if (port) {
      try {
        if (reader) {
          await reader.cancel().catch(() => {});
        }
        if (readPromise) {
          await readPromise.catch(() => {});
        }
        if (writer) {
          await writer.close().catch(() => {});
        }
        if (writePromise) {
          await writePromise.catch(() => {});
        }
        await port.close();
      } catch (err) {
        console.error("Disconnect error:", err);
      }
      
      port = reader = writer = readPromise = writePromise = null;
      
      btnConnect.innerText = 'Connect';
      statusText.innerText = 'Disconnected';
      statusText.classList.remove('connected');
      toggleButtons(false);
      logToTerminal('Disconnected.');
      return;
    }

    port = await navigator.serial.requestPort();
    await port.open({ baudRate: SERIAL_BAUD_RATE });

    statusText.innerText = 'Connected';
    statusText.classList.add('connected');
    btnConnect.innerText = 'Disconnect';
    toggleButtons(true);
    logToTerminal('Connected! Fetching settings...');

    const textEncoder = new TextEncoderStream();
    writePromise = textEncoder.readable.pipeTo(port.writable);
    writer = textEncoder.writable.getWriter();

    readLoop();
    setTimeout(() => sendCommand(CMD_GET_ALL), FETCH_TIMEOUT_MS);

  } catch (err) {
    console.error(err);
    logToTerminal(`<span class="term-err">Error: ${err.message}</span>`);
    if (port && !port.readable) port = null;
  }
}

syncInputs('slideFwd', 'valFwd');
syncInputs('slideRev', 'valRev');
syncInputs('slideSpin', 'valSpin');
syncInputs('slidePause', 'valPause');

btnConnect.addEventListener('click', connectSerial);
btnStart.addEventListener('click', () => sendCommand(CMD_START));
btnStop.addEventListener('click', () => sendCommand(CMD_STOP));
btnGet.addEventListener('click', () => sendCommand(CMD_GET_ALL));

sendButtons.forEach(btn => {
  if (btn.id === "btnSendBoot") return;
  btn.addEventListener('click', (e) => {
    const inputId = e.target.dataset.input;
    const val = document.getElementById(inputId).value;
    sendCommand(e.target.dataset.cmd + val);
  });
});

btnSendBoot.addEventListener('click', () => {
  const isChecked = valBoot.checked;
  const val = isChecked ? "1" : "0";
  sendCommand(CMD_SET_BOOT + val);
});

window.addEventListener('beforeunload', () => {
  if (reader) reader.cancel().catch(() => {});
  if (writer) writer.releaseLock();
  if (port) port.close();
});

const modal = document.getElementById('imageModal');
const modalImg = document.getElementById('modalImage');
const closeModal = document.querySelector('.close-modal');

const images = document.querySelectorAll('.main-photo, .diagram-img, .hw-card img, .step-grid img');

images.forEach(img => {
  img.classList.add('clickable-img');
  
  img.addEventListener('click', () => {
    modal.classList.remove('hidden');
    modalImg.src = img.src;
  });
});

closeModal.addEventListener('click', () => {
  modal.classList.add('hidden');
});

modal.addEventListener('click', (e) => {
  if (e.target === modal) {
    modal.classList.add('hidden');
  }
});

document.addEventListener('keydown', (e) => {
  if (e.key === 'Escape' && !modal.classList.contains('hidden')) {
    modal.classList.add('hidden');
  }
});