
//-------------Demonstrate PhoneGap Features -----------------------------------------

function initDemo() {        
    PhoneGap.addConstructor(function(){
        document.addEventListener("touchmove", preventBehavior, false);
        document.addEventListener('orientationChanged', function(e) { debug.log("Orientation changed to " + e.orientation); }, false);
    });
    webGLStart();
}

function preventBehavior(e) { e.preventDefault(); };
 
function deviceInfo(){
    document.getElementById("platform").innerHTML   = device.platform;
    document.getElementById("version").innerHTML    = device.version;
    document.getElementById("devicename").innerHTML = device.name;
    document.getElementById("uuid").innerHTML       = device.uuid;
}
    
function watchAccel() {
    var suc = function(a){
        document.getElementById('x').innerHTML = roundNumber(a.x, 3);
        document.getElementById('y').innerHTML = roundNumber(a.y, 3);
        document.getElementById('z').innerHTML = roundNumber(a.z, 3);
    };
    var fail = function(){};
    var opt = {};
    opt.frequency = 500;
    timer = navigator.accelerometer.watchAcceleration(suc,fail,opt);
}
      
function roundNumber(num, dec) { return Math.round(num*Math.pow(10,dec))/Math.pow(10,dec); }

function showAlert(){
    navigator.notification.alert("This is an alert", "Alert");
}
  
function beep(){
  debug.log("beep");
  navigator.notification.beep(2);
}
  
function vibrate(){
  debug.log("vibrate");
  navigator.notification.vibrate(0);
}
  
//-------------WebGL--------------------------------------------------------

var gl;

function glLoop(){
    for(var x=0; x<40; x++) gl.pass();
}

function initGL() {
    var w,h;
    try {
        var canvas = document.getElementById("webgl-canvas");
        gl = canvas.getContext("experimental-webgl");
        w=canvas.width; h=canvas.height;
    } catch(e) { }
    if(!gl){ try{
        gl = navigator.gles2;
        w=800; h=480;
    } catch(e) { } }
    if(!gl){ alert("Could not initialise WebGL"); return; }

    gl.viewport(0, 0, w, h);
}

function getShader(gl, id) {

    var shaderScript = document.getElementById(id);
    if (!shaderScript) return null;

    var str = "";
    var k = shaderScript.firstChild;
    while (k) {
      if (k.nodeType == 3) {
        str += k.textContent;
      }
      k = k.nextSibling;
    }

    var shader;
    if (shaderScript.type == "x-shader/x-fragment") {
      shader = gl.createShader(gl.FRAGMENT_SHADER);
    } else if (shaderScript.type == "x-shader/x-vertex") {
      shader = gl.createShader(gl.VERTEX_SHADER);
    } else {
      return null;
    }

    gl.shaderSource(shader, str);
    gl.compileShader(shader);

    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
      alert(gl.getShaderInfoLog(shader));
      return null;
    }

    return shader;
}


var shaderProgram;
var position_loc;
var phase_loc;
var offset_loc;

function initShaders() {

    var vertexShader   = getShader(gl, "shader-vs");
    var fragmentShader = getShader(gl, "shader-fs");

    shaderProgram = gl.createProgram();

    gl.attachShader(shaderProgram, vertexShader);
    gl.attachShader(shaderProgram, fragmentShader);
    gl.linkProgram(shaderProgram);

    if(!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
      alert("Could not initialise shaders");
    }

    gl.useProgram(shaderProgram);

    position_loc = gl.getAttribLocation( shaderProgram, "position");
    phase_loc    = gl.getUniformLocation(shaderProgram, "phase");
    offset_loc   = gl.getUniformLocation(shaderProgram, "offset");
}

var norm_x    =  0.0;
var norm_y    =  0.0;
var offset_x  =  0.0;
var offset_y  =  0.0;
var p1_pos_x  =  0.0;
var p1_pos_y  =  0.0;

function initBuffers() {
    vertexPositionBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vertexPositionBuffer);

    var vertices = [
       0.0,  0.5,  0.0,
      -0.5,  0.0,  0.0,
       0.0, -0.5,  0.0,
       0.5,  0.0,  0.0,
       0.0,  0.5,  0.0 
    ];

    gl.bufferData(gl.ARRAY_BUFFER, new WebGLFloatArray(vertices), gl.STATIC_DRAW);

    vertexPositionBuffer.itemSize = 3;
    vertexPositionBuffer.numItems = 5;
}

var phase = 0;

function drawScene() {
    gl.clear(gl.COLOR_BUFFER_BIT)

    gl.uniform1f( phase_loc , phase );
    phase  =  ( phase + 0.5 ) % ( 2.0 * 3.141 );

    gl.uniform4f( offset_loc,  offset_x , offset_y , 0.0 , 0.0 );

    gl.vertexAttribPointer ( position_loc, 3, gl.FLOAT, false, 0, vertexPositionBuffer );
    gl.enableVertexAttribArray ( position_loc );
    gl.drawArrays( gl.TRIANGLE_STRIP, 0, 5 );
}

var lastTime = 0;
function animate() {
    var timeNow = new Date().getTime();
    if (lastTime != 0) {
      var elapsed = timeNow - lastTime;
      offset_x += elapsed / 100000.0;
      offset_y -= elapsed / 100000.0;
    }
    lastTime = timeNow;
}

function tick() {
    drawScene();
    animate();
}

function webGLStart() {

    initGL(); if(!gl) return;
    initShaders()
    initBuffers();

    gl.clearColor (0.08 , 0.06 , 0.07 , 1.0);

    setInterval(tick, 15);
}

//--------------------------------------------------------------------------


