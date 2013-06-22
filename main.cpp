#if defined(__APPLE__)
    #include <OpenGL/OpenGL.h>
    #include <GLUT/GLUT.h>
#else
    #include <GL/gl.h>
    #include <GL/freeglut.h>
#endif

#include <math.h>
#include <iostream>
#include <vector>

#include "./Model/model.h"

using namespace std;

// Estructura de coordenada
struct Coord {
   GLdouble x, y, z;
};
// Estructura de caja contenedora (dos coordenadas, max y min)
struct ModelContainer {
   Coord max_origin_vertex, min_origin_vertex, max_translated_vertex, min_translated_vertex;
   GLdouble max_origin_radio, max_translated_radio;
};

//Modelo homer
Model homer_model;
// Caja contenedora del comer
ModelContainer homer_box;
// Coordenadas de translación para centrar el homer
Coord homer_center_translation;
// Coordenadas de translación para centrar el homer en la base del eje de coordenadas
Coord homer_base_translation;
// Factor de escalado para que el objeto se vea al máximo
GLdouble homer_scale_factor;
// Matriz de transformación con escalado y traslación al origen/base del homer
GLdouble homer_transformation_matrix[16];

// Grosor en y de la base
GLdouble base_height = 1;

// Factor de escalado máximo
GLdouble max_scale_factor = 4.0;
// Radio máximo de la escena.
GLdouble max_scene_radius = 0.0;
// Relación de aspecto
GLdouble aspect_ratio = 1.0;

// Posición del último click para tenerlo en cuenta a la hora de mover la cámara
Coord last_mouse_click_position;

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

/**
  * Las librerías de c (operaciones matemáticas) devuelven y necesitan radianes
  * OpenGL trabaja en grados
  *
  * Para pasar de radianes a grados: x * 180.0 / M_PI
  */
GLdouble toDegrees( const GLdouble radians )
{
    return radians * 180.0 / M_PI;
}

/**
  * Las librerías de c (operaciones matemáticas) devuelven y necesitan radianes
  * OpenGL trabaja en grados
  *
  * Para pasar de grados a radianes: x * M_PI / 180.0
  */
GLdouble toRadians( const GLdouble degrees )
{
    return degrees * M_PI / 180.0;
}

/**
  * Inicializamos las propiedades de glut
  * Inicializamos también todo lo referente al z-buffer (a excepción del bit de limpieza, establecido en glClear)
  */
void initGL( int argc, const char *argv[] )
{
    glutInit( &argc, ( char ** )argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH ); // Activo doble buffer, colores RGB y buffer de profundidad
    glutInitWindowSize( 600, 600 );

    // Propiedades de la ventana
    glutCreateWindow( "[IDI] 03 Camaras" );

    glEnable( GL_DEPTH_TEST ); // Activo algoritmo z-buffer (sólo pintar elementos más cercanos) (tiene que estar después del createWindow)

    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // Establezco el modo de pintado a "hilo"
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); // Establezco el modo de pintado con relleno
}

void initOrthoCamera( const GLdouble max_scene_radio, const GLdouble aspect_ratio, const GLdouble camera_distance, const GLdouble zoom )
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    GLdouble w_camera = max_scene_radio, h_camera = max_scene_radio;

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
        camera_distance - max_scene_radio,  // z_near
        camera_distance + max_scene_radio   // z_far
    );

    glMatrixMode( GL_MODELVIEW );
}

void initPerspectiveCamera( const GLdouble max_scene_radio, const GLdouble aspect_ratio, const GLdouble camera_distance, const GLdouble zoom )
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    // http://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml

    GLdouble fovy = asin( max_scene_radio / camera_distance ) / zoom;

    if ( aspect_ratio < 1.0 )
    {
        fovy = atan( tan( fovy ) / aspect_ratio );
    }

    fovy = toDegrees( 2.0 * fovy );

    gluPerspective(
        fovy, // fovy (ángulo de apertura)
        aspect_ratio,  // aspect
        camera_distance - max_scene_radio,  // z_near
        camera_distance + max_scene_radio   // z_far
    );

    glMatrixMode( GL_MODELVIEW );
}

