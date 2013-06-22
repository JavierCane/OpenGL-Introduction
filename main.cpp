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
   Vertex x, y, z;
};
// Estructura de caja contenedora (dos coordenadas, max y min)
struct ModelContainer {
   Coord max_vertex, min_vertex;
   double max_radio;
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
double homer_scale_factor;
// Factor de escalado máximo
double max_scale_factor;
// Ángulo rotación Homer
double homer_angle = 0;

// Grosor en y de la base
double base_height = 0.05;

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
    glutCreateWindow( "[IDI] Intro Glut" );

    glEnable( GL_DEPTH_TEST ); // Activo algoritmo z-buffer (sólo pintar elementos más cercanos) (tiene que estar después del createWindow)

    //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // Establezco el modo de pintado a "hilo"
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); // Establezco el modo de pintado con relleno
}

void initOrthoCamera()
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    // http://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml
    // Distancias desde donde definamos el observador hasta los planos de recorte
    glOrtho(
        -1, // left
        1,  // right
        -1, // bottom
        1,  // top
        0,  // z_near
        2   // z_far
    );
}

void initPerspectiveCamera()
{
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    // http://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml

    gluPerspective(
        -1, // fovy (ángulo de apertura)
        1,  // aspect
        -1, // z_near
        1   // z_far
    );
}

void positionOrthoCamera()
{
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    // http://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml
    //  El observador (OBS) define la posición de la cámara, que mirará en la dirección que establezca el VRP
    //  El vector UP, define la orientación de la cámara, no tiene sentido establecerlo en la misma dirección en la que mira, ya que
    // la orientación realmente se obtiene mediante la proyección del vector de UP en el plano perpendicular a la dirección de visión (recta que une el OBS con VRP).
    // si sólo es un uno (tipo 0, 0, 1) el vector que se establezca a 1 será el que se vea en el eje y
    gluLookAt(
        0, 1, 0, // posición desde dónde miro (obs)
        0, 0, 0, // posición objetivo a mirar (vrp)
        1, 0, 0  // orientación de cámara (up)
    );
}

/**
  * Pinta ejes de coordenadas X Y Z en R G B respectivamente
  */
void drawCoordinateAxis()
{
    glBegin( GL_LINES );
    glColor3f( 1, 0, 0 );
    glVertex3f( -1, 0, 0 );
    glVertex3f( 1, 0, 0 );
    glEnd();

    glBegin( GL_LINES );
    glColor3f( 0, 1, 0 );
    glVertex3f( 0, -1, 0 );
    glVertex3f( 0, 1, 0 );
    glEnd();

    glBegin( GL_LINES );
    glColor3f( 0, 0, 1 );
    glVertex3f( 0, 0, -1 );
    glVertex3f( 0, 0, 1 );
    glEnd();
}

void renderFloor()
{
    glColor3f( 0.7, 0.7, 0.3 );
    glPushMatrix();
        glScaled( 1, base_height, 1 );
        glutSolidCube( 1 );
    glPopMatrix();
}

void renderModel( Model *model, ModelContainer *model_box, Coord *model_center_translation, Coord *model_base_translation, double *model_scale_factor, bool center_in_0 )
{
    glScaled( *model_scale_factor, *model_scale_factor, *model_scale_factor ); // Escalo el objeto en base al factor de escala

    // Si quiero centrarlo en el eje de coordenadas, aplico translación en base a model_center_translation
    if ( center_in_0 )
    {
        glTranslated( model_center_translation->x, model_center_translation->y, model_center_translation->z );
    }
    else
    {
        glTranslated( model_base_translation->x, model_base_translation->y, model_base_translation->z );
    }

    // Obtengo todas las caras del modelo
    const vector<Face> &faces = homer_model.faces();

    // Itero por cada cara del objeto
    for ( int face_iteration = 0; face_iteration < faces.size(); ++face_iteration )
    {
        // Como los objetos están almacenados de forma triangular, por cada cara itero por sus tres vértices y los pinto
        glBegin( GL_TRIANGLES );

        for ( int vertex_iteration = 0; vertex_iteration < 3; ++vertex_iteration )
        {
            glVertex3dv( &homer_model.vertices()[ faces[face_iteration].v[ vertex_iteration ] ] );
        }

        glEnd();
    }
}

void renderHomer()
{
    glColor3f( 0.5, 0.5, 0.5 ); // Establezco color gris para pintar el homer
    glPushMatrix();
        glRotated( homer_angle, 0, 1, 0 ); // número de grados que tiene que rotar y vector de rotación
        glTranslated( 0, homer_box.min_vertex.y, 0 ); // número de grados que tiene que rotar y vector de rotación
        glScaled( 0.2, 0.2, 0.2 ); // Escalo el objeto en base al factor de escala
        renderModel( &homer_model, &homer_box, &homer_center_translation, &homer_base_translation, &homer_scale_factor, false );
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

    renderHomer(); // Renderizo el Homer aplicando  transformaciones de rotación

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

    // Dibujar cuadrado en función de la longitud más grande
    if ( w_window > h_window )
    {
        w_viewport = h_window;
        x_viewport = ( w_window - h_window ) / 2;
    }
    else
    {
        h_viewport = w_window;
        y_viewport = ( h_window - w_window ) / 2;
    }

    glViewport( x_viewport, y_viewport, w_viewport, h_viewport );
}

/**
  * Renderiza la escena en estado idle
  * Simplemente modifica el ángulo a girar para que la próxima vez que se pinte, se haga acumulando este ángulo
  * Llama a glutPostRedisplay
  */
