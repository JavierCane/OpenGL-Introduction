#ifndef _STRUCTS_HPP
#define _STRUCTS_HPP

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

// Estructura de coordenada
struct Coord
{
   GLdouble x, y, z;
};

// Estructura de caja contenedora, almacena:
//  coordenadas de máximos, mínimos y centro del modelo original y tras la transformacion
//  también contiene el radio máximo una vez aplicada la transformación
struct ModelContainer
{
   Coord max_origin_vertex, min_origin_vertex, center_origin_vertex,
    max_translated_vertex, min_translated_vertex, center_translated_vertex;

   GLdouble max_translated_radius;
};

#endif