void initCamera( const bool camera_ortho_mode, const GLdouble max_scene_radio, const GLdouble aspect_ratio, const GLdouble camera_distance )
{
    if ( camera_ortho_mode )
    {
        initOrthoCamera( max_scene_radio, aspect_ratio, camera_distance, zoom );
    }
    else
    {
        initPerspectiveCamera( max_scene_radio, aspect_ratio, camera_distance, zoom);
    }
}

void positionCamera( const GLdouble max_scene_radio, const bool camera_euler_mode, const Coord euler_angles, const Coord vrp_pos )
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
//            0, 2 * max_scene_radio, 0, // posición desde dónde miro (obs)
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

/**
  * Pinta ejes de coordenadas X Y Z en R G B respectivamente
  */
void drawCoordinateAxis()
{
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
}

void renderFloor()
{
    glColor3f( 0.7, 0.7, 0.3 );
    glPushMatrix();
        glTranslated( 0.0, base_height / 2.0, 0 );
        glScaled( 10, base_height, 10 );
        glutSolidCube( 1 );
    glPopMatrix();
}

void renderSphere( GLdouble radio )
{
    glColor3f( 0.7, 0.0, 0.0 );
    glPushMatrix();
        glutWireSphere( radio, 20, 20 );
    glPopMatrix();
}

/**
  * Calcula el radio desde el origen de coordenadas hasta una coordenada concreta
  */
GLdouble getCoordRadio( const Coord *coord )
{
    return (GLdouble) sqrt(
        coord->x * coord->x +
        coord->y * coord->y +
        coord->z * coord->z
    );
}

/**
  * Multiplica un determinado punto/coordenada por una matriz de transformación.
  */
void calcTranslatedCoord( Coord *origin_coord, Coord *translated_coord, const GLdouble *transformation_matrix )
{
    translated_coord->x =
        transformation_matrix[0] * origin_coord->x +
        transformation_matrix[1] * origin_coord->y +
        transformation_matrix[2] * origin_coord->z +
        transformation_matrix[3];

    translated_coord->y =
        transformation_matrix[4] * origin_coord->x +
        transformation_matrix[5] * origin_coord->y +
        transformation_matrix[6] * origin_coord->z +
        transformation_matrix[7];

    translated_coord->z =
        transformation_matrix[8] * origin_coord->x +
        transformation_matrix[9] * origin_coord->y +
        transformation_matrix[10] * origin_coord->z +
        transformation_matrix[11];
}

void calcRenderModel( Model *model, ModelContainer *model_box, Coord *model_center_translation, Coord *model_base_translation,
                      const GLdouble model_scale_factor, const bool center_in_0, GLdouble transformation_matrix[16], GLdouble *max_scene_radio )
{
    // Empiezo matriz de transformación en base a la modificación de cámara realizada previamente
    glPushMatrix();

    // Cargo la identidad (desechando transformaciones de cámara) para poder separar la transformación del objeto de la de la cámara
    glLoadIdentity();

    if ( !center_in_0 )
    {
        glTranslated( 0, base_height, 0 );
    }

    glScaled( model_scale_factor, model_scale_factor, model_scale_factor ); // Escalo el objeto en base al factor de escala

    // Si quiero centrarlo en el eje de coordenadas, aplico translación en base a model_center_translation
    if ( center_in_0 )
    {
        glTranslated( model_center_translation->x, model_center_translation->y, model_center_translation->z );
    }
    else
    {
        glTranslated( model_base_translation->x, model_base_translation->y, model_base_translation->z );
    }

    glGetDoublev( GL_MODELVIEW_MATRIX, transformation_matrix );

    // Calculo los valores de su caja y esfera contenedoras en base a esta modificación
    calcTranslatedCoord( &model_box->max_origin_vertex, &model_box->max_translated_vertex, transformation_matrix );
    calcTranslatedCoord( &model_box->min_origin_vertex, &model_box->min_translated_vertex, transformation_matrix );

    // Calculo radio máximo (tiene que ser desde alguno de los dos puntos de la caja contenedora)
    ( *model_box ).max_translated_radio = getCoordRadio( &model_box->max_translated_vertex );
    GLdouble radio_tmp = getCoordRadio( &model_box->min_translated_vertex );

    if ( model_box->max_translated_radio < radio_tmp )
    {
        model_box->max_translated_radio = radio_tmp;
    }

    if ( *max_scene_radio < model_box->max_translated_radio )
    {
        *max_scene_radio = model_box->max_translated_radio;
    }

    glPopMatrix(); // Quito la última modificación para mantener la transformación cámara
}

