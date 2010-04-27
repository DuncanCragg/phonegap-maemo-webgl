#ifndef GPS_H
#define GPS_H

#include <location/location-gps-device.h>
#include <location/location-gpsd-control.h>

#include <QObject>
#include <QWebView>

class GPS : public QObject
{
    Q_OBJECT

public:
    GPS(QWebView*);
   ~GPS();

public slots:
    void get();

   private:

    QWebView* iWebView;
    LocationGPSDControl *control;
    LocationGPSDevice   *device;
};

#endif // GPS_H
