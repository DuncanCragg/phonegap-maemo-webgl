#ifndef GLES2_H
#define GLES2_H

#include <QObject>
#include <QWebView>

class GLES2 : public QObject
{
    Q_OBJECT

public:
    GLES2(QWebView*);

public slots:
    void pass();

   private:

    QWebView* iWebView;
};

#endif // GLES2_H