void renderModel( Model *model, GLdouble transformation_matrix[16] )
{
    glPushMatrix(); // Cargo otro elemento en la pila de transformaciones duplicando la transformación de cámara
    glMultMatrixd( transformation_matrix ); // Modifico la transformacioón de cámara aplicándole también las transformaciones de modelo

    // Obtengo todas las caras del modelo
    const vector<Face> &faces = model->faces();

    // Itero por cada cara del objeto
    for ( int face_iteration = 0; face_iteration < faces.size(); ++face_iteration )
    {
        // Como los objetos están almacenados de forma triangular, por cada cara itero por sus tres vértices y los pinto
        glBegin( GL_TRIANGLES );

        for ( int vertex_iteration = 0; vertex_iteration < 3; ++vertex_iteration )
        {
            glVertex3dv( &model->vertices()[ faces[face_iteration].v[ vertex_iteration ] ] );
        }

        glEnd();
    }

    // Quito la transformación relativa a este modelo de la pila para dejar únicamente la de la cámara
    glPopMatrix();
}

/**
  * En el renderizado inicial pintamos el triángulo en el buffer trasero y hacemos el swap
  * Equivalente a "función refresh" en los ejercicios de clase
  */
void renderScene( void )
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // Limpiar la pantalla limpiando también el buffer de color y profundidad.

    // Modifica la última matriz de transformación de la pila de transformaciones substituyéndola por la de identidad.
    // (estado inicial de los vértices), con lo que la próxima vez que ejecute una transformación,
    // lo volverá a hacer sobre el estado inicial (así volvemos a dibujar los ejes de coordenadas sin rotarlos)
    //glLoadIdentity();

    drawCoordinateAxis(); // Dibujamos ejes de coordenadas

    renderFloor(); // Renderizo un cubo aplanado (suelo)

    // Renderizo el Homer aplicando  transformaciones de rotación
    glColor3f( 0.5, 0.5, 0.5 ); // Establezco color gris para pintar el homer
    renderModel( &homer_model, homer_transformation_matrix );

    renderSphere( max_scene_radius );

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

    aspect_ratio = GLdouble ( w_window ) / GLdouble ( h_window );

    initCamera( camera_ortho_mode, max_scene_radius, aspect_ratio, camera_distance );

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

    // Calculo radio máximo (tiene que ser desde alguno de los dos puntos de la caja contenedora)
    ( *model_box ).max_origin_radio = getCoordRadio( &( *model_box ).max_origin_vertex );
    GLdouble radio_tmp = getCoordRadio( &( *model_box ).min_origin_vertex );

    if( ( *model_box ).max_origin_radio < radio_tmp )
    {
        ( *model_box ).max_origin_radio = radio_tmp;
    }
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
void calcBaseTranslation( ModelContainer *model_box, Coord *model_base_translation, GLdouble y_offset )
{
    ( *model_base_translation ).x = - ( ( ( *model_box ).max_origin_vertex.x + ( *model_box ).min_origin_vertex.x ) / 2.0 );
    ( *model_base_translation ).y = - ( *model_box ).min_origin_vertex.y;
    ( *model_base_translation ).z = - ( ( ( *model_box ).max_origin_vertex.z + ( *model_box ).min_origin_vertex.z ) / 2.0 );
}

/**
  * Calculo el factor de escala a aplicar cada vez que queramos redimensionar el objeto con respecto a su máximo volumen manteniendo el ratio de aspecto
  */
void calcScaleFactor( const ModelContainer *model_box, GLdouble *model_scale_factor, const GLdouble max_scale_factor )
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

