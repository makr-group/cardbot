var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

// initailize
window.addEventListener("load", onLoad);

function onLoad(event) {
  initWebSocket();
  initButtonUp();
  initButtonDown();
  initButtonLeft();
  initButtonRight();
  initKeyboardControl();
  
  sendRequest("name")
}

// websocket handleing
function initWebSocket() {
  console.log("Trying to open a WebSocket connection...");
  websocket = new WebSocket(gateway);
  websocket.onopen = onOpen;
  websocket.onclose = onClose;
  websocket.onmessage = onMessage;
}

function onOpen(event) {
  console.log("Connection opened");
}

function onClose(event) {
  console.log("Connection closed");
  setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
  let data = JSON.parse(event.data);
  switch (data.type) {
    case "name":
      document.getElementById("name").innerHTML = data.data;
      break;
    case "motor":
      document.getElementById("nameBanner").className = data.data;
      break;
    case "ip":
      console.log(data.data);
      break;
    default:
      console.log("invalid")
      break;
  }
}

// button function
function upPress() {
  websocket.send(JSON.stringify({'type':'action','data':'up'}));
}
function downPress() {
  websocket.send(JSON.stringify({'type':'action','data':'dp'}));
}
function leftPress() {
  websocket.send(JSON.stringify({'type':'action','data':'lp'}));
}
function rightPress() {
  websocket.send(JSON.stringify({'type':'action','data':'rp'}));
}
function upRelease() {
  websocket.send(JSON.stringify({'type':'action','data':'ur'}));
}
function downRelease() {
  websocket.send(JSON.stringify({'type':'action','data':'dr'}));
}
function leftRelease() {
  websocket.send(JSON.stringify({'type':'action','data':'lr'}));
}
function rightRelease() {
  websocket.send(JSON.stringify({'type':'action','data':'rr'}));
}

function sendRequest(data) {
  websocket.send(
    JSON.stringify({
      type: "request",
      data: data,
    })
  );
}

// keyboard commands
function initKeyboardControl() {
  document.addEventListener("keydown", (event) => {
    switch(event.code){
      case "KeyW":
        upPress();
        break;
      case "KeyA":
        leftPress();
        break;
      case "KeyS":
        downPress();
        break;
      case "KeyD":
        rightPress();
        break;
      default:
        break;
    }
  })
  window.addEventListener("keyup", (event) => {
    switch(event.code){
      case "KeyW":sw
        upRelease();
        break;
      case "KeyA":
        leftRelease();
        break;
      case "KeyS":
        downRelease();
        break;
      case "KeyD":
        rightRelease();
        break;
      default:
        break;
    }
  })
}
// buttons commands
function initButtonUp() {
  document.getElementById("buttonUp").addEventListener("mousedown", upPress);
  document.getElementById("buttonUp").addEventListener("mouseup", upRelease);
  document.getElementById("buttonUp").addEventListener("touchstart", upPress);
  document.getElementById("buttonUp").addEventListener("touchend", upRelease);
  document
    .getElementById("buttonUp")
    .addEventListener("touchcancel", upRelease);
}
function initButtonDown() {
  document
    .getElementById("buttonDown")
    .addEventListener("mousedown", downPress);
  document
    .getElementById("buttonDown")
    .addEventListener("mouseup", downRelease);
  document
    .getElementById("buttonDown")
    .addEventListener("touchstart", downPress);
  document
    .getElementById("buttonDown")
    .addEventListener("touchend", downRelease);
  document
    .getElementById("buttonDown")
    .addEventListener("touchcancel", downRelease);
}
function initButtonLeft() {
  document
    .getElementById("buttonLeft")
    .addEventListener("mousedown", leftPress);
  document
    .getElementById("buttonLeft")
    .addEventListener("mouseup", leftRelease);
  document
    .getElementById("buttonLeft")
    .addEventListener("touchstart", leftPress);
  document
    .getElementById("buttonLeft")
    .addEventListener("touchend", leftRelease);
  document
    .getElementById("buttonLeft")
    .addEventListener("touchcancel", leftRelease);
}
function initButtonRight() {
  document
    .getElementById("buttonRight")
    .addEventListener("mousedown", rightPress);
  document
    .getElementById("buttonRight")
    .addEventListener("mouseup", rightRelease);
  document
    .getElementById("buttonRight")
    .addEventListener("touchstart", rightPress);
  document
    .getElementById("buttonRight")
    .addEventListener("touchend", rightRelease);
  document
    .getElementById("buttonRight")
    .addEventListener("touchcancel", rightRelease);
}


