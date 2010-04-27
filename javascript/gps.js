
/**
 * This class provides access to device GPS data.
 * @constructor
 */
function GPS() {
    /**
     * The last known coordinates.
     */
    this.lastCoordinates = null;
}

/**
 * Asynchronously aquires the current GPS coordinates.
 * @param {Function} successCallback The function to call when the 
 * data is available
 * @param {Function} errorCallback The function to call when there is an error 
 * getting the GPS data.
 * @param {GPSOptions} options The options for getting the GPS data
 * such as timeout.
 */
__PG_GPS_CALLBACK_USER = null;
__PG_GPS_CALLBACK = function(latitude, longitude)
{
    var coords =  { latitude:latitude, longitude:longitude, };
    __PG_GPS_CALLBACK_USER(coords);
    GPS.lastCoordinates = coords;
}

GPS.prototype.getCurrentCoordinates = function(successCallback, errorCallback, options) {
    if (typeof successCallback == "function") {
        _NativeGPS.get();
        __PG_GPS_CALLBACK_USER = successCallback;
    }
}

/**
 * Asynchronously aquires the coordinates repeatedly at a given interval.
 * @param {Function} successCallback The function to call each time the GPS
 * data is available
 * @param {Function} errorCallback The function to call when there is an error 
 * getting the GPS data.
 * @param {GPSOptions} options The options for getting the GPS data
 * such as timeout.
 */
GPS.prototype.watchGPS = function(successCallback, errorCallback, options) {
    this.getCurrentCoordinates(successCallback, errorCallback, options);
    // TODO: add the interval id to a list so we can clear all watches
     var frequency = (options != undefined)? options.frequency : 10000;
    return setInterval(function() {
        navigator.gps.getCurrentCoordinates(successCallback, errorCallback, options);
    }, frequency);
}

/**
 * Clears the specified GPS watch.
 * @param {String} watchId The ID of the watch returned from #watchGPS.
 */
GPS.prototype.clearWatch = function(watchId) {
    clearInterval(watchId);
}

if (typeof navigator.gps == "undefined") navigator.gps = new GPS();

