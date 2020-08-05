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
    //document.getElementById('led').className = event.data;
    let data = JSON.parse(event.data);
    console.log(data);
    document.getElementById('led').className = data.led1status;
    document.getElementById('led2').className = data.led2status;
}

function initButton() {
    document.getElementById('toggle').addEventListener('click', onToggle);
    document.getElementById('button2').addEventListener('mousedown', mouseDown);
    document.getElementById('button2').addEventListener('mouseup', mouseUp);
}

function onToggle(event) {
    console.log(`Sending the toggle message to esp32`);
    //websocket.send('toggle');
    websocket.send(JSON.stringify({'action':'toggle', 'button2status' : 'none'}));
}

function mouseDown(event) {
    console.log(`Sending the mousedown message to esp32`);
    websocket.send(JSON.stringify({'action':'none', 'button2status' : 'down'}));
}

function mouseUp(event) {
    console.log(`Sending the mouseup message to esp32`);
    websocket.send(JSON.stringify({'action':'none', 'button2status' : 'up'}));
}