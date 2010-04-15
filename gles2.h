#ifndef GLES2_H
#define GLES2_H

#include  <GLES2/gl2.h>

#include <QObject>
#include <QWebView>

class GLES2 : public QObject
{
    Q_OBJECT

public:
    GLES2(QWebView*);

public slots:
    int FRAGMENT_SHADER(){ return GL_FRAGMENT_SHADER; };
    int VERTEX_SHADER(){   return GL_VERTEX_SHADER; };

    void viewport(int x, int y, int w, int h);
    int createShader(int type);

    void pass();

   private:

    QWebView* iWebView;
};

#endif // GLES2_H
