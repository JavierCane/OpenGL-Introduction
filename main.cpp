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
struct Box {
   Coord max, min;
};

//Modelo homer
Model homer_model;
// Caja contenedora del comer
Box homer_box;
// Coordenadas de translación para centrar el homer
Coord homer_center_translation;
// Factor de escalador para que el objeto se vea al máximo
double homer_scale_factor;
// ángulo rotación Homer
double homer_angle = 0;

// ángulo rotación Mercury
double mercury_angle = 0;
// ángulo rotación Venus
double venus_angle = 0;
// ángulo rotación Earth
double earth_angle = 0;
// ángulo rotación Moon
double moon_angle = 0;

// ángulo de rotación del universo ( mod con arrastre de click, first = x, second = y )
pair<double, double> universe_angle = make_pair( 0, 0 );

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

    // Para evitar problemas con el volumen de visión por defecto en OpenGL
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -1., 1., -1., 1., -1., 1. );
    glMatrixMode( GL_MODELVIEW );
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

void renderSun()
{
    glColor3f( 1, 1, 0 ); // Establezco color amarillo para el sol
    glPushMatrix();
        glutSolidSphere( 0.3, 25, 25 ); // Radio, número de líneas horizontales, número de líneas verticales
    glPopMatrix();
}

void renderMercury()
{
    glColor3f( 0, 0, 0.5 );
    glPushMatrix();
        glRotated( mercury_angle, 0, 1, 0 ); // número de grados que tiene que rotar y vector de rotación
        glTranslated( 0.4, 0, 0 );
        glutSolidSphere( 0.05, 25, 25 );
    glPopMatrix();
}

void renderVenus()
{
    glColor3f( 1, 0.1, 0.1 );
    glPushMatrix();
        glRotated( venus_angle, 0, 1, 0 ); // número de grados que tiene que rotar y vector de rotación
        glTranslated( 0.6, 0, 0 );
        glutSolidSphere( 0.09, 25, 25 ); // Radio, número de líneas horizontales, número de líneas verticales
    glPopMatrix();
}

void renderEarthAndMoon()
{
    glColor3f( 0.1, 1, 0.1 );
    glPushMatrix();
        glRotated( earth_angle, 0, 1, 0 ); // número de grados que tiene que rotar y vector de rotación
        glTranslated( 0.7, 0, 0 );
        glutSolidSphere( 0.1, 25, 25 ); // Radio, número de líneas horizontales, número de líneas verticales

        glColor3f( 0.7, 0.7, 0.9 );
        glPushMatrix();
            glRotated( moon_angle, 0, 1, 0 ); // número de grados que tiene que rotar y vector de rotación
            glTranslated( 0.12, 0, 0 );
            glutSolidSphere( 0.03, 25, 25 ); // Radio, número de líneas horizontales, número de líneas verticales
        glPopMatrix();

    glPopMatrix();
}

void renderModel( Model *model, Box *model_box, Coord *model_center_translation, double *model_scale_factor )
{
    glScaled( *model_scale_factor, *model_scale_factor, *model_scale_factor ); // Escalo el objeto en base al factor de escala

    glTranslated( model_center_translation->x, model_center_translation->y, model_center_translation->z );

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
        glRotated( homer_angle, 0.3, 1.8, 0 ); // número de grados que tiene que rotar y vector de rotación
        glTranslated( 0.5, 0, 0 );
        glScaled( 0.2, 0.2, 0.2 ); // Escalo el objeto en base al factor de escala
        renderModel( &homer_model, &homer_box, &homer_center_translation, &homer_scale_factor );
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
    glLoadIdentity();

    drawCoordinateAxis(); // Dibujamos ejes de coordenadas

    GLdouble m[16];
    glGetDoublev( GL_MODELVIEW_MATRIX, m );
    glLoadIdentity();
    //glRotated( universe_angle.first, 1, 0, 0 );
    glRotated( universe_angle.second, 0, 1, 0 );
    glMultMatrixd( m );

    renderSun();
    renderMercury();
    renderVenus();
    renderEarthAndMoon();
    renderHomer();

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
    homer_angle = fmod( 2 + homer_angle, 360 );
    // ángulo rotación Mercury
    mercury_angle = fmod( 0.5 + mercury_angle, 360 );
    // ángulo rotación Venus
    venus_angle = fmod( 0.2 + venus_angle, 360 );
    // ángulo rotación Earth
    earth_angle = fmod( 1 + earth_angle, 360 );
    // ángulo rotación Moon
    moon_angle = fmod( 1.5 + moon_angle, 360 );

    glutPostRedisplay(); // Llamo a postRedisplay para que se ejecute el callback registrado mediante glutDisplayFunc (función renderScene)
}

