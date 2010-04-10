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
#include  <GLES2/gl2.h>
#include  <EGL/egl.h>

const char vertex_src [] =
"                                        \
   attribute vec4        position;       \
   varying mediump vec2  pos;            \
   uniform vec4          offset;         \
                                         \
   void main()                           \
   {                                     \
      gl_Position = position + offset;   \
      pos = position.xy;                 \
   }                                     \
";


const char fragment_src [] =
"                                                      \
   varying mediump vec2    pos;                        \
   uniform mediump float   phase;                      \
                                                       \
   void  main()                                        \
   {                                                   \
      gl_FragColor  =  vec4( 1., 0.9, 0.7, 1.0 ) *     \
        cos( 30.*sqrt(pos.x*pos.x + 1.5*pos.y*pos.y)   \
             + atan(pos.y,pos.x) - phase );            \
   }                                                   \
";


void print_shader_info_log ( GLuint  shader)
{
   GLint  length;

   glGetShaderiv ( shader , GL_INFO_LOG_LENGTH , &length );

   if ( length ) {
      char* buffer  =  new char [ length ];
      glGetShaderInfoLog ( shader , length , NULL , buffer );
      cout << "shader info: " <<  buffer << flush;
      delete [] buffer;

      GLint success;
      glGetShaderiv( shader, GL_COMPILE_STATUS, &success );
      if ( success != GL_TRUE )   exit ( 1 );
   }
}


GLuint load_shader(const char  *shader_source, GLenum type)
{
   GLuint  shader = glCreateShader( type );

   glShaderSource  ( shader , 1 , &shader_source , NULL );
   glCompileShader ( shader );

   print_shader_info_log ( shader );

   return shader;
}


Display    *x_display;
Window      win;
EGLDisplay  egl_display;
EGLContext  egl_context;

GLfloat
   norm_x    =  0.0,
   norm_y    =  0.0,
   offset_x  =  0.0,
   offset_y  =  0.0,
   p1_pos_x  =  0.0,
   p1_pos_y  =  0.0;

GLint
   phase_loc,
   offset_loc,
   position_loc;


EGLSurface  egl_surface;
bool        update_pos = false;

const float vertexArray[] = {
   0.0,  0.5,  0.0,
  -0.5,  0.0,  0.0,
   0.0, -0.5,  0.0,
   0.5,  0.0,  0.0,
   0.0,  0.5,  0.0 
};


void  render()
{
   static float  phase = 0;
   static int    donesetup = 0;

   if ( !donesetup ) {
      glViewport ( 0 , 0 , 800, 480 );
      glClearColor ( 0.08 , 0.06 , 0.07 , 1.);
      donesetup = 1;
   }
   glClear ( GL_COLOR_BUFFER_BIT );

   glUniform1f ( phase_loc , phase );
   phase  =  fmodf ( phase + 0.5f , 2.f * 3.141f );

   if ( update_pos ) {
      GLfloat old_offset_x  =  offset_x;
      GLfloat old_offset_y  =  offset_y;

      offset_x  =  norm_x - p1_pos_x;
      offset_y  =  norm_y - p1_pos_y;

      p1_pos_x  =  norm_x;
      p1_pos_y  =  norm_y;

      offset_x  +=  old_offset_x;
      offset_y  +=  old_offset_y;

      update_pos = false;
   }

   glUniform4f ( offset_loc  ,  offset_x , offset_y , 0.0 , 0.0 );

   glVertexAttribPointer ( position_loc, 3, GL_FLOAT, false, 0, vertexArray );
   glEnableVertexAttribArray ( position_loc );
   glDrawArrays ( GL_TRIANGLE_STRIP, 0, 5 );

   eglSwapBuffers ( egl_display, egl_surface );
}


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

int goGL()
{
   GLuint vertexShader   = load_shader ( vertex_src , GL_VERTEX_SHADER  );
   GLuint fragmentShader = load_shader ( fragment_src , GL_FRAGMENT_SHADER );

   GLuint shaderProgram  = glCreateProgram ();
   glAttachShader ( shaderProgram, vertexShader );
   glAttachShader ( shaderProgram, fragmentShader );

   glLinkProgram ( shaderProgram );
   glUseProgram  ( shaderProgram );

   position_loc  = glGetAttribLocation  ( shaderProgram , "position" );
   phase_loc     = glGetUniformLocation ( shaderProgram , "phase"    );
   offset_loc    = glGetUniformLocation ( shaderProgram , "offset"   );
   if ( position_loc < 0  ||  phase_loc < 0  ||  offset_loc < 0 ) {
      cerr << "Unable to get uniform location" << endl;
      return 0;
   }
   return 1;
}


bool GLPass()
{
   bool ok = true;

   const float window_width  = 800.0;
   const float window_height = 480.0;

   while(XPending(x_display)){

         XEvent  xev;
         XNextEvent( x_display, &xev );

         if ( xev.type == MotionNotify ){
            GLfloat window_y  =  (window_height - xev.xmotion.y) - window_height / 2.0;
            norm_y            =  window_y / (window_height / 2.0);
            GLfloat window_x  =  xev.xmotion.x - window_width / 2.0;
            norm_x            =  window_x / (window_width / 2.0);
            update_pos = true;
         }
         if ( xev.type == KeyPress )   ok = false;
   }

   render();

   return ok;
}

void cleanUpEGL()
{
   eglDestroyContext ( egl_display, egl_context );
   eglDestroySurface ( egl_display, egl_surface );
   eglTerminate      ( egl_display );
}

void cleanUpX11()
{
   XDestroyWindow( x_display, win );
   XCloseDisplay( x_display );
}

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
    if(!goGL())          return 0;

    app.exec();

    cleanUpEGL();
    cleanUpX11();

    return 1;
}