void loadAndCalcObjectData()
{
    // Cargo objeto del homer
    homer_model.load( "./Model/homer.obj" );

    // Calculo caja contenedora del objeto
    calcObjectBoxAndSphere( &homer_model, &homer_box );

    // Calculo la translación necesaria para mover el objeto al centro
    calcCenterTranslation( &homer_box, &homer_center_translation );

    // Calculo la translación necesaria para mover el objeto al centro
    calcBaseTranslation( &homer_box, &homer_base_translation, base_height );

    // Calculo el escalado necesario para que se vea todo el homer
    calcScaleFactor( &homer_box, &homer_scale_factor, max_scale_factor );
}

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
        zoom += ( GLdouble ) ( mouse_y - last_mouse_click_position.y ) / glutGet( GLUT_WINDOW_HEIGHT );

        if ( zoom < 0.00001 )
        {
            zoom = 0.00001;
        }

        initCamera( camera_ortho_mode, max_scene_radius, aspect_ratio, camera_distance );
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
        euler_angles.x += 180.0 * ( GLdouble ) ( mouse_y - last_mouse_click_position.y ) / glutGet( GLUT_WINDOW_HEIGHT );
        euler_angles.y -= 180.0 * ( GLdouble ) ( mouse_x - last_mouse_click_position.x ) / glutGet( GLUT_WINDOW_WIDTH );

        positionCamera( max_scene_radius, camera_euler_mode, euler_angles, vrp_pos ); // Declaro posición cámara ortogonal
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
        cout << "Ayuda:" << endl << endl;
        cout << "Pulsa la tecla [h] para acceder a esta ayuda (help)." << endl << endl;
        cout << "Pulsa la tecla [x] para habilitar el modo de cámara axonométrica/ortogonal." << endl;
        cout << "Pulsa la tecla [p] para habilitar el modo de cámara perspectiva." << endl << endl;
        cout << "Pulsa la tecla [a] para habilitar el cálculo de la posición de la cámara mediante gluLookAt." << endl;
        cout << "Pulsa la tecla [e] para habilitar el cálculo de la posición de la cámara mediante Euler." << endl;
        cout << "Pulsa la tecla [Esc] para salir del programa." << endl;
    }
    else if ( 27 == key ) // Esc => Exit
    {
        exit( 0 );
    }
    else if ( key == 'x' )
    {
        camera_ortho_mode = true; // cámara ortogonal/axonométrica

        initCamera( camera_ortho_mode, max_scene_radius, aspect_ratio, camera_distance ); // Inicializo cámara ortogonal en base a la esferá mínima contenedora de los objetos anteriores
    }
    else if ( key == 'p' )
    {
        camera_ortho_mode = false; // cámara perspectiva

        initCamera( camera_ortho_mode, max_scene_radius, aspect_ratio, camera_distance ); // Inicializo cámara ortogonal en base a la esferá mínima contenedora de los objetos anteriores
    }
    else if ( key == 'a' )
    {
        camera_euler_mode = false; // cálculo de la posición de la cámara mediante gluLookAt

        positionCamera( max_scene_radius, camera_euler_mode, euler_angles, vrp_pos ); // Declaro posición cámara ortogonal
    }
    else if ( key == 'e' )
    {
        camera_euler_mode = true; // cálculo de la posición de la cámara mediante euler

        positionCamera( max_scene_radius, camera_euler_mode, euler_angles, vrp_pos ); // Declaro posición cámara ortogonal
    }
}

/**
  * Carga el homer no centrado y lo centra
  */
int main( int argc, const char *argv[] )
{
    // Inicializo ángulos de euler para mirar de frente y VRP
    euler_angles.x = vrp_pos.x =
    euler_angles.y = vrp_pos.y =
    euler_angles.z = vrp_pos.z = 0;

    //euler_angles.y = 90;

    initGL( argc, argv ); // Inicializo propiedades OpenGL

    loadAndCalcObjectData(); // Cargo el modelo y calculo valores de caja contenedora, translación al origen, translación al suelo y factor de escala

    // Calculo los parámetros para el renderizado del modelo del homer (matriz de transformación) que usaré cada vez que deba renderizarlo
    calcRenderModel( &homer_model, &homer_box, &homer_center_translation, &homer_base_translation, homer_scale_factor, false, homer_transformation_matrix, &max_scene_radius );

    camera_distance = 2.0 * max_scene_radius; // Definimos la distancia inicial de las cámaras a 2 veces el radio máximo de la escena

    initCamera( camera_ortho_mode, max_scene_radius, aspect_ratio, camera_distance ); // Inicializo cámara ortogonal en base a la esferá mínima contenedora de los objetos anteriores

    positionCamera( max_scene_radius, camera_euler_mode, euler_angles, vrp_pos ); // Declaro posición cámara ortogonal

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
