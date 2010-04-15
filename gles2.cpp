
#include <QtDebug>
#include <QWebFrame>
#include <QWebPage>

#include "utils.h"
#include "gles2.h"

static QWebView *gWebView = NULL;

GLES2::GLES2(QWebView *aWebView) : iWebView(aWebView)
{
    gWebView = iWebView;
}

extern bool GLPass();

void GLES2::viewport(int x, int y, int w, int h){
    glViewport(x, y, w, h);
}

int GLES2::createShader(int type){
    return glCreateShader(type);
}

void GLES2::pass(){
    GLPass();
}