/**
  * Calcula la caja contenedora del modelo model en los dos vectores de vértices máximos y mínimos
  * Asumo que los vectores de vértices los estructuro siguiendo 3 parámetros 0=x, 1=y, 2=z
  */
void calcObjectBox( Model *model, Box *model_box )
{
    // Obtengo todos los vértices del modelo
    const vector<Vertex> &vertexs = ( *model ).vertices();

    // Cargo los valores del primer vértice de la primera cara para inicializar
    ( *model_box ).max.x = ( *model ).vertices()[0];
    ( *model_box ).max.y = ( *model ).vertices()[1];
    ( *model_box ).max.z = ( *model ).vertices()[2];
    ( *model_box ).min.x = ( *model ).vertices()[0];
    ( *model_box ).min.y = ( *model ).vertices()[1];
    ( *model_box ).min.z = ( *model ).vertices()[2];

    // Itero por cada vértice del modelo
    for ( int vertex_iteration = 0; vertex_iteration < vertexs.size(); vertex_iteration += 3 )
    {
        // Obtengo máximos
        ( *model_box ).max.x = max( ( *model_box ).max.x, vertexs[ vertex_iteration ] );
        ( *model_box ).max.y = max( ( *model_box ).max.y, vertexs[ vertex_iteration + 1 ] );
        ( *model_box ).max.z = max( ( *model_box ).max.z, vertexs[ vertex_iteration + 2 ] );

        // Obtengo mínimos
        ( *model_box ).min.x = min( ( *model_box ).min.x, vertexs[ vertex_iteration ] );
        ( *model_box ).min.y = min( ( *model_box ).min.y, vertexs[ vertex_iteration + 1 ] );
        ( *model_box ).min.z = min( ( *model_box ).min.z, vertexs[ vertex_iteration + 2 ] );
    }

    // DEBUG
    // cout << "x max:" << ( *model_box ).max.x << ", y max: " << ( *model_box ).max.y << ", z max: " << ( *model_box ).max.z << endl;
    // cout << "x min:" << ( *model_box ).min.x << ", y min: " << ( *model_box ).min.y << ", z min: " << ( *model_box ).min.z << endl;
}

/**
  * Calculo la translación necesaria a aplicar cada vez que queramos centrar el objeto en el eje de coordenadas
  */
void calcCenterTranslation( Box *model_box, Coord *model_center_translation )
{
    ( *model_center_translation ).x = - ( ( ( *model_box ).max.x + ( *model_box ).min.x ) / 2 );
    ( *model_center_translation ).y = - ( ( ( *model_box ).max.y + ( *model_box ).min.y ) / 2 );
    ( *model_center_translation ).z = - ( ( ( *model_box ).max.z + ( *model_box ).min.z ) / 2 );
}

/**
  * Calculo el factor de escala a aplicar cada vez que queramos redimensionar el objeto con respecto a su máximo volumen manteniendo el ratio de aspecto
  */
void calcScaleFactor( Box *model_box, double *model_scale_factor )
{
    *model_scale_factor = 2 / (
            max(
                ( ( *model_box ).max.x - ( *model_box ).min.x ),
                max(
                    ( ( *model_box ).max.y - ( *model_box ).min.y ),
                    ( ( *model_box ).max.z - ( *model_box ).min.z )
                    )
                )
                );
}

/**
  * Al arrastrar el ratón haciendo click rotamos todo
  */
void mouseDragEvent( int mouse_x, int mouse_y )
{
    universe_angle.first = fmod( 0.001 * mouse_x + universe_angle.first, 360 );
    universe_angle.second = fmod( 0.01 * mouse_y + universe_angle.second, 360 );
}

/**
  * Carga el homer no centrado y lo centra
  */
int main( int argc, const char *argv[] )
{
    // Inicializar
    initGL( argc, argv );

    // Cargo objeto del homer
    homer_model.load( "./Model/homer.obj" );

    // Calculo caja contenedora del objeto
    calcObjectBox( &homer_model, &homer_box );

    // Calculo la translación necesaria para mover el objeto al centro
    calcCenterTranslation( &homer_box, &homer_center_translation );

    // Calculo el escalado necesario para que se vea todo el homer
    calcScaleFactor( &homer_box, &homer_scale_factor );

    glClearColor( 0.1, 0.1, 0.1, 1.0 );

    // Registro de callbacks
    glutDisplayFunc( renderScene ); // Render principal
    glutReshapeFunc( reshapeScene ); // Render redimensionado ventana (llama automáticamente al callback de glutDisplayFunc())
    glutIdleFunc( idleRenderScene ); // Callback continuo (no llama a glutDisplayFunc() => llamar a glutPostRedisplay())
    glutMotionFunc( mouseDragEvent ); // Callback de "arrastrar click" de ratón

    // Main loop
    glutMainLoop();

    return 0;
}
