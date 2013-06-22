#include "utils.hpp"

/**
  * Las librerías de c (operaciones matemáticas) devuelven y necesitan radianes
  * OpenGL trabaja en grados
  *
  * Para pasar de radianes a grados: x * 180.0 / M_PI
  */
GLdouble toDegrees( GLdouble radians )
{
    return radians * 180.0 / M_PI;
}

/**
  * Las librerías de c (operaciones matemáticas) devuelven y necesitan radianes
  * OpenGL trabaja en grados
  *
  * Para pasar de grados a radianes: x * M_PI / 180.0
  */
GLdouble toRadians( GLdouble degrees )
{
    return degrees * M_PI / 180.0;
}

/**
  * Calcula el radio desde el origen de coordenadas hasta una coordenada concreta
  */
GLdouble getCoordRadius( Coord coord )
{
    return (GLdouble) sqrt(
        coord.x * coord.x +
        coord.y * coord.y +
        coord.z * coord.z
    );
}

/**
  * Calcula el radio desde una coordenada central (center_coord) hasta una coordenada concreta (coord)
  */
GLdouble getCoordRadius( Coord coord, Coord center_coord )
{
    return sqrt( pow( coord.x - center_coord.x, 2.0 ) + pow( coord.y - center_coord.y, 2.0 ) + pow( coord.z - center_coord.z, 2.0 ) );
}

/**
  * Calcula el radio desde el origen de coordenadas hasta una coordenada concreta
  */
GLdouble getCubeRadius( Coord coord )
{
    return ( sqrt( coord.x * coord.x + coord.z * coord.z ) ) * 0.5;
}

/**
  * Multiplica un determinado punto/coordenada por una matriz de transformación.
  */
void calcTranslatedCoord( Coord origin_coord, Coord *translated_coord, const GLdouble *transformation_matrix )
{
    translated_coord->x =
        transformation_matrix[0] * origin_coord.x +
        transformation_matrix[1] * origin_coord.y +
        transformation_matrix[2] * origin_coord.z +
        transformation_matrix[3];

    translated_coord->y =
        transformation_matrix[4] * origin_coord.x +
        transformation_matrix[5] * origin_coord.y +
        transformation_matrix[6] * origin_coord.z +
        transformation_matrix[7];

    translated_coord->z =
        transformation_matrix[8] * origin_coord.x +
        transformation_matrix[9] * origin_coord.y +
        transformation_matrix[10] * origin_coord.z +
        transformation_matrix[11];
}

/**
  * Calcula el vértice/coordenada central entre dos puntos.
  */
Coord getCenterVertex( Coord min_vertex, Coord max_vertex )
{
    Coord center;

    center.x = ( max_vertex.x + min_vertex.x ) * 0.5;
    center.y = ( max_vertex.y + min_vertex.y ) * 0.5;
    center.z = ( max_vertex.z + min_vertex.z ) * 0.5;

    return center;
}
