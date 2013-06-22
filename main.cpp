
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

#include <iostream>
#include <string>
#include <vector>

#include "math.h"
#include "./Model/model.h"

using namespace std;

#include "structs.hpp"
#include "utils.hpp"
#include "modelPropierties.hpp"
#include "cameraControl.hpp"
#include "lightControl.hpp"

// Luces
Light light0, light1;

// Cámara
Camera camera;

// Mostrar la esfera contenedora o no
bool show_container_sphere = true;

// Propiedades del suelo
Coord floor_size;
Coord floor_color;
Coord floor_translation;
GLdouble floor_radius;

// Tipo de suelo (true = el suelo será un cubo (se tendrá en cuenta componente y), false = el suelo será un polígono)
bool floor_by_cube = false;

// Define si intentaré pintar una normal por cada vértice del modelo (en caso de estar definidas) o una normal por cara
bool normal_per_vertex = true;

// Factor de escalado máximo
GLdouble max_scale_factor = 4.0;
// Radio máximo de la escena.
GLdouble max_scene_radius = 0.0;
// Relación de aspecto
GLdouble aspect_ratio = 1.0;

// Altura del objeto a posicionar
GLdouble object_height = 4.0;

// Offset en y del modelo
GLdouble model_y_offset;

// Posición del último click para tenerlo en cuenta a la hora de mover la cámara
Coord last_mouse_click_position;

/***********************************************************************************************************
************************************************************************************************************
************************************************************************************************************
\\\\\\\\\\                                  INICIALIZACIONES                                    ////////////
************************************************************************************************************
************************************************************************************************************
***********************************************************************************************************/

/**
  * Inicializa las propiedades del suelo
  */
void setFloorPropierties( Coord *floor_size, Coord *floor_color, Coord *floor_translation, GLdouble *max_scene_radius )
{
    floor_size->x = 10;
    floor_size->y = 1;
    floor_size->z = 10;

    floor_color->x = 0.7;
    floor_color->y = 0.7;
    floor_color->z = 0.3;

    floor_translation->x = 0.0;
    floor_translation->y = floor_size->y / 2.0;
    floor_translation->z = 0.0;

    floor_radius = getCubeRadius( *floor_size );
    *max_scene_radius = floor_radius;
}

/**
  * Inicializa propiedades de luz de cámara (LIGHT0).
  * Ya se inicializan por defecto, pero por probar y tal :)
  */
void setLight0Propierties( Light *light0, GLdouble max_scene_radius, bool enabled )
{
    light0->enabled = enabled;

    // No hago nada con respecto a la luz ambiente (LIGHT0) para respetar los valores por defecto
    /*
    // No defino la posición para dejar que lo haga OpenGL y así siga a la cámara automáticamente ( no llamaré a glLightfv( GL_LIGHT0, GL_POSITION, light0.position ); :) )
//    light0->position[0] = 0.0;
//    light0->position[1] = max_scene_radius;
//    light0->position[2] = 0.0;
//    light0->position[3] = 1.0; // 1 = Estamos definiendo posición de la luz, 0 = Estamos definiendo vector desde donde viene la luz (sol...)

    light0->ambient[0] = 0.5;
    light0->ambient[1] = 0.5;
    light0->ambient[2] = 0.5;
    light0->ambient[3] = 1.0;

    light0->diffuse[0] = 0.5;
    light0->diffuse[1] = 0.5;
    light0->diffuse[2] = 0.5;
    light0->diffuse[3] = 1.0;

    light0->specular[0] = 0.75;
    light0->specular[1] = 0.75;
    light0->specular[2] = 0.75;
    light0->specular[3] = 1.0;
    */
}

/**
  * Inicializa propiedades de luz LIGHT1
  *  Lo hace en función del tamaño del suelo y
  * de la posición dinámica que tenga la luz en este momento (valor del enum lightControl::dynamicPosition)
  */
void setLight1Propierties( Light *light1, Coord floor_size, GLdouble max_scene_radius, bool enabled )
{
    light1->enabled = enabled;

    light1->position[0] = floor_size.x * lightControl::getXSign( light1->dynamic_light_position );
    light1->position[1] = max_scene_radius;
    light1->position[2] = floor_size.z * lightControl::getZSign( light1->dynamic_light_position );
    light1->position[3] = 1.0; // 1 = Estamos definiendo posición de la luz, 0 = Estamos definiendo vector desde donde viene la luz (sol...)

    light1->ambient[0] = 0.2;
    light1->ambient[1] = 0.0;
    light1->ambient[2] = 0.0;
    light1->ambient[3] = 1.0;

    // La componente difusa y especular, en las luces, suele ser similar
    light1->diffuse[0] = 0.8;
    light1->diffuse[1] = 0.0;
    light1->diffuse[2] = 0.0;
    light1->diffuse[3] = 1.0;

    light1->specular[0] = 0.8;
    light1->specular[1] = 0.0;
    light1->specular[2] = 0.0;
    light1->specular[3] = 1.0;
}

