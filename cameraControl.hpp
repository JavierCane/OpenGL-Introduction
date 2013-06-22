#ifndef _CAMERA_CONTROL
#define _CAMERA_CONTROL

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

#include "math.h"
#include "structs.hpp"
#include "utils.hpp"

// Declaro el struct para que el comilador lo coja en las declaraciones de los métodos
struct Camera;

class cameraControl
{

private:

    static const int NUM_POSITIONS = 4;
    static Coord last_user_defined_euler_angles;
    static Coord last_user_defined_vrp_pos;


    static void initOrthoCamera( Camera camera, GLdouble max_scene_radius );

    static void initPerspectiveCamera( Camera camera, GLdouble max_scene_radius );

public:

    enum dynamicCameraPosition
    {
        USER_DEFINED = 0,   // 0 =>  0, 0,  0
        FLOOR = 1,          // 1 => -x, y, -z
        FRONT = 2,          // 2 =>  x, y, -z
        SIDE = 3            // 3 =>  x, y,  z
    };

    /**
      * Devuelve el siguiente valor del enum
      * Se asume que van a tener valores consecutivos
      */
    static cameraControl::dynamicCameraPosition getNext( cameraControl::dynamicCameraPosition dynamic_camera_position, Coord euler_angles, Coord vrp_pos );

    /**
      * Modifica los parámetros de entrada/salida euler_angles y vrp_pos en función de la posición dada.
      * Si la posición es la definida por el usuario, recupera la que se guardó con el último getNext
      */
    static void calcEulerAndVrp( cameraControl::dynamicCameraPosition dynamic_camera_position, Coord *euler_angles, Coord *vrp_pos );

    static void initCamera( Camera camera, GLdouble max_scene_radius );

    static void moveCamera( Camera camera );
};

// Estructura de cámara
struct Camera
{
    // Distancia de la cámara con respecto a la escena
    GLdouble camera_distance;
    // Posición del VRP (objetivo a enfocar, View Reference Point)
    Coord vrp_pos;
    // Modo cámara
    bool camera_ortho_mode; // true = ortho, false = perspective
    // Modo cálculo posición cámara
    bool camera_euler_mode;
    // Ángulos de euler (0, 0, 0 para mirar de frente)
    Coord euler_angles;
    // Nivel de zoom, por defecto escala real = 1x
    GLdouble zoom;
    // Aspect ratio de la escena (ancho window / alto window)
    GLdouble aspect_ratio;
    // Posición dinámica de la cámara (planta, perfil...)
    cameraControl::dynamicCameraPosition dynamic_camera_position;
};

#endif
