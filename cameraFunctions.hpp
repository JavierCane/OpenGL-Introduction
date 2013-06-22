#ifndef _CAMERA_FUNCTIONS_HPP
#define _CAMERA_FUNCTIONS_HPP

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

void initOrthoCamera( GLdouble max_scene_radius, GLdouble aspect_ratio, GLdouble camera_distance, GLdouble zoom );

void initPerspectiveCamera( GLdouble max_scene_radius, GLdouble aspect_ratio, GLdouble camera_distance, GLdouble zoom );

void initCamera( bool camera_ortho_mode, GLdouble max_scene_radius, GLdouble aspect_ratio, GLdouble camera_distance, GLdouble zoom );

void positionCamera( GLdouble max_scene_radius, bool camera_euler_mode, Coord euler_angles, Coord vrp_pos, GLdouble camera_distance );

void floorPlan( GLdouble max_scene_radius, bool camera_euler_mode, Coord *euler_angles, Coord *vrp_pos, GLdouble camera_distance );

void sidePlan( GLdouble max_scene_radius, bool camera_euler_mode, Coord *euler_angles, Coord *vrp_pos, GLdouble camera_distance );

#endif