/**
  * Inicializa las propiedades de la cámara
  */
void setCameraPropierties( Camera *camera, GLdouble distance_from_scene_radius, bool camera_ortho_mode, bool camera_euler_mode, GLdouble zoom, GLdouble max_scene_radius, GLdouble initial_aspect_ratio )
{
    // Inicializo ángulos de euler y VRP para mirar de frente
    camera->euler_angles.x = camera->vrp_pos.x =
    camera->euler_angles.y = camera->vrp_pos.y =
    camera->euler_angles.z = camera->vrp_pos.z = 0;

    camera->camera_distance = distance_from_scene_radius * max_scene_radius; // Definimos la distancia inicial de las cámaras a 2 veces el radio máximo de la escena
    camera->camera_ortho_mode = camera_ortho_mode;
    camera->camera_euler_mode = camera_euler_mode;
    camera->zoom = zoom;
    camera->aspect_ratio = initial_aspect_ratio;
}

/**
  * Inicializamos las propiedades de glut
  * Inicializamos también todo lo referente al z-buffer (a excepción del bit de limpieza, establecido en glClear)
  */
void initGL( int argc, const char *argv[] )
{
    glutInit( &argc, ( char ** )argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH ); // Activo doble buffer, colores RGB y buffer de profundidad
    glutInitWindowSize( 800, 600 );

    // Propiedades de la ventana
    glutCreateWindow( "[IDI] 04 Iluminacion" );

    glEnable( GL_DEPTH_TEST ); // Activo algoritmo z-buffer (sólo pintar elementos más cercanos) (tiene que estar después del createWindow)
    glEnable( GL_NORMALIZE ); // Activo normalización de normales para que a pesar de escalar los objetos las normales se mantengan igual.
    glEnable( GL_LIGHTING ); // Activo iluminación
    glEnable( GL_LIGHT0 ); // Activo luz por defecto

    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // Establezco el modo de pintado a "hilo"
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); // Establezco el modo de pintado con relleno
}

/***********************************************************************************************************
************************************************************************************************************
************************************************************************************************************
\\\\\\\\\\                         FUNCIONES AUXILIARES DE RENDERIZADO                          ////////////
************************************************************************************************************
************************************************************************************************************
***********************************************************************************************************/

/**
  * Pinta ejes de coordenadas X Y Z en R G B respectivamente
  */
void renderCoordinateAxis()
{
    glDisable( GL_LIGHTING ); // Desactivo iluminación para que no le afecte a los ejes de coordenadas

    glBegin( GL_LINES );
    glColor3f( 1, 0, 0 );
    glVertex3f( -100, 0, 0 );
    glVertex3f( 100, 0, 0 );
    glEnd();

    glBegin( GL_LINES );
    glColor3f( 0, 1, 0 );
    glVertex3f( 0, -100, 0 );
    glVertex3f( 0, 100, 0 );
    glEnd();

    glBegin( GL_LINES );
    glColor3f( 0, 0, 1 );
    glVertex3f( 0, 0, -100 );
    glVertex3f( 0, 0, 100 );
    glEnd();

    glEnable( GL_LIGHTING ); // Vuelvo a activar iluminación
}

/**
  * Pinta un cubo del tamaño y color especificados con la translación indicada.
  * Usada para pintar el suelo
  */
void renderColorCube( Coord size, Coord color, Coord translation )
{
    glColor3f( color.x, color.y, color.z );
    glPushMatrix();
        glTranslated( translation.x, translation.y, translation.z );
        glScaled( size.x, size.y, size.z );
        glutSolidCube( 1 );
    glPopMatrix();
}

/**
  * Pinta un cubo del tamaño y color especificados con la translación indicada.
  */
void renderMaterialCube( GLdouble edge_lenght )
{
    // Componente de luz ambiente. Comunmente baja para poder distinguir cuando le da la luz.
    GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };

    // Componente de luz difusa. Comunmente más alta que la ambiente para poder distinguir cuando le da la luz.
    GLfloat diffuse[] = { 0.5, 0.5, 0.5, 1.0 };

    // Componente de luz especular. Comunmente blanca para poder reflejar todos los colores que le lleguen.
    // Al ser un material pulido la subimos bastante
    GLfloat specular[] = { 0.8, 0.8, 0.8, 1.0 };

    // Componente de brillantez. Al ser un material metálico la subimos bastante
    GLfloat shininess = 100;

    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, ambient );
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specular );
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, shininess );

    glPushMatrix();
        glTranslated( 0.0, edge_lenght / 2.0, 0.0 );
        glutSolidCube( edge_lenght ); // renderizo un cubo de tamaño de aresta 4
    glPopMatrix();
}
/**
  * Pinta una esfera del radio especificado.
  */
