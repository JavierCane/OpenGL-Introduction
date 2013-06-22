#ifndef _MODEL_PROPIERTIES_HPP
#define _MODEL_PROPIERTIES_HPP

#include "structs.hpp"

// Estructura del modelo
Model model_structure;

// Caja contenedora del modelo
ModelContainer model_box;

// Coordenadas de translación para centrar el modelo en el eje de coordenadas
Coord model_center_translation;

// Coordenadas de translación para centrar el modelo en el eje de coordenadas teniendo en cuenta el suelo
Coord model_floor_translation;

// Factor de escalado para que el objeto se vea al máximo
GLdouble model_scale_factor;

// Matriz de transformación con escalado y traslación al origen/base del homer
GLdouble model_transformation_matrix[16];

#endif
