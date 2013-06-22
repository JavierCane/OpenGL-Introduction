#ifndef _UTILS_HPP
#define _UTILS_HPP

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

/**
  * Las librerías de c (operaciones matemáticas) devuelven y necesitan radianes
  * OpenGL trabaja en grados
  *
  * Para pasar de radianes a grados: x * 180.0 / M_PI
  */
GLdouble toDegrees( GLdouble radians );

/**
  * Las librerías de c (operaciones matemáticas) devuelven y necesitan radianes
  * OpenGL trabaja en grados
  *
  * Para pasar de grados a radianes: x * M_PI / 180.0
  */
GLdouble toRadians( GLdouble degrees );
/**
  * Calcula el radio desde el origen de coordenadas hasta una coordenada concreta
  */
GLdouble getCoordRadius( Coord coord );

/**
  * Calcula el radio desde una coordenada central (center_coord) hasta una coordenada concreta (coord)
  */
GLdouble getCoordRadius( Coord coord, Coord center_coord );

/**
  * Calcula el radio desde el origen de coordenadas hasta una coordenada concreta
  */
GLdouble getCubeRadius( Coord coord );

/**
  * Multiplica un determinado punto/coordenada por una matriz de transformación.
  */
void calcTranslatedCoord( Coord origin_coord, Coord *translated_coord, const GLdouble *transformation_matrix );

/**
  * Calcula el vértice/coordenada central entre dos puntos.
  */
Coord getCenterVertex( Coord min_vertex, Coord max_vertex );

#endif
