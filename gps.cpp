
#include <QtDebug>
#include <QWebFrame>
#include <QWebPage>

#include <stdio.h>

#include "utils.h"
#include "gps.h"

static QWebView *gWebView = NULL;

static void on_gps_changed(LocationGPSDevice *device, gpointer data)
{
    printf("on_gps_changed\n");
    if(!device) return;

    if(device->fix && (device->fix->fields & LOCATION_GPS_DEVICE_LATLONG_SET)){

            printf("lat = %f, long = %f\n", device->fix->latitude, device->fix->longitude);
            gfloat latitude  = device->fix->latitude;
            gfloat longitude = device->fix->longitude;

            location_gpsd_control_stop((LocationGPSDControl *) data);

            QString callback = QString("__PG_GPS_CALLBACK(%1,%2);").arg(latitude).arg(longitude);
            gWebView->page()->mainFrame()->evaluateJavaScript(callback);
    }
}

static void on_gps_error(LocationGPSDControl *control, LocationGPSDControlError error, gpointer data)
{ 
    printf("on_gps_error: %d %p %p\n", error, control, data);
}

static void on_gps_stop(LocationGPSDControl *control, gpointer data)
{
    printf("on_gps_stop %p %p\n", control, data);
} 

GPS::GPS(QWebView *aWebView): iWebView(aWebView)
{
    gWebView = iWebView;

    control = location_gpsd_control_get_default();
    device  = (LocationGPSDevice*)g_object_new(LOCATION_TYPE_GPS_DEVICE, NULL);

    g_object_set(G_OBJECT(control),
                 "preferred-method",   LOCATION_METHOD_USER_SELECTED,
                 "preferred-interval", LOCATION_INTERVAL_5S,
                 NULL);

    g_signal_connect(control, "error-verbose", G_CALLBACK(on_gps_error), 0);
    g_signal_connect(device,  "changed",       G_CALLBACK(on_gps_changed), 0);
    g_signal_connect(control, "gpsd-stopped",  G_CALLBACK(on_gps_stop), 0);
}

GPS::~GPS()
{
    g_object_unref(device);
    g_object_unref(control);
}

void GPS::get(){
    printf("get: location_gpsd_control_start\n");
    location_gpsd_control_start(control);
}