void idleRenderScene( void )
{
    // Modifico el valor del ángulo a girar respecto al que ya había

    // ángulo rotación Homer
    homer_angle = fmod( 4 + homer_angle, 360 );

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
    ( *model_box ).max_vertex.x = ( *model ).vertices()[0];
    ( *model_box ).max_vertex.y = ( *model ).vertices()[1];
    ( *model_box ).max_vertex.z = ( *model ).vertices()[2];
    ( *model_box ).min_vertex.x = ( *model ).vertices()[0];
    ( *model_box ).min_vertex.y = ( *model ).vertices()[1];
    ( *model_box ).min_vertex.z = ( *model ).vertices()[2];
    ( *model_box ).max_radio = (double) sqrt(
        ( *model ).vertices()[0] * ( *model ).vertices()[0] +
        ( *model ).vertices()[1] * ( *model ).vertices()[1] +
        ( *model ).vertices()[2] * ( *model ).vertices()[2]
    );

    // Itero por cada vértice del modelo
    for ( int vertex_iteration = 0; vertex_iteration < vertexs.size(); vertex_iteration += 3 )
    {
        // Obtengo máximos
        ( *model_box ).max_vertex.x = max( ( *model_box ).max_vertex.x, vertexs[ vertex_iteration ] );
        ( *model_box ).max_vertex.y = max( ( *model_box ).max_vertex.y, vertexs[ vertex_iteration + 1 ] );
        ( *model_box ).max_vertex.z = max( ( *model_box ).max_vertex.z, vertexs[ vertex_iteration + 2 ] );

        // Obtengo mínimos
        ( *model_box ).min_vertex.x = min( ( *model_box ).min_vertex.x, vertexs[ vertex_iteration ] );
        ( *model_box ).min_vertex.y = min( ( *model_box ).min_vertex.y, vertexs[ vertex_iteration + 1 ] );
        ( *model_box ).min_vertex.z = min( ( *model_box ).min_vertex.z, vertexs[ vertex_iteration + 2 ] );

        // Calculo radio máximo
        double radio_tmp = (double) sqrt(
            vertexs[ vertex_iteration ] * vertexs[ vertex_iteration ] +
            vertexs[ vertex_iteration + 1 ] * vertexs[ vertex_iteration + 1 ] +
            vertexs[ vertex_iteration + 2 ] * vertexs[ vertex_iteration + 2 ]
        );

        if( ( *model_box ).max_radio < radio_tmp )
        {
            ( *model_box ).max_radio = radio_tmp;
        }
    }
}

/**
  * Calculo la translación necesaria a aplicar cada vez que queramos centrar el objeto en el eje de coordenadas
  */
void calcCenterTranslation( ModelContainer *model_box, Coord *model_center_translation )
{
    ( *model_center_translation ).x = - ( ( ( *model_box ).max_vertex.x + ( *model_box ).min_vertex.x ) / 2 );
    ( *model_center_translation ).y = - ( ( ( *model_box ).max_vertex.y + ( *model_box ).min_vertex.y ) / 2 );
    ( *model_center_translation ).z = - ( ( ( *model_box ).max_vertex.z + ( *model_box ).min_vertex.z ) / 2 );
}

/**
  * Calculo la translación necesaria a aplicar cada vez que queramos centrar el objeto en la base del eje de coordenadas
  */
void calcBaseTranslation( ModelContainer *model_box, Coord *model_base_translation, double y_offset )
{
    ( *model_base_translation ).x = - ( ( ( *model_box ).max_vertex.x + ( *model_box ).min_vertex.x ) / 2 );
    ( *model_base_translation ).y = - ( *model_box ).min_vertex.y + y_offset;
    ( *model_base_translation ).z = - ( ( ( *model_box ).max_vertex.z + ( *model_box ).min_vertex.z ) / 2 );
}

/**
  * Calculo el factor de escala a aplicar cada vez que queramos redimensionar el objeto con respecto a su máximo volumen manteniendo el ratio de aspecto
  */
void calcScaleFactor( ModelContainer *model_box, double *model_scale_factor )
{
    *model_scale_factor = 4 / (
        max(
            ( ( *model_box ).max_vertex.x - ( *model_box ).min_vertex.x ),
            max(
                ( ( *model_box ).max_vertex.y - ( *model_box ).min_vertex.y ),
                ( ( *model_box ).max_vertex.z - ( *model_box ).min_vertex.z )
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
    calcScaleFactor( &homer_box, &homer_scale_factor );
}

/**
  * Carga el homer no centrado y lo centra
  */
int main( int argc, const char *argv[] )
{
    initGL( argc, argv ); // Inicializo propiedades OpenGL

    loadAndCalcObjectData(); // Cargo el modelo y calculo valores de caja contenedora, translación al origen, translación al suelo y factor de escala

    initOrthoCamera(); // Inicializo cámara ortogonal

    positionOrthoCamera(); // Declaro posición cámara ortogonal

    glClearColor( 0.1, 0.1, 0.1, 1.0 ); // Establezo color de fondo de la ventana

    // Registro de callbacks
    glutDisplayFunc( renderScene ); // Render principal
    glutReshapeFunc( reshapeScene ); // Render redimensionado ventana (llama automáticamente al callback de glutDisplayFunc())
    glutIdleFunc( idleRenderScene ); // Callback continuo (no llama a glutDisplayFunc() => llamar a glutPostRedisplay())

    // Main loop
    glutMainLoop();

    return 0;
}
