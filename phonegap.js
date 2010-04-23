
var PhoneGap = {}

PhoneGap.addConstructor = function(func) 
{
    func();
};


// DeviceInfo is a Qt object on Maemo

function Device() {
    this.available = true;
    try{
    this.platform  = DeviceInfo.platform;
    this.version   = DeviceInfo.version;
    this.name      = DeviceInfo.name;
    this.gap       = DeviceInfo.gap;
    this.uuid      = DeviceInfo.uuid;
    }catch(e){}
}

navigator.Device = window.Device = window.device = new Device();

/**
 * This class provides access to device accelerometer data.
 * @constructor
 */
function Accelerometer() {
    /**
     * The last known acceleration.
     */
    this.lastAcceleration = null;
}

/**
 * Asynchronously aquires the current acceleration.
 * @param {Function} successCallback The function to call when the acceleration
 * data is available
 * @param {Function} errorCallback The function to call when there is an error 
 * getting the acceleration data.
 * @param {AccelerationOptions} options The options for getting the accelerometer data
 * such as timeout.
 */
__PG_ACCELEROMETER_CALLBACK_USER = null;
__PG_ACCELEROMETER_CALLBACK = function(x,y,z)
{
    var accel =  { x:x, y:y, z:z };
    __PG_ACCELEROMETER_CALLBACK_USER(accel);
    Accelerometer.lastAcceleration = accel;
}

Accelerometer.prototype.getCurrentAcceleration = function(successCallback, errorCallback, options) {
    if (typeof successCallback == "function") {
        _NativeAccelerometer.get();
        __PG_ACCELEROMETER_CALLBACK_USER = successCallback;
    }
}

/**
 * Asynchronously aquires the acceleration repeatedly at a given interval.
 * @param {Function} successCallback The function to call each time the acceleration
 * data is available
 * @param {Function} errorCallback The function to call when there is an error 
 * getting the acceleration data.
 * @param {AccelerationOptions} options The options for getting the accelerometer data
 * such as timeout.
 */

Accelerometer.prototype.watchAcceleration = function(successCallback, errorCallback, options) {
    this.getCurrentAcceleration(successCallback, errorCallback, options);
    // TODO: add the interval id to a list so we can clear all watches
     var frequency = (options != undefined)? options.frequency : 10000;
    return setInterval(function() {
        navigator.accelerometer.getCurrentAcceleration(successCallback, errorCallback, options);
    }, frequency);
}

/**
 * Clears the specified accelerometer watch.
 * @param {String} watchId The ID of the watch returned from #watchAcceleration.
 */
Accelerometer.prototype.clearWatch = function(watchId) {
    clearInterval(watchId);
}

if (typeof navigator.accelerometer == "undefined") navigator.accelerometer = new Accelerometer();

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


/**
 * This class provides access to notifications on the device.
 */
function Notification() {
    
}

/**
 * Open a native alert dialog, with a customizable title and button text.
 * @param {String} message Message to print in the body of the alert
 * @param {String} [title="Alert"] Title of the alert dialog (default: Alert)
 * @param {String} [buttonLabel="OK"] Label of the close button (default: OK)
 */
Notification.prototype.alert = function(message, title, buttonLabel) {
    _NativeNotification.alert(message, title, buttonLabel);
};

/**
 * Start spinning the activity indicator on the statusbar
 */
Notification.prototype.activityStart = function() {
    
};

/**
 * Stop spinning the activity indicator on the statusbar, if it's currently spinning
 */
Notification.prototype.activityStop = function() {
};

/**
 * Causes the device to blink a status LED.
 * @param {Integer} count The number of blinks.
 * @param {String} colour The colour of the light.
 */
Notification.prototype.blink = function(count, colour) {
    
};

/**
 * Causes the device to vibrate.
 * @param {Integer} mills The number of milliseconds to vibrate for.
 */
Notification.prototype.vibrate = function(mills) {
    
};

/**
 * Causes the device to beep.
 * @param {Integer} count The number of beeps.
 * @param {Integer} volume The volume of the beep.
 */
Notification.prototype.beep = function(count, volume) {
    
};


if (typeof navigator.notification == "undefined") navigator.notification = new Notification();