void renderMaterialSphere( GLdouble radius )
{
    // Componente de luz ambiente. Comunmente baja para poder distinguir cuando le da la luz.
    GLfloat ambient[] = { 0.2, 0.2, 0.2, 1.0 };

    // Componente de luz difusa. Comunmente más alta que la ambiente para poder distinguir cuando le da la luz.
    GLfloat diffuse[] = { 0.5, 0.5, 0.5, 1.0 };

    // Componente de luz especular. Comunmente blanca para poder reflejar todos los colores que le lleguen.
    // Al ser un material pulido la subimos bastante
    GLfloat specular[] = { 0.8, 0.8, 0.8, 1.0 };

    // Componente de brillantez. Al ser un material metálico la subimos bastante
    GLfloat shininess = 100;

    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, ambient );
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specular );
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, shininess );

    glPushMatrix();
        glTranslated( 0.0, radius, 0.0 );
        glutSolidSphere( radius, 75, 75 );
    glPopMatrix();
}

/**
  * Pinta una esfera de radio y color indicados. Dependiendo del booleano solid la pinta sólida o "a hilos".
  */
void renderColorSphere( GLdouble radius, Coord color, bool solid )
{
    glColor3f( color.x, color.y, color.z );
    glPushMatrix();
        if ( solid )
        {
            glutSolidSphere( radius, 20, 20 );
        }
        else
        {
            glutWireSphere( radius, 20, 20 );
        }
    glPopMatrix();
}

/**
  * Renderiza un polígono para hacer de suelo
  */
void renderPolygonFloor( Coord floor_size )
{
    // Componente de luz ambiente. Comunmente baja para poder distinguir cuando le da la luz.
    GLfloat ambient[] = { 0.0, 0.0, 0.2, 1.0 };

    // Componente de luz difusa. Comunmente más alta que la ambiente para poder distinguir cuando le da la luz.
    GLfloat diffuse[] = { 0.0, 0.0, 0.6, 1.0 };

    // Componente de luz especular. Comunmente blanca para poder reflejar todos los colores que le lleguen.
    // Al ser un material pulido la subimos bastante
    GLfloat specular[] = { 0.8, 0.8, 0.8, 1.0 };

    // Componente de brillantez. Al ser un material metálico la subimos bastante
    GLfloat shininess = 100;

    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, ambient );
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, specular );
    glMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, shininess );

    // Especifico normal hacia arriba
    glNormal3d( 0.0, 1.0, 0.0 );

    glBegin( GL_POLYGON );
        glVertex3f( -floor_size.x/2.0, 0, -floor_size.z/2.0 );
        glVertex3f( floor_size.x/2.0, 0, -floor_size.z/2.0 );
        glVertex3f( floor_size.x/2.0, 0, floor_size.z/2.0 );
        glVertex3f( -floor_size.x/2.0, 0, floor_size.z/2.0 );
    glEnd();
}

/**
  * Calcula la matríz de transformación a aplicar al modelo (transformation_matrix) en base a las modificaciones a realizarle.
  * Tiene en cuenta si lo debemos centrar en el (0, 0, 0) o sobre floor_height.
  * Calcula los vértices de la caja contenedora aplicándole la transformación.
  *
  */
void calcModelTransformation( Model *model, ModelContainer *model_box, Coord *model_center_translation, Coord *model_floor_translation,
                      GLdouble model_scale_factor, bool center_in_0, GLdouble transformation_matrix[16],
                      GLdouble *max_scene_radius, GLdouble y_offset )
{
    // Empiezo matriz de transformación en base a la modificación de cámara realizada previamente
    glPushMatrix();

    // Cargo la identidad (desechando transformaciones de cámara) para poder separar la transformación del objeto de la de la cámara
    glLoadIdentity();

    if ( !center_in_0 )
    {
        glTranslated( 0, y_offset, 0 );
    }

    glScaled( model_scale_factor, model_scale_factor, model_scale_factor ); // Escalo el objeto en base al factor de escala

    // Si quiero centrarlo en el eje de coordenadas, aplico translación en base a model_center_translation
    if ( center_in_0 )
    {
        glTranslated( model_center_translation->x, model_center_translation->y, model_center_translation->z );
    }
    else
    {
        glTranslated( model_floor_translation->x, model_floor_translation->y, model_floor_translation->z );
    }

    glGetDoublev( GL_MODELVIEW_MATRIX, transformation_matrix );

    // Calculo los valores de su caja y esfera contenedoras en base a esta modificación
    calcTranslatedCoord( model_box->max_origin_vertex, &model_box->max_translated_vertex, transformation_matrix );
    calcTranslatedCoord( model_box->min_origin_vertex, &model_box->min_translated_vertex, transformation_matrix );

    // Calculo en centro de la caja transladada
    ( *model_box ).center_translated_vertex = getCenterVertex( ( *model_box ).min_translated_vertex, ( *model_box ).max_translated_vertex );

    // Calculo radio máximo (tiene que ser desde alguno de los dos puntos de la caja contenedora)
    ( *model_box ).max_translated_radius = getCoordRadius( model_box->max_translated_vertex );
    GLdouble tmp_radius = getCoordRadius( model_box->min_translated_vertex );

    // TODO: Revisarlo. Debería calcular radio máximo desde el centro del objeto.
    // Haría falta tener una matriz objetos -> calcular el centro de la escena en base al punto central de todos los objetos
    // -> calcular el radio mínimo que contenga todos los elementos en base al punto más alejado con respecto al punto central de la escena

    if ( model_box->max_translated_radius < tmp_radius )
    {
        model_box->max_translated_radius = tmp_radius;
    }

    if ( *max_scene_radius < model_box->max_translated_radius )
    {
        *max_scene_radius = model_box->max_translated_radius;
    }

    glPopMatrix(); // Quito la última modificación para mantener la transformación cámara
}

