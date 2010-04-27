#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <QObject>
#include <QWebView>

// See Maemo API at: http://wiki.maemo.org/Accelerometers

class Accelerometer : public QObject
{
    Q_OBJECT

public:
    Accelerometer(QWebView*);

public slots:
    void get();

   private:

    QWebView* iWebView;
};

#endif // ACCELEROMETER_H
