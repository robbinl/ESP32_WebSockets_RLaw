// The websocket address has the following signature  ws://#ESP_IP#/#websocket_endpoint#
// The "window.location.hostname" is a browser call that returns the ip address 
// of the host url in the request
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection to...');
    console.log(gateway);
    websocket = new WebSocket(gateway);
    websocket.onopen  = onOpen;
    websocket.onclose = onClose;
}

function onOpen(event) {
    console.log('Connection opened');
    
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}