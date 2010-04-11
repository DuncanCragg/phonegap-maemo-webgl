using namespace std;

#include <iostream>
#include <unistd.h>

#include  <cmath>
#include  <sys/time.h>

#include  <X11/Xlib.h>

#include  <GLES2/gl2.h>
#include  <EGL/egl.h>

extern Display    *x_display;
extern EGLDisplay  egl_display;
extern EGLSurface  egl_surface;

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

int initShaders()
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

const float vertexArray[] = {
   0.0,  0.5,  0.0,
  -0.5,  0.0,  0.0,
   0.0, -0.5,  0.0,
   0.5,  0.0,  0.0,
   0.0,  0.5,  0.0 
};

bool update_pos = false;

void drawScene()
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

   drawScene();

   return ok;
}