/**
  * Pinta el modelo aplicándole la matríz de transformación previamente calculada en el calcModelTransformation
  */
void renderModel( Model *model, GLdouble transformation_matrix[16] )
{
    glPushMatrix(); // Cargo otro elemento en la pila de transformaciones duplicando la transformación de cámara
    glMultMatrixd( transformation_matrix ); // Modifico la transformacioón de cámara aplicándole también las transformaciones de modelo

    // Obtengo todas las caras del modelo
    const vector<Face> &faces = model->faces();

    // Itero por cada cara del objeto
    for ( int face_iteration = 0; face_iteration < faces.size(); ++face_iteration )
    {
        Material previous_material;
        Material material = Materials[faces[face_iteration].mat];

        // Optimización para indicarle a Open GL el material con el que pintar únicamente en los casos en los que el material varíe
        // con respecto al anterior (en el caso del porsche pasa de 7321 veces a 219 :) )
        if ( face_iteration > 0 )
        {
            previous_material = Materials[faces[face_iteration-1].mat];
        }

        if ( face_iteration == 0 || previous_material.name != material.name )
        {
            glMaterialfv( GL_FRONT, GL_AMBIENT, material.ambient );
            glMaterialfv( GL_FRONT, GL_DIFFUSE, material.diffuse );
            glMaterialfv( GL_FRONT, GL_SPECULAR, material.specular );
            glMaterialf( GL_FRONT, GL_SHININESS, material.shininess );
        }

        // Indicamos la normal a la superficie actual para que el modelo de iluminación empírico pueda hacerla servir
        // Si hemos establecido pintar una normal por cara o si el modelo no tiene normales por vértice, pinto normal por cara
        if ( !normal_per_vertex || ( *model ).normals().size() == 0 )
        {
            glNormal3dv( faces[face_iteration].normalC );
        }

        // Como los objetos están almacenados de forma triangular, por cada cara itero por sus tres vértices y los pinto
        glBegin( GL_TRIANGLES );

        for ( int vertex_iteration = 0; vertex_iteration < 3; ++vertex_iteration )
        {
            // Si hemos definido pintar una normal por vértice y el modelo tiene normales por vértice, las aplico
            if ( normal_per_vertex && ( *model ).normals().size() > 0 )
            {
                glNormal3dv( &model->normals()[ faces[face_iteration].n[vertex_iteration] ] );
            }

            glVertex3dv( &model->vertices()[ faces[face_iteration].v[ vertex_iteration ] ] );
        }

        glEnd();
    }

    // Quito la transformación relativa a este modelo de la pila para dejar únicamente la de la cámara
    glPopMatrix();
}

/***********************************************************************************************************
************************************************************************************************************
************************************************************************************************************
\\\\\\\\\\                             CALLBACKS RENDERIZADO OPENGL                             ////////////
************************************************************************************************************
************************************************************************************************************
***********************************************************************************************************/

/**
  * En el renderizado inicial pintamos el triángulo en el buffer trasero y hacemos el swap
  * Equivalente a "función refresh" en los ejercicios de clase
  */
