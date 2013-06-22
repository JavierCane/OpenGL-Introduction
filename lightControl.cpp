// Control de luces
#include "lightControl.hpp"

lightControl::dynamicLightPosition lightControl::getNext( lightControl::dynamicLightPosition dynamic_light_position )
{
    const int i = static_cast<int> ( dynamic_light_position ) + 1;

    dynamic_light_position = static_cast<lightControl::dynamicLightPosition>( (i) % lightControl::NUM_POSITIONS );

    return dynamic_light_position;
}

/**
  * Devuelve si la coordenada x debe expresarse negativamente o no.
  * Devuelve un 1 con el signo pertinente para que se pueda multiplicar el resultado de esta operación por la coordenada x directamente
  */
GLfloat lightControl::getXSign( lightControl::dynamicLightPosition dynamic_light_position )
{
    switch ( dynamic_light_position )
    {
        case lightControl::CENTER:
            return 0.0;
            break;
        case lightControl::LEFT_BACK:
        case lightControl::LEFT_FRONT:
            return -1.0;
            break;
        default:
            return 1.0;
            break;
    }
}

/**
  * Devuelve si la coordenada z debe expresarse negativamente o no.
  * Devuelve un 1 con el signo pertinente para que se pueda multiplicar el resultado de esta operación por la coordenada z directamente
  */
GLfloat lightControl::getZSign( lightControl::dynamicLightPosition dynamic_light_position )
{
    switch ( dynamic_light_position )
    {
        case lightControl::CENTER:
            return 0.0;
            break;
        case lightControl::LEFT_BACK:
        case lightControl::RIGHT_BACK:
            return -1.0;
            break;
        default:
            return 1.0;
            break;
    }
}
