/**
 * This class provides access to GL ES 2.0
 */

function GLES2() {
}

GLES2.prototype.pass = function() {
	_NativeGLES2.pass();
}

if (typeof navigator.gles2 == "undefined") navigator.gles2 = new GLES2();

