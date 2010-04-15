
var PhoneGap={}
PhoneGap.addConstructor=function(func)
{func();};function Device(){this.available=true;try{this.platform=DeviceInfo.platform;this.version=DeviceInfo.version;this.name=DeviceInfo.name;this.gap=DeviceInfo.gap;this.uuid=DeviceInfo.uuid;}catch(e){}}
navigator.Device=window.Device=window.device=new Device();function Accelerometer(){this.lastAcceleration=null;}
__PG_ACCELEROMETER_CALLBACK_USER=null;__PG_ACCELEROMETER_CALLBACK=function(x,y,z)
{var accel={x:x,y:y,z:z};__PG_ACCELEROMETER_CALLBACK_USER(accel);Accelerometer.lastAcceleration=accel;}
Accelerometer.prototype.getCurrentAcceleration=function(successCallback,errorCallback,options){if(typeof successCallback=="function"){_NativeAccelerometer.get();__PG_ACCELEROMETER_CALLBACK_USER=successCallback;}}
Accelerometer.prototype.watchAcceleration=function(successCallback,errorCallback,options){this.getCurrentAcceleration(successCallback,errorCallback,options);var frequency=(options!=undefined)?options.frequency:10000;return setInterval(function(){navigator.accelerometer.getCurrentAcceleration(successCallback,errorCallback,options);},frequency);}
Accelerometer.prototype.clearWatch=function(watchId){clearInterval(watchId);}
if(typeof navigator.accelerometer=="undefined")navigator.accelerometer=new Accelerometer();if(typeof _NativeGLES2!="undefined"){function GLES2(){}
GLES2.prototype.FRAGMENT_SHADER=_NativeGLES2.FRAGMENT_SHADER();GLES2.prototype.VERTEX_SHADER=_NativeGLES2.VERTEX_SHADER();GLES2.prototype.viewport=function(x,y,w,h){_NativeGLES2.viewport(x,y,w,h);}
GLES2.prototype.createShader=function(type){return _NativeGLES2.createShader(type);}
GLES2.prototype.pass=function(){_NativeGLES2.pass();}
if(typeof navigator.gles2=="undefined")navigator.gles2=new GLES2();}
function Notification(){}
Notification.prototype.alert=function(message,title,buttonLabel){_NativeNotification.alert(message,title,buttonLabel);};Notification.prototype.activityStart=function(){};Notification.prototype.activityStop=function(){};Notification.prototype.blink=function(count,colour){};Notification.prototype.vibrate=function(mills){};Notification.prototype.beep=function(count,volume){};if(typeof navigator.notification=="undefined")navigator.notification=new Notification();