void renderScene( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // Limpiar la pantalla limpiando también el buffer de color y profundidad.

    // No hago nada con respecto a la luz ambiente (LIGHT0) para respetar los valores por defecto
    /*
    // Establezco propiedades de la luz de cámara (LIGHT0) en caso de que esté habilitada
    if ( light0.enabled )
    {
        //glLightfv( GL_LIGHT0, GL_POSITION, light0.position );
        glLightfv( GL_LIGHT0, GL_AMBIENT, light0.ambient );
        glLightfv( GL_LIGHT0, GL_DIFFUSE, light0.diffuse );
        glLightfv( GL_LIGHT0, GL_SPECULAR, light0.specular );
    }
    */

    // Establezco propiedades de la luz de escena (LIGHT1) en caso de que esté habilitada
    if ( light1.enabled )
    {
        glLightfv( GL_LIGHT1, GL_POSITION, light1.position );
        glLightfv( GL_LIGHT1, GL_AMBIENT, light1.ambient );
        glLightfv( GL_LIGHT1, GL_DIFFUSE, light1.diffuse );
        glLightfv( GL_LIGHT1, GL_SPECULAR, light1.specular );
    }

    // Modifica la última matriz de transformación de la pila de transformaciones substituyéndola por la de identidad.
    // (estado inicial de los vértices), con lo que la próxima vez que ejecute una transformación,
    // lo volverá a hacer sobre el estado inicial (así volvemos a dibujar los ejes de coordenadas sin rotarlos)
    //glLoadIdentity();

    renderCoordinateAxis(); // Dibujamos ejes de coordenadas

    //renderColorCube( floor_size, floor_color, floor_translation ); // Renderizo un cubo aplanado (suelo)
    renderPolygonFloor( floor_size ); // Renderizo polígono suelo

    //renderMaterialCube( object_height );
    renderMaterialSphere( object_height / 2.0 );

    // Renderizo el modelo aplicando  transformaciones de rotación
    renderModel( &model_structure, model_transformation_matrix );

    // Renderizo esfera contenedora
    if ( show_container_sphere )
    {
        glDisable( GL_LIGHTING ); // Desactivo iluminación para que no le afecte a la esfera contenedora
        Coord sphere_color;
        sphere_color.x = 0.0;
        sphere_color.y = 0.7;
        sphere_color.z = 0.7;

        renderColorSphere( max_scene_radius, sphere_color, false );

        glEnable( GL_LIGHTING ); // Desactivo iluminación para que no le afecte a la esfera contenedora
    }

    glutSwapBuffers();
}

/**
  * Al redimensionar ventana volvemos a calcular el ancho y alto máximos del triángulo
  */
void reshapeScene( int w_window, int h_window )
{
    int w_viewport = w_window;
    int h_viewport = h_window;
    int x_viewport = 0;
    int y_viewport = 0;

    camera.aspect_ratio = GLdouble ( w_window ) / GLdouble ( h_window );

    cameraControl::initCamera( camera, max_scene_radius );

    glViewport( x_viewport, y_viewport, w_viewport, h_viewport );

    glutPostRedisplay();
}

/**
  * Renderiza la escena en estado idle
  * Simplemente modifica el ángulo a girar para que la próxima vez que se pinte, se haga acumulando este ángulo
  * Llama a glutPostRedisplay
  */
void idleRenderScene( void )
{
    glutPostRedisplay(); // Llamo a postRedisplay para que se ejecute el callback registrado mediante glutDisplayFunc (función renderScene)
}

/***********************************************************************************************************
************************************************************************************************************
************************************************************************************************************
\\\\\\\\\\                               CALLBACKS EVENTOS OPENGL                               ////////////
************************************************************************************************************
************************************************************************************************************
***********************************************************************************************************/

/**
  * Con el click cambiamos color de fondo (callback no registrado para no entrar en conflicto con mouseDragEvent)
  */
void mouseClickEvent( int button, int event, int mouse_x, int mouse_y )
{
    last_mouse_click_position.x = mouse_x;
    last_mouse_click_position.y = mouse_y;
}

/**
  * Al arrastrar el ratón haciendo click cambiamos color de fondo
  */
void mouseDragEvent( int mouse_x, int mouse_y )
{
    // Si está con el ctrl apretado, aumento zoom,
    if ( glutGetModifiers() == GLUT_ACTIVE_CTRL )
    {
        camera.zoom += ( GLdouble ) ( mouse_y - last_mouse_click_position.y ) / glutGet( GLUT_WINDOW_HEIGHT );

        // Zoom mínimo
        if ( camera.zoom < 0.00001 )
        {
            camera.zoom = 0.00001;
        }

        cameraControl::initCamera( camera, max_scene_radius );
    }
    else if ( glutGetModifiers() == GLUT_ACTIVE_SHIFT ) // Si está el shift apretado, modifico los ángulos de visionado
    {
        GLdouble inspect_angle_x = 90.0 * ( GLdouble ) ( mouse_y - last_mouse_click_position.y ) / glutGet( GLUT_WINDOW_HEIGHT );
        GLdouble inspect_angle_y = -90.0 * ( GLdouble ) ( mouse_x - last_mouse_click_position.x ) / glutGet( GLUT_WINDOW_WIDTH );

        GLdouble transformation_matrix[16];
        glGetDoublev( GL_MODELVIEW_MATRIX, transformation_matrix );
        glLoadIdentity();
        glRotated( inspect_angle_x, 1.0, 0.0, 0.0 );
        glRotated( inspect_angle_y, 0.0, 1.0, 0.0 );
        glMultMatrixd( transformation_matrix );
    }
    else // Si no hay modificadores, muevo ángulos de Euler
    {
        camera.euler_angles.x += 180.0 * ( GLdouble ) ( mouse_y - last_mouse_click_position.y ) / glutGet( GLUT_WINDOW_HEIGHT );
        camera.euler_angles.y -= 180.0 * ( GLdouble ) ( mouse_x - last_mouse_click_position.x ) / glutGet( GLUT_WINDOW_WIDTH );

        cameraControl::moveCamera( camera );
    }

    last_mouse_click_position.y = mouse_y;
    last_mouse_click_position.x = mouse_x;

    glutPostRedisplay(); // Marcamos la ventana como "necesita refrescarse"
}

