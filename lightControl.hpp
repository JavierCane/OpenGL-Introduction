#ifndef _LIGHT_CONTROL
#define _LIGHT_CONTROL

#ifndef _OPEN_GL_INCLUDED
    #if defined(__APPLE__)
        #include <OpenGL/OpenGL.h>
        #include <GLUT/GLUT.h>
        #define _OPEN_GL_INCLUDED
    #else
        #include <GL/gl.h>
        #include <GL/freeglut.h>
        #define _OPEN_GL_INCLUDED
    #endif
#endif

struct Light;

class lightControl
{

public:

    static const int NUM_POSITIONS = 5;

    enum dynamicLightPosition
    {
        CENTER = 0,         // 0 =>  0, 0,  0
        LEFT_BACK = 1,      // 1 => -x, y, -z
        RIGHT_BACK = 2,     // 2 =>  x, y, -z
        RIGHT_FRONT = 3,    // 3 =>  x, y,  z
        LEFT_FRONT = 4      // 4 => -x, y,  z
    };

    /**
      * Devuelve el siguiente valor del enum
      * Se asume que van a tener valores consecutivos
      */
    static lightControl::dynamicLightPosition getNext( lightControl::dynamicLightPosition dynamic_light_position );

    /**
      * Devuelve si la coordenada x debe expresarse negativamente o no.
      * Devuelve un 1 con el signo pertinente para que se pueda multiplicar el resultado de esta operación por la coordenada x directamente
      */
    static GLfloat getXSign( lightControl::dynamicLightPosition dynamic_light_position );

    /**
      * Devuelve si la coordenada z debe expresarse negativamente o no.
      * Devuelve un 1 con el signo pertinente para que se pueda multiplicar el resultado de esta operación por la coordenada z directamente
      */
    static GLfloat getZSign( lightControl::dynamicLightPosition dynamic_light_position );
};

// Estructura de iluminación
struct Light
{
    bool enabled;
    GLfloat position[4], ambient[4], diffuse[4], specular[4];
    lightControl::dynamicLightPosition dynamic_light_position;
};

#endif
