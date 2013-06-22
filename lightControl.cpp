// Control de luces

#ifndef _LIGHT_CONTROL
#define _LIGHT_CONTROL

class lightControl
{
public:
    static const int NUM_POSITIONS = 5;

    enum dynamicLightPosition{
        CENTER = 0,         // 0 =>  0, 0,  0
        LEFT_BACK = 1,      // 1 => -x, y, -z
        RIGHT_BACK = 2,     // 2 =>  x, y, -z
        RIGHT_FRONT = 3,    // 3 =>  x, y,  z
        LEFT_FRONT = 4      // 4 => -x, y,  z
    };

    /**
      * Devuelve el siguiente valor del enum
      * Se asume que van a tener valores consecutivos
      */
    static dynamicLightPosition getNext( dynamicLightPosition dynamic_position )
    {
        const int i = static_cast<int> ( dynamic_position ) + 1;

        dynamic_position = static_cast<dynamicLightPosition>( (i) % NUM_POSITIONS );

        return dynamic_position;
    }

    /**
      * Devuelve si la coordenada x debe expresarse negativamente o no.
      * Devuelve un 1 con el signo pertinente para que se pueda multiplicar el resultado de esta operación por la coordenada x directamente
      */
    static float getXSign( dynamicLightPosition dynamic_position )
    {
        switch ( dynamic_position )
        {
            case CENTER:
                return 0.0;
                break;
            case LEFT_BACK:
            case LEFT_FRONT:
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
    static float getZSign( dynamicLightPosition dynamic_position )
    {
        switch ( dynamic_position )
        {
            case CENTER:
                return 0.0;
                break;
            case LEFT_BACK:
            case RIGHT_BACK:
                return -1.0;
                break;
            default:
                return 1.0;
                break;
        }
    }
};

#endif