/**
  * Capturamos los eventos de teclado
  */
void keyboardEvent( unsigned char key, int mouse_x, int mouse_y )
{
    if ( 'h' == key ) // h => help
    {
        cout << endl << "******************************************************* Ayuda **********************************************************" << endl << endl;

        cout << "Pulsa la tecla [h] para acceder a esta ayuda (help)." << endl;
        cout << "Pulsa la tecla [Esc] para salir del programa." << endl << endl;

        cout << "/                                                   *** Cámaras ***                                                    \\" << endl;
        cout << "Pulsa la tecla [x] para habilitar el modo de cámara axonométrica/ortogonal (habilitada por defecto)." << endl;
        cout << "Pulsa la tecla [p] para habilitar el modo de cámara perspectiva." << endl << endl;

        cout << "Pulsa la tecla [a] para habilitar el cálculo de la posición de la cámara mediante gluLookAt (habilitado por defecto)." << endl;
        cout << "Pulsa la tecla [e] para habilitar el cálculo de la posición de la cámara mediante Euler." << endl << endl;

        cout << "Pulsa la tecla [m] para cambiar la vista de cámara (Definida por usuario, planta, frente y perfil)." << endl << endl;

        cout << "/                                                   *** Luces ***                                                     \\" << endl;
        cout << "Pulsa la tecla [c] para des/habilitar la luz de cámara (LIGHT0) (habilitada por defecto)." << endl;
        cout << "Pulsa la tecla [f] para des/habilitar la luz de escena (LIGHT1) (deshabilitada por defecto)." << endl;
        cout << "Pulsa la tecla [s] para cambiar la posición de la luz de escena (LIGHT1) (Centro, izquierda atrás, derecha atrás, derecha delante e izquierda delante)." << endl << endl;

    }
    else if ( 27 == key ) // Esc => Exit
    {
        cout << "Bye!" << endl;

        exit( 0 );
    }
    else if ( key == 'x' )
    {
        cout << "Se ha  habilitado el modo de cámara axonométrica/ortogonal." << endl;

        camera.camera_ortho_mode = true; // cámara ortogonal/axonométrica

        cameraControl::initCamera( camera, max_scene_radius ); // Inicializo cámara ortogonal en base a la esferá mínima contenedora de los objetos anteriores
    }
    else if ( key == 'p' )
    {
        cout << "Se ha  habilitado el modo de cámara perspectiva." << endl;

        camera.camera_ortho_mode = false; // cámara perspectiva

        cameraControl::initCamera( camera, max_scene_radius ); // Inicializo cámara ortogonal en base a la esferá mínima contenedora de los objetos anteriores
    }
    else if ( key == 'a' )
    {
        cout << "Se ha  habilitado el cálculo de la posición de la cámara mediante gluLookAt." << endl;

        camera.camera_euler_mode = false; // cálculo de la posición de la cámara mediante gluLookAt

        cameraControl::moveCamera( camera );
    }
    else if ( key == 'e' )
    {
        cout << "Se ha  habilitado el cálculo de la posición de la cámara mediante Euler." << endl;

        camera.camera_euler_mode = true; // cálculo de la posición de la cámara mediante euler

        cameraControl::moveCamera( camera );
    }
    else if ( key == 'm' )
    {
        // Obtengo la siguiente posición de cámara con respecto a la actual.
        // Este método guardará la posición actual en caso de ser la posición definida por el usuario.
        camera.dynamic_camera_position = cameraControl::getNext( camera.dynamic_camera_position, camera.euler_angles, camera.vrp_pos );

        // Calculo los ángulos de la posición a la que he cambiado
        cameraControl::calcEulerAndVrp( camera.dynamic_camera_position, &camera.euler_angles, &camera.vrp_pos );

        // Muevo la cámara a su nueva posición
        cameraControl::moveCamera( camera );

        cout << "Se ha reseteado la vista de cámara a la vista " << camera.dynamic_camera_position << "." << endl;
    }
    else if ( key == 'c' )
    {
        // Si antes estaba habilitada, la deshabilito y viceversa
        if ( light0.enabled )
        {
            cout << "Se ha desactivado la luz de cámara." << endl;
            glDisable( GL_LIGHT0 ); // Desactivo luz de cámara
        }
        else
        {
            cout << "Se ha activado la luz de cámara." << endl;
            glEnable( GL_LIGHT0 ); // Activo luz de cámara
        }

        light0.enabled = !light0.enabled;
    }
    else if ( key == 'f' )
    {
        // Si antes estaba habilitada, la deshabilito y viceversa
        if ( light1.enabled )
        {
            cout << "Se ha desactivado la luz de escena." << endl;
            glDisable( GL_LIGHT1 ); // Desactivo luz de cámara
        }
        else
        {
            cout << "Se ha activado la luz de escena." << endl;
            glEnable( GL_LIGHT1 ); // Activo luz de cámara
        }

        light1.enabled = !light1.enabled;
    }
    else if ( key == 's' )
    {
        // Cambio la posición dinámica de la luz
        light1.dynamic_light_position = lightControl::getNext( light1.dynamic_light_position );

        // Vuelvo a inicializar los parámetros (entre ellos las coordenadas de la posición) de la luz
        // Necesario para que cuando se encienda, se haga en la posición que se ha modificado a pesar de estar apagada
        setLight1Propierties( &light1, floor_size, max_scene_radius, light1.enabled );

        cout << "Se ha posicionado la luz de escena en " << light1.dynamic_light_position << " independientemente de si estaba encendida o no." << endl;
    }
    else
    {
        cout << endl << "Comando no reconocido, pulsa la tecla [h] para obtener ayuda." << endl << endl;
    }
}

