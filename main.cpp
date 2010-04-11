using namespace std;

#include <iostream>
#include <unistd.h>

#include  <cmath>
#include  <sys/time.h>

#include <QApplication>
#include <QWidget>
#include <QWebPage>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QWebFrame>

#include "deviceinfo.h"
#include "commandmanager.h"
#include "webview.h"

#define BINARY_NAME "phonegapdemo"

using namespace PhoneGap;

/** Custom QWebPage which prints the javascript errors to console */
class PGWebPage : public QWebPage
{
    public:

    void javaScriptConsoleMessage(const QString& message, int lineNumber, const QString& sourceID)
    {
        std::cout << lineNumber << ":" << sourceID.toAscii().data() << " " << message.toAscii().data() << std::endl;
    }
};

PGNetworkAccessManager::PGNetworkAccessManager(QWebView *webview)
{
    this->iWebView = webview;
}

#include  <X11/Xlib.h>
#include  <X11/Xatom.h>

Display    *x_display;
Window      win;

int getX11Display()
{
   x_display = XOpenDisplay ( NULL );
   if ( x_display == NULL ) {
      cerr << "cannot connect to X server" << endl;
      return 0;
   }

   Window root  =  DefaultRootWindow( x_display );

   XSetWindowAttributes  swa;
   swa.event_mask  =  ExposureMask | PointerMotionMask | KeyPressMask;

   win  =  XCreateWindow (
              x_display, root,
              0, 0, 800, 480,   0,
              CopyFromParent, InputOutput,
              CopyFromParent, CWEventMask,
              &swa );

   XSetWindowAttributes  xattr;
   Atom  atom;
   int   one = 1;

   xattr.override_redirect = False;
   XChangeWindowAttributes ( x_display, win, CWOverrideRedirect, &xattr );

   atom = XInternAtom ( x_display, "_NET_WM_STATE_FULLSCREEN", True );
   XChangeProperty (
      x_display, win,
      XInternAtom ( x_display, "_NET_WM_STATE", True ),
      XA_ATOM,  32,  PropModeReplace,
      (unsigned char*) &atom,  1 );

   XChangeProperty (
      x_display, win,
      XInternAtom ( x_display, "_HILDON_NON_COMPOSITED_WINDOW", True ),
      XA_INTEGER,  32,  PropModeReplace,
      (unsigned char*) &one,  1);

   XMapWindow ( x_display , win );
   XStoreName ( x_display , win , "GL test" );

   Atom wm_state   = XInternAtom ( x_display, "_NET_WM_STATE", False );
   Atom fullscreen = XInternAtom ( x_display, "_NET_WM_STATE_FULLSCREEN", False );

   XEvent xev;
   memset ( &xev, 0, sizeof(xev) );

   xev.type                 = ClientMessage;
   xev.xclient.window       = win;
   xev.xclient.message_type = wm_state;
   xev.xclient.format       = 32;
   xev.xclient.data.l[0]    = 1;
   xev.xclient.data.l[1]    = fullscreen;
   XSendEvent (
      x_display,
      DefaultRootWindow ( x_display ),
      False,
      SubstructureNotifyMask,
      &xev );

   return 1;
}

void cleanUpX11()
{
   XDestroyWindow( x_display, win );
   XCloseDisplay( x_display );
}

#include  <EGL/egl.h>

EGLDisplay  egl_display;
EGLContext  egl_context;
EGLSurface  egl_surface;

int setUpEGL()
{
   egl_display  =  eglGetDisplay( (EGLNativeDisplayType) x_display );
   if ( egl_display == EGL_NO_DISPLAY ) {
      cerr << "Got no EGL display." << endl;
      return 0;
   }

   if ( !eglInitialize( egl_display, NULL, NULL ) ) {
      cerr << "Unable to initialize EGL" << endl;
      return 0;
   }

   EGLint attr[] = {
      EGL_BUFFER_SIZE, 16,
      EGL_RENDERABLE_TYPE,
      EGL_OPENGL_ES2_BIT,
      EGL_NONE
   };

   EGLConfig  ecfg;
   EGLint     num_config;
   if ( !eglChooseConfig( egl_display, attr, &ecfg, 1, &num_config ) ) {
      cerr << "Failed to choose config (eglError: " << eglGetError() << ")" << endl;
      return 0;
   }

   if ( num_config != 1 ) {
      cerr << "Didn't get exactly one config, but " << num_config << endl;
      return 0;
   }

   egl_surface = eglCreateWindowSurface ( egl_display, ecfg, (void*)win, NULL );
   if ( egl_surface == EGL_NO_SURFACE ) {
      cerr << "Unable to create EGL surface (eglError: " << eglGetError() << ")" << endl;
      return 0;
   }

   EGLint ctxattr[] = {
      EGL_CONTEXT_CLIENT_VERSION, 2,
      EGL_NONE
   };
   egl_context = eglCreateContext ( egl_display, ecfg, EGL_NO_CONTEXT, ctxattr );
   if ( egl_context == EGL_NO_CONTEXT ) {
      cerr << "Unable to create EGL context (eglError: " << eglGetError() << ")" << endl;
      return 0;
   }

   eglMakeCurrent( egl_display, egl_surface, egl_surface, egl_context );

   return 1;
}

void cleanUpEGL()
{
   eglDestroyContext ( egl_display, egl_context );
   eglDestroySurface ( egl_display, egl_surface );
   eglTerminate      ( egl_display );
}

// -------------------------------------------------------------------

extern int initShaders();

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;       

    window.resize(800, 480);
    window.setWindowTitle("PhoneGap");
    window.show();

    WebView *view = new WebView(&window);

    PGWebPage* page = new PGWebPage();
    view->setPage(page);

    view->settings()->setAttribute(QWebSettings::JavascriptEnabled, TRUE);

    PGNetworkAccessManager mymanager(view);
    page->setNetworkAccessManager(&mymanager);

    view->initPhoneGapAPI();
    
    QFile *file = new QFile("www/index.html");
    QUrl *url = new QUrl("file:///www/index.html");

    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
         std::cout << "No local index.html - trying global" << std::endl;
	 file = new QFile("/usr/share/" BINARY_NAME "/www/index.html");
	 if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {
		std::cout << "No bootstrap JS - giving up" << std::endl;
		return 1;
	 }
	 url = new QUrl("file:////usr/share/" BINARY_NAME "/www/index.html");
    }

    QTextStream in(file);

    view->setHtml( in.readAll(),  *url);
    view->show();

    if(!getX11Display()) return 0;
    if(!setUpEGL())      return 0;

    if(!initShaders())   return 0;

    app.exec();

    cleanUpEGL();
    cleanUpX11();

    return 1;
}
