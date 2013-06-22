// Control de cámaras
#include "cameraControl.hpp"

// Reservo espacio de memoria para los atributos de clase estáticos
Coord cameraControl::last_user_defined_euler_angles;
Coord cameraControl::last_user_defined_vrp_pos;

/**
  * Devuelve el siguiente valor del enum
  * Se asume que van a tener valores consecutivos
  */
cameraControl::dynamicCameraPosition cameraControl::getNext( cameraControl::dynamicCameraPosition dynamic_camera_position, Coord euler_angles, Coord vrp_pos )
{
    // Si la posición desde la que cambiamos es la definida por el usuario, guardo sus propiedades para poder restaurarla luego
    if ( dynamic_camera_position == USER_DEFINED )
    {
        cameraControl::last_user_defined_euler_angles = euler_angles;
        cameraControl::last_user_defined_vrp_pos = vrp_pos;
    }

    const int i = static_cast<int> ( dynamic_camera_position ) + 1;

    dynamic_camera_position = static_cast<dynamicCameraPosition>( (i) % NUM_POSITIONS );

    return dynamic_camera_position;
}

/**
  * Modifica los parámetros de entrada/salida euler_angles y vrp_pos en función de la posición dada.
  * Si la posición es la definida por el usuario, recupera la que se guardó con el último getNext
  */
void cameraControl::calcEulerAndVrp( cameraControl::dynamicCameraPosition dynamic_camera_position, Coord *euler_angles, Coord *vrp_pos )
{
    if ( dynamic_camera_position == USER_DEFINED )
    {
        ( *euler_angles ) = cameraControl::last_user_defined_euler_angles;
        ( *vrp_pos ) = cameraControl::last_user_defined_vrp_pos;
    }
    else
    {
        vrp_pos->x = 0;
        vrp_pos->y = 0;
        vrp_pos->z = 0;

        euler_angles->x = 0;
        euler_angles->y = 0;
        euler_angles->z = 0;

        switch ( dynamic_camera_position )
        {
            case FLOOR:
                euler_angles->x = 90;
                break;
            case SIDE:
                euler_angles->y = 90;
                break;
            case FRONT:
                euler_angles->z = 90;
                break;
        }
    }
}

/**
  * Inicializa una cámara ortogonal/axonométrica o perspectiva dependiendo del modo que tenga establecido
  */
void cameraControl::initCamera( Camera camera, GLdouble max_scene_radius )
{
    if ( camera.camera_ortho_mode )
    {
        cameraControl::initOrthoCamera( camera, max_scene_radius );
    }
    else
    {
        cameraControl::initPerspectiveCamera( camera, max_scene_radius );
    }
}

/**
  * Inicializa una cámara ortogonal/axonométrica
  */
void cameraControl::initOrthoCamera( Camera camera, GLdouble max_scene_radius )
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    GLdouble w_camera = max_scene_radius,
            h_camera = max_scene_radius;

    // ancho de la ventana mayor a alto => aumento ancho de la cámara
    if ( camera.aspect_ratio >= 1 )
    {
        w_camera *= camera.aspect_ratio;
    }
    else
    {
        h_camera /= camera.aspect_ratio;
    }

    w_camera /= camera.zoom;
    h_camera /= camera.zoom;

    // http://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml
    // Distancias desde donde definamos el observador hasta los planos de recorte
    glOrtho(
        -w_camera, // left
        w_camera,  // right
        -h_camera, // bottom
        h_camera,  // top
        camera.camera_distance - max_scene_radius,  // z_near
        camera.camera_distance + max_scene_radius   // z_far
    );

    glMatrixMode( GL_MODELVIEW );
}

/**
  * Inicializa una cámara perspectiva
  */
void cameraControl::initPerspectiveCamera( Camera camera, GLdouble max_scene_radius )
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    // http://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml

    GLdouble fovy = asin( max_scene_radius / camera.camera_distance ) / camera.zoom;

    if ( camera.aspect_ratio < 1.0 )
    {
        fovy = atan( tan( fovy ) / camera.aspect_ratio );
    }

    fovy = toDegrees( 2.0 * fovy );

    gluPerspective(
        fovy, // fovy (ángulo de apertura)
        camera.aspect_ratio,  // aspect
        camera.camera_distance - max_scene_radius,  // z_near
        camera.camera_distance + max_scene_radius   // z_far
    );

    glMatrixMode( GL_MODELVIEW );
}

/**
  * Basándonos siempre en lso ángulos de Euler definidos en la cámara, calcula la transformación necesaria o
  * la posición del observador y VRP y las aplica.
  */
void cameraControl::moveCamera( Camera camera )
{
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    if ( camera.camera_euler_mode )
    {
        glTranslated( 0.0, 0.0, -camera.camera_distance );
        glRotated( -camera.euler_angles.z, 0.0, 0.0, 1.0 );
        glRotated( camera.euler_angles.x, 1.0, 0.0, 0.0 );
        glRotated( -camera.euler_angles.y, 0.0, 1.0, 0.0 );
        glTranslated( -camera.vrp_pos.x, -camera.vrp_pos.y, -camera.vrp_pos.z );
    }
    else
    {
        // http://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml
        //  El observador (OBS) define la posición de la cámara, que mirará en la dirección que establezca el VRP
        //  El vector UP, define la orientación de la cámara, no tiene sentido establecerlo en la misma dirección en la que mira, ya que
        // la orientación realmente se obtiene mediante la proyección del vector de UP en el plano perpendicular a la dirección de visión (recta que une el OBS con VRP).
        // si sólo es un uno (tipo 0, 0, 1) el vector que se establezca a 1 será el que se vea en el eje y

        // En base a ángulos de Euler
        gluLookAt(
            camera.vrp_pos.x + camera.camera_distance * cos( toRadians( camera.euler_angles.x ) ) * sin( toRadians( camera.euler_angles.y ) ), // OBS.x
            camera.vrp_pos.y + camera.camera_distance * sin( toRadians( camera.euler_angles.x ) ), // OBS.y
            camera.vrp_pos.z + camera.camera_distance * cos( toRadians( camera.euler_angles.x ) ) * cos( toRadians( camera.euler_angles.y ) ), // OBS.z
            camera.vrp_pos.x, camera.vrp_pos.y, camera.vrp_pos.z, // posición objetivo a mirar (vrp)
            0, 1, 0  // orientación de cámara (up)
        );

        /*
        // Vertical
        gluLookAt(
            0, camera.camera_distance, 0, // posición desde dónde miro (obs)
            0, 0, 0, // posición objetivo a mirar (vrp)
            1, 0, 0  // orientación de cámara (up)
        );

        // Horizontal
        gluLookAt(
            0, 0, camera.camera_distance, // posición desde dónde miro (obs)
            camera.vrp_pos.x, camera.vrp_pos.y, camera.vrp_pos.z, // posición objetivo a mirar (vrp)
            0, 1, 0  // orientación de cámara (up)
        );
        */
    }
}