/***********************************************************************************************************
************************************************************************************************************
************************************************************************************************************
\\\\\\\\\\                              FUNCIONES PARA EL MODELO                                ////////////
************************************************************************************************************
************************************************************************************************************
***********************************************************************************************************/

/**
  * Calcula la caja contenedora del modelo model en los dos vectores de vértices máximos y mínimos
  * Asumo que los vectores de vértices los estructuro siguiendo 3 parámetros 0=x, 1=y, 2=z
  */
void calcObjectBoxAndSphere( Model *model, ModelContainer *model_box )
{
    // Obtengo todos los vértices del modelo
    const vector<Vertex> &vertexs = ( *model ).vertices();

    // Cargo los valores del primer vértice de la primera cara para inicializar
    ( *model_box ).max_origin_vertex.x = ( *model ).vertices()[0];
    ( *model_box ).max_origin_vertex.y = ( *model ).vertices()[1];
    ( *model_box ).max_origin_vertex.z = ( *model ).vertices()[2];
    ( *model_box ).min_origin_vertex.x = ( *model ).vertices()[0];
    ( *model_box ).min_origin_vertex.y = ( *model ).vertices()[1];
    ( *model_box ).min_origin_vertex.z = ( *model ).vertices()[2];

    // Itero por cada vértice del modelo
    for ( int vertex_iteration = 0; vertex_iteration < vertexs.size(); vertex_iteration += 3 )
    {
        // Obtengo máximos
        ( *model_box ).max_origin_vertex.x = max( ( *model_box ).max_origin_vertex.x, vertexs[ vertex_iteration ] );
        ( *model_box ).max_origin_vertex.y = max( ( *model_box ).max_origin_vertex.y, vertexs[ vertex_iteration + 1 ] );
        ( *model_box ).max_origin_vertex.z = max( ( *model_box ).max_origin_vertex.z, vertexs[ vertex_iteration + 2 ] );

        // Obtengo mínimos
        ( *model_box ).min_origin_vertex.x = min( ( *model_box ).min_origin_vertex.x, vertexs[ vertex_iteration ] );
        ( *model_box ).min_origin_vertex.y = min( ( *model_box ).min_origin_vertex.y, vertexs[ vertex_iteration + 1 ] );
        ( *model_box ).min_origin_vertex.z = min( ( *model_box ).min_origin_vertex.z, vertexs[ vertex_iteration + 2 ] );
    }

    // Calculo en centro de la caja
    ( *model_box ).center_origin_vertex = getCenterVertex( ( *model_box ).min_origin_vertex, ( *model_box ).max_origin_vertex );
}

/**
  * Calculo la translación necesaria a aplicar cada vez que queramos centrar el objeto en el eje de coordenadas
  */
void calcCenterTranslation( ModelContainer *model_box, Coord *model_center_translation )
{
    ( *model_center_translation ).x = - ( ( ( *model_box ).max_origin_vertex.x + ( *model_box ).min_origin_vertex.x ) / 2.0 );
    ( *model_center_translation ).y = - ( ( ( *model_box ).max_origin_vertex.y + ( *model_box ).min_origin_vertex.y ) / 2.0 );
    ( *model_center_translation ).z = - ( ( ( *model_box ).max_origin_vertex.z + ( *model_box ).min_origin_vertex.z ) / 2.0 );
}

/**
  * Calculo la translación necesaria a aplicar cada vez que queramos centrar el objeto en la base del eje de coordenadas
  */
