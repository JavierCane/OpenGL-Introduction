#include "cameraFunctions.hpp"

void initOrthoCamera( GLdouble max_scene_radius, GLdouble aspect_ratio, GLdouble camera_distance, GLdouble zoom )
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    GLdouble w_camera = max_scene_radius, h_camera = max_scene_radius;

    // ancho de la ventana mayor a alto => aumento ancho de la cámara
    if ( aspect_ratio >= 1 )
    {
        w_camera *= aspect_ratio;
    }
    else
    {
        h_camera /= aspect_ratio;
    }

    w_camera /= zoom;
    h_camera /= zoom;

    // http://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml
    // Distancias desde donde definamos el observador hasta los planos de recorte
    glOrtho(
        -w_camera, // left
        w_camera,  // right
        -h_camera, // bottom
        h_camera,  // top
        camera_distance - max_scene_radius,  // z_near
        camera_distance + max_scene_radius   // z_far
    );

    glMatrixMode( GL_MODELVIEW );
}

void initPerspectiveCamera( GLdouble max_scene_radius, GLdouble aspect_ratio, GLdouble camera_distance, GLdouble zoom )
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    // http://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml

    GLdouble fovy = asin( max_scene_radius / camera_distance ) / zoom;

    if ( aspect_ratio < 1.0 )
    {
        fovy = atan( tan( fovy ) / aspect_ratio );
    }

    fovy = toDegrees( 2.0 * fovy );

    gluPerspective(
        fovy, // fovy (ángulo de apertura)
        aspect_ratio,  // aspect
        camera_distance - max_scene_radius,  // z_near
        camera_distance + max_scene_radius   // z_far
    );

    glMatrixMode( GL_MODELVIEW );
}

void initCamera( bool camera_ortho_mode, GLdouble max_scene_radius, GLdouble aspect_ratio, GLdouble camera_distance, GLdouble zoom )
{
    if ( camera_ortho_mode )
    {
        initOrthoCamera( max_scene_radius, aspect_ratio, camera_distance, zoom );
    }
    else
    {
        initPerspectiveCamera( max_scene_radius, aspect_ratio, camera_distance, zoom);
    }
}

void positionCamera( GLdouble max_scene_radius, bool camera_euler_mode, Coord euler_angles, Coord vrp_pos, GLdouble camera_distance )
{
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    if ( camera_euler_mode )
    {
        glTranslated( 0.0, 0.0, -camera_distance );
        glRotated( -euler_angles.z, 0.0, 0.0, 1.0 );
        glRotated( euler_angles.x, 1.0, 0.0, 0.0 );
        glRotated( -euler_angles.y, 0.0, 1.0, 0.0 );
        glTranslated( -vrp_pos.x, -vrp_pos.y, -vrp_pos.z );
    }
    else
    {
        // http://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml
        //  El observador (OBS) define la posición de la cámara, que mirará en la dirección que establezca el VRP
        //  El vector UP, define la orientación de la cámara, no tiene sentido establecerlo en la misma dirección en la que mira, ya que
        // la orientación realmente se obtiene mediante la proyección del vector de UP en el plano perpendicular a la dirección de visión (recta que une el OBS con VRP).
        // si sólo es un uno (tipo 0, 0, 1) el vector que se establezca a 1 será el que se vea en el eje y

        // Vertical
//        gluLookAt(
//            0, 2 * max_scene_radius, 0, // posición desde dónde miro (obs)
//            0, 0, 0, // posición objetivo a mirar (vrp)
//            1, 0, 0  // orientación de cámara (up)
//        );

        // Horizontal
//        gluLookAt(
//            0, 0, camera_distance, // posición desde dónde miro (obs)
//            vrp_pos.x, vrp_pos.y, vrp_pos.z, // posición objetivo a mirar (vrp)
//            0, 1, 0  // orientación de cámara (up)
//        );

        // En base a ángulos de Euler
        gluLookAt(
            vrp_pos.x + camera_distance * cos( toRadians( euler_angles.x ) ) * sin( toRadians( euler_angles.y ) ), // OBS.x
            vrp_pos.y + camera_distance * sin( toRadians( euler_angles.x ) ), // OBS.y
            vrp_pos.z + camera_distance * cos( toRadians( euler_angles.x ) ) * cos( toRadians( euler_angles.y ) ), // OBS.z
            vrp_pos.x, vrp_pos.y, vrp_pos.z, // posición objetivo a mirar (vrp)
            0, 1, 0  // orientación de cámara (up)
        );
    }
}

void floorPlan( GLdouble max_scene_radius, bool camera_euler_mode, Coord *euler_angles, Coord *vrp_pos, GLdouble camera_distance )
{
    euler_angles->x = 90;
    euler_angles->y = 0;
    euler_angles->z = 0;
    vrp_pos->x = 0;
    vrp_pos->y = 0;
    vrp_pos->z = 0;

    positionCamera( max_scene_radius, camera_euler_mode, ( *euler_angles ), ( *vrp_pos ), camera_distance ); // Declaro posición cámara ortogonal
}

void sidePlan( GLdouble max_scene_radius, bool camera_euler_mode, Coord *euler_angles, Coord *vrp_pos, GLdouble camera_distance )
{
    euler_angles->x = 0;
    euler_angles->y = 90;
    euler_angles->z = 0;
    vrp_pos->x = 0;
    vrp_pos->y = 0;
    vrp_pos->z = 0;

    positionCamera( max_scene_radius, camera_euler_mode, ( *euler_angles ), ( *vrp_pos ), camera_distance ); // Declaro posición cámara ortogonal
}
