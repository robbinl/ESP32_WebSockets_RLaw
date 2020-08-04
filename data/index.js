var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
    initButton();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection to...');
    console.log(gateway);
    websocket = new WebSocket(gateway);
    websocket.onopen  = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    console.log(`Received a message from ${event.origin}`);
    console.log(event);
    document.getElementById('led').className = event.data;
}

function initButton() {
    document.getElementById('toggle').addEventListener('click', onToggle);
}

function onToggle(event) {
    console.log(`Sending the toggle message to esp32`);
    websocket.send('toggle');
}