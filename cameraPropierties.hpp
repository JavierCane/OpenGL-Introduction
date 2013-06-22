#ifndef _CAMERA_PROPIERTIES_HPP
#define _CAMERA_PROPIERTIES_HPP

#include "structs.hpp"

// Distancia de la cámara con respecto a la escena
GLdouble camera_distance;
// Posición del VRP (objetivo a enfocar, View Reference Point)
Coord vrp_pos;
// Modo cámara
bool camera_ortho_mode = true; // true = ortho, false = perspective
// Modo cálculo posición cámara
bool camera_euler_mode = false;
// Ángulos de euler (0, 0, 0 para mirar de frente)
Coord euler_angles;
// Nivel de zoom, por defecto escala real = 1x
GLdouble zoom = 1;

#endif