void calcFloorTranslation( ModelContainer *model_box, Coord *model_floor_translation )
{
    ( *model_floor_translation ).x = - ( ( ( *model_box ).max_origin_vertex.x + ( *model_box ).min_origin_vertex.x ) / 2.0 );
    ( *model_floor_translation ).y = - ( *model_box ).min_origin_vertex.y;
    ( *model_floor_translation ).z = - ( ( ( *model_box ).max_origin_vertex.z + ( *model_box ).min_origin_vertex.z ) / 2.0 );
}

/**
  * Calculo el factor de escala a aplicar cada vez que queramos redimensionar el objeto con respecto a su máximo volumen
  * manteniendo el ratio de aspecto
  */
void calcScaleFactor( const ModelContainer *model_box, GLdouble *model_scale_factor, GLdouble max_scale_factor )
{
    *model_scale_factor = max_scale_factor / (
        max(
            ( ( *model_box ).max_origin_vertex.x - ( *model_box ).min_origin_vertex.x ),
            max(
                ( ( *model_box ).max_origin_vertex.y - ( *model_box ).min_origin_vertex.y ),
                ( ( *model_box ).max_origin_vertex.z - ( *model_box ).min_origin_vertex.z )
            )
        )
    );
}

/**
  * Carga el modelo y calcula valores de caja contenedora, translación al origen, translación al suelo y factor de escala
  */
void loadAndCalcObjectData( string object_path, Model *model_structure, ModelContainer *model_box, Coord *model_center_translation,
                            Coord *model_floor_translation )
{
    // Cargo objeto del homer
    ( *model_structure ).load( object_path );

    // Calculo caja contenedora del objeto
    calcObjectBoxAndSphere( model_structure, model_box );

    // Calculo la translación necesaria para mover el objeto al centro
    calcCenterTranslation( model_box, model_center_translation );

    // Calculo la translación necesaria para mover el objeto al centro
    calcFloorTranslation( model_box, model_floor_translation );

    // Calculo el escalado necesario para que se vea todo el homer
    calcScaleFactor( model_box, &model_scale_factor, max_scale_factor );
}

/***********************************************************************************************************
************************************************************************************************************
************************************************************************************************************
\\\\\\\\\\                                         MAIN                                         ////////////
************************************************************************************************************
************************************************************************************************************
***********************************************************************************************************/

/**
  * Carga el homer no centrado y lo centra
  */
int main( int argc, const char *argv[] )
{
    setFloorPropierties( &floor_size, &floor_color, &floor_translation, &max_scene_radius ); // Inicializo suelo

    setLight0Propierties( &light0, max_scene_radius, true ); // Inicializo LIGHT0

    setLight1Propierties( &light1, floor_size, max_scene_radius, false ); // Inicializo LIGHT1

    initGL( argc, argv ); // Inicializo propiedades OpenGL

    // Cargo el modelo y calculo valores de caja contenedora, translación al origen, translación al suelo y factor de escala
    loadAndCalcObjectData( "./Model/porsche.obj", &model_structure, &model_box, &model_center_translation,
                           &model_floor_translation );


    if ( floor_by_cube )
    {
        model_y_offset = floor_size.y + object_height;
    }
    else
    {
        model_y_offset = object_height;
    }

    // Calculo los parámetros para el renderizado del modelo del homer (matriz de transformación) que usaré cada vez que deba renderizarlo
    calcModelTransformation( &model_structure, &model_box, &model_center_translation, &model_floor_translation, model_scale_factor,
                     false, model_transformation_matrix, &max_scene_radius, model_y_offset );

    setCameraPropierties( &camera, 2.0, true, false, 1.0, max_scene_radius, 1.0 ); // Inicializo cámara ortogonal en base a la esferá mínima contenedora de los objetos anteriores

    cameraControl::initCamera( camera, max_scene_radius ); // Inicializo cámara ortogonal en base a la esferá mínima contenedora de los objetos anteriores

    cameraControl::moveCamera( camera ); // Declaro posición cámara ortogonal

    glClearColor( 0.1, 0.1, 0.1, 1.0 ); // Establezo color de fondo de la ventana

    // Registro de callbacks
    glutDisplayFunc( renderScene ); // Render principal
    glutReshapeFunc( reshapeScene ); // Render redimensionado ventana (llama automáticamente al callback de glutDisplayFunc())
    glutIdleFunc( idleRenderScene ); // Callback continuo (no llama a glutDisplayFunc() => llamar a glutPostRedisplay())
    glutKeyboardFunc( keyboardEvent ); // Callback de eventos de teclado
    glutMouseFunc( mouseClickEvent ); // Callback de clicks de ratón
    glutMotionFunc( mouseDragEvent ); // Callback de "arrastrar click" de ratón

    // Main loop
    glutMainLoop();

    return 0;
}
