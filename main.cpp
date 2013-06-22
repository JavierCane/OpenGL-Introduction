#if defined(__APPLE__)
    #include <OpenGL/OpenGL.h>
    #include <GLUT/GLUT.h>
#else
    #include <GL/gl.h>
    #include <GL/freeglut.h>
#endif

#include <iostream>
#include <vector>

#include "./Model/model.h"

using namespace std;

// Ángulo de giro
float angle;

//Modelo homer
Model homer_model;

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

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); // Establezco el modo de pintado a "hilo"

    // Para evitar problemas con el volumen de visión por defecto en OpenGL
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( -1., 1., -1., 1., -1., 1. );
    glMatrixMode( GL_MODELVIEW );
}

/**
  * Capturamos los eventos de teclado
  */
void keyboardEvent( unsigned char key, int mouse_x, int mouse_y )
{
    if ( 'h' == key ) // h => help
    {
        cout << "Ayuda:" << endl;
        cout << "Haz click y desplaza el ratón (sin soltar el botón de click) para cambiar el color de fondo de la ventana." << endl;
        cout << "Pulsa la tecla [h] para acceder a esta ayuda (help)." << endl;
        cout << "Pulsa la tecla [f] para des/habilitar la función del cambio de color de fondo de la ventana." << endl;
        cout << "Pulsa la tecla [Esc] para salir del programa." << endl;
    }
    else if ( 27 == key ) // Esc => Exit
    {
        exit( 0 );
    }
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

void const renderModel()
{
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

    glColor3f( 0.5, 0.5, 0.5 ); // Establezco color gris para dibujar objetos

    // Introducimos en la pila de modificaciones la matriz deseada
    //glRotated( angle * 0.2, 1, 0, 0 ); // Rotamos sobre el eje de las "x" (rota verticalmente hacia delate)
    glRotated( angle, 0, 1, 0 ); // Rotamos sobre el eje de las "y" (rota horizontalmente hacia la derecha)
    //glRotated( angle * 0.2, 0, 0, 1 ); // Rotamos sobre el eje de las "z" (rota en sentido anti-horario)

    renderModel();

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
void indleRenderScene( void )
{
    angle += 2; // Modifico el valor del ángulo a girar respecto al que ya había

    glutPostRedisplay(); // Llamo a postRedisplay para que se ejecute el callback registrado mediante glutDisplayFunc (función renderScene)
}

/**
  * Carga el objeto almacenado en el path object_path a la variable del modelo
  */
void loadObject( string object_path )
{
    homer_model.load( object_path );
}

/**
  * Simplemente carga el objeto ya centrado
  */
int main( int argc, const char *argv[] )
{
    angle = 0;

    // Inicializar
    initGL( argc, argv );

    // Cargar objeto
    loadObject( "./Model/HomerProves.obj" );

    glClearColor( 0.1, 0.1, 0.1, 1.0 );

    // Registro de callbacks
    glutDisplayFunc( renderScene ); // Render principal
    glutReshapeFunc( reshapeScene ); // Render redimensionado ventana (llama automáticamente al callback de glutDisplayFunc())
    glutIdleFunc( indleRenderScene ); // Callback continuo (no llama a glutDisplayFunc() => llamar a glutPostRedisplay())
    glutKeyboardFunc( keyboardEvent ); // Callback de eventos de teclado
    //glutMouseFunc( mouseClickEvent ); // Callback de clicks de ratón (callback no registrado para no entrar en conflicto con mouseDragEvent)
    //glutMotionFunc( mouseDragEvent ); // Callback de "arrastrar click" de ratón

    // Main loop
    glutMainLoop();

    return 0;
}
