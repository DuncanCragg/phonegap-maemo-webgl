/**
 * This class provides access to GL ES 2.0
 */

if(typeof _NativeGLES2 != "undefined"){

function GLES2() { }

// -----------------------------------------------------------

GLES2.prototype.FRAGMENT_SHADER = _NativeGLES2.FRAGMENT_SHADER();
GLES2.prototype.VERTEX_SHADER   = _NativeGLES2.VERTEX_SHADER();

// -----------------------------------------------------------

GLES2.prototype.viewport     = function(x,y,w,h) {    _NativeGLES2.viewport(x,y,w,h); }
GLES2.prototype.createShader = function(type){ return _NativeGLES2.createShader(type); }

// -----------------------------------------------------------

GLES2.prototype.pass = function() { _NativeGLES2.pass(); }

// -----------------------------------------------------------

if(typeof navigator.gles2 == "undefined") navigator.gles2 = new GLES2();

}

// -----------------------------------------------------------